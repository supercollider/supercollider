#include "SCDocSearchIndex.h"
#include "SCDoc.h"

#include <sstream>
#include <string.h>
#include <yaml-cpp/yaml.h>

namespace {

bool isNode(const DocNode* n, const char* id) { return strcmp(n->id, id) == 0; }

// html anchors/urls need to be escaped - i.e. "Foo Bar" needs to become "Foo%20Bar"
// see SCDocHTMLRenderer:escapeSpacesInAnchor
std::string escapeAnchor(const char* str) {
    std::string res;
    for (const char* c = str; *c; c++) {
        switch (*c) {
        case ' ':
            res += "%20";
            break;
        case '"':
            res += "%22";
            break;
        case '\'':
            res += "%27";
            break;
        case ',':
            res += "%2C";
            break;
        case '<':
            res += "%3C";
            break;
        case '>':
            res += "%3E";
            break;
        case '`':
            res += "%60";
            break;
        default:
            res += *c;
        }
    }
    return res;
}

struct Entry {
    std::string anchor;
    std::string heading;
    std::vector<std::string> methodNames; // only for methods
    std::string body;
    std::string code;
};

bool isBlank(const std::string& str) {
    for (unsigned char c : str) {
        if (c > ' ')
            return false;
    }
    return true;
}

/**
 * Walks the \c DocNode tree and collects one entry per section and method.
 * Empty sections (e.g. no \c ClassMethods) will be omitted from the output.
 *
 * \details In order to determine if a section has been empty, we store the data in a buffer
 * which will only be written via \c endEntry if the content is non-empty.
 *
 * See also SCDocHTMLRenderer - keep both in sync!
 */
class TextExtractor {
public:
    std::vector<Entry> extract(const DocNode* body, const std::string& summary) {
        mEntries.clear();
        beginEntry("", "");
        mCurrent.body += summary;
        mCurrent.body += ' ';
        if (body)
            walkChildren(body);
        endEntry();
        return std::move(mEntries);
    }

private:
    void beginEntry(std::string anchor, std::string heading) {
        endEntry();
        mCurrent.anchor = std::move(anchor);
        mCurrent.heading = std::move(heading);
    }

    void endEntry() {
        bool hasText = !isBlank(mCurrent.body) || !isBlank(mCurrent.code);
        // methods are always kept, as their name is searchable
        if (hasText || !mCurrent.methodNames.empty())
            mEntries.push_back(std::move(mCurrent));
        mCurrent = Entry();
    }

    void walkChildren(const DocNode* n) {
        for (int i = 0; i < n->n_childs; i++)
            walk(n->children[i]);
    }

    void appendBody(const char* text) {
        if (text)
            mCurrent.body += text;
    }

    void walk(const DocNode* n) {
        if (isNode(n, "DESCRIPTION")) {
            beginEntry("description", "Description");
            walkChildren(n);
        } else if (isNode(n, "EXAMPLES")) {
            beginEntry("examples", "Examples");
            walkChildren(n);
        } else if (isNode(n, "CLASSMETHODS")) {
            beginEntry("classmethods", "Class Methods");
            walkChildren(n);
        } else if (isNode(n, "INSTANCEMETHODS")) {
            beginEntry("instancemethods", "Instance Methods");
            walkChildren(n);
        } else if (isNode(n, "SECTION") || isNode(n, "SUBSECTION") || isNode(n, "SUBSUBSECTION")) {
            const char* text = n->text ? n->text : "";
            beginEntry(escapeAnchor(text), text);
            walkChildren(n);
        } else if (isNode(n, "CMETHOD")) {
            walkMethod(n, "*");
        } else if (isNode(n, "IMETHOD")) {
            walkMethod(n, "-");
        } else if (isNode(n, "METHOD")) {
            walkMethod(n, ".");
        } else if (isNode(n, NODE_TEXT) || isNode(n, "CODE") || isNode(n, "TELETYPE") || isNode(n, "EMPHASIS")
                   || isNode(n, "STRONG") || isNode(n, "SOFT")) {
            appendBody(n->text);
        } else if (isNode(n, "LINK")) {
            if (n->text)
                mCurrent.body += extractLinkText(n->text);
        } else if (isNode(n, "CODEBLOCK") || isNode(n, "TELETYPEBLOCK")) {
            if (n->text) {
                mCurrent.code += n->text;
                mCurrent.code += '\n';
            }
        } else if (isNode(n, "KEYWORD")) {
            for (int i = 0; i < n->n_childs; i++) {
                mCurrent.body += ' ';
                appendBody(n->children[i]->text);
                mCurrent.body += ' ';
            }
        } else if (isNode(n, "ARGUMENT")) {
            mCurrent.body += ' ';
            if (n->text) {
                mCurrent.body += n->text;
                mCurrent.body += ": ";
            }
            walkChildren(n);
        } else if (isNode(n, "MATH") || isNode(n, "IMAGE") || isNode(n, "MATHBLOCK") || isNode(n, "CLASSTREE")
                   || isNode(n, "ANCHOR") || isNode(n, "CPRIVATE") || isNode(n, "IPRIVATE") || isNode(n, "COPYMETHOD")
                   || isNode(n, "CCOPYMETHOD") || isNode(n, "ICOPYMETHOD") || isNode(n, NODE_NL)) {
            // nothing to index for these tags
        } else {
            // block elements: keep words of adjacent blocks apart
            mCurrent.body += ' ';
            walkChildren(n);
            mCurrent.body += ' ';
        }
    }

    // see SCDocHTMLRenderer:renderMethod for the anchors
    void walkMethod(const DocNode* n, const char* indicator) {
        if (n->n_childs < 1 || n->children[0]->n_childs < 1 || !n->children[0]->children[0]->text)
            return;
        const DocNode* names = n->children[0];
        beginEntry(indicator + std::string(names->children[0]->text), "");
        for (int i = 0; i < names->n_childs; i++) {
            if (names->children[i]->text)
                mCurrent.methodNames.push_back(names->children[i]->text);
        }
        // arguments of generic methods (outside class docs)
        if (n->text) {
            mCurrent.body += n->text;
            mCurrent.body += ' ';
        }
        if (n->n_childs > 1)
            walkChildren(n->children[1]);
    }

    // converts "link::Classes/Pbind::" to "Pbind" for the search index.
    // see also SCDocHTMLRenderer:htmlForLink
    static std::string extractLinkText(const char* link) {
        // split "path#anchor#label", anchor and label are optional
        std::vector<std::string> parts;
        std::istringstream stream(link);
        for (std::string part; std::getline(stream, part, '#');)
            parts.push_back(part);
        parts.resize(3);
        const std::string& path = parts[0];
        const std::string& anchor = parts[1];
        const std::string& label = parts[2];

        if (!label.empty())
            return label;
        // like the renderer, show external links as they are
        if (path.find("://") != std::string::npos)
            return link;
        // "Classes/SinOsc" -> "SinOsc" (rfind returns npos if there is no slash, and npos + 1 == 0)
        std::string name = path.substr(path.rfind('/') + 1);
        if (anchor.empty())
            return name;
        return name.empty() ? anchor : name + ": " + anchor;
    }

    Entry mCurrent;
    std::vector<Entry> mEntries;
};

const DocNode* findBody(const DocNode* root) {
    if (isNode(root, "BODY"))
        return root; // partial parse
    for (int i = 0; i < root->n_childs; i++) {
        if (isNode(root->children[i], "BODY"))
            return root->children[i];
    }
    return nullptr;
}

// strip repeated whitespaces from string to reduce space of the search index since they are not relevant for the index
std::string stripWhitespace(const std::string& str) {
    std::string res;
    bool pendingSpace = false;
    for (unsigned char c : str) {
        if (c <= ' ') {
            pendingSpace = !res.empty();
            continue;
        }
        if (pendingSpace)
            res += ' ';
        pendingSpace = false;
        res += c;
    }
    return res;
}

void emitEntry(YAML::Emitter& out, const SCDocTextSource& doc, const Entry& entry) {
    out << YAML::BeginMap;
    out << YAML::Key << "path" << YAML::Value << doc.path;
    out << YAML::Key << "anchor" << YAML::Value << entry.anchor;
    out << YAML::Key << "title" << YAML::Value << stripWhitespace(doc.title);
    out << YAML::Key << "heading" << YAML::Value << stripWhitespace(entry.heading);
    if (!entry.methodNames.empty())
        out << YAML::Key << "method" << YAML::Value << entry.methodNames;
    out << YAML::Key << "installed" << YAML::Value << (doc.isExtension ? "extension" : "standard");
    out << YAML::Key << "body" << YAML::Value << stripWhitespace(entry.body);
    out << YAML::Key << "code" << YAML::Value << stripWhitespace(entry.code);
    out << YAML::EndMap;
}

int emitDocument(YAML::Emitter& out, const SCDocTextSource& doc, const std::string& file, bool isAddition) {
    DocNode* root = scdoc_parse_file(file, isAddition ? SCDOC_PARSE_PARTIAL : SCDOC_PARSE_FULL);
    if (!root)
        return 0;
    std::vector<Entry> entries = TextExtractor().extract(findBody(root), isAddition ? "" : doc.summary);
    doc_node_free_tree(root);
    for (const Entry& entry : entries)
        emitEntry(out, doc, entry);
    return static_cast<int>(entries.size());
}

} // namespace

int scdocExportSearchIndex(const std::vector<SCDocTextSource>& docs, std::ostream& out) {
    out << "doctext = ";
    // @todo once yaml-cpp 0.8 is available across all distros consider using YAML::EscapeAsJson
    YAML::Emitter emitter(out);
    emitter.SetStringFormat(YAML::DoubleQuoted);
    emitter << YAML::Flow << YAML::BeginSeq;
    int count = 0;
    for (const SCDocTextSource& doc : docs) {
        count += emitDocument(emitter, doc, doc.fullPath, false);
        for (const std::string& addition : doc.additions)
            count += emitDocument(emitter, doc, addition, true);
    }
    emitter << YAML::EndSeq;
    out << ";\n";
    return count;
}
