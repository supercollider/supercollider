#include "SCDocSearchIndex.h"
#include "SCDoc.h"

#include <sstream>
#include <string.h>
#include <string_view>
#include <yaml-cpp/yaml.h>

namespace {

bool isNode(const DocNode& n, const std::string_view id) { return n.id == id; }

/// Method kind determined by the section it appears in
enum class MethodKind {
    INSTANCE, // inside classmethods::, e.g. .reverse
    CLASS, // inside instancemethods:: e.g. SinOsc.ar
    GENERIC, // outside class docs (e.g. in a guide), e.g. play
};

char anchorPrefix(const MethodKind kind) {
    switch (kind) {
    case MethodKind::INSTANCE:
        return '-';
    case MethodKind::CLASS:
        return '*';
    case MethodKind::GENERIC:
        return '.';
    }
    // generic return value to silence -Wreturn-type
    return '.';
}

// html anchors/urls need to be escaped - i.e. "Foo Bar" needs to become "Foo%20Bar"
// see SCDocHTMLRenderer:escapeSpacesInAnchor
std::string escapeAnchor(std::string_view str) {
    std::string res;
    res.reserve(str.size());
    for (const auto c : str) {
        switch (c) {
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
            res += c;
        }
    }
    return res;
}

// same characters as SCDocHTMLRenderer.binaryOperatorCharacters
constexpr std::string_view binaryOperatorChars = "!@%&*-+=|<>?/";

bool isBinaryOperator(const std::string_view name) {
    return name.find_first_of(binaryOperatorChars) != std::string_view::npos;
}

struct Entry {
    std::string anchor;
    std::string heading;
    bool isMethod = false;
    std::string body;
    std::string code;
};

bool isBlank(const std::string_view str) {
    for (const unsigned char c : str) {
        // we use > here in order to catch any non-printable ascii chars as well
        // see https://en.cppreference.com/cpp/language/ascii
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
    std::vector<Entry> extract(const DocNode* body, const std::string_view title, const std::string_view summary) {
        mEntries.clear();
        mTitle = title;
        beginEntry("", "");
        mCurrent.body += summary;
        mCurrent.body += ' ';
        if (body)
            walkChildren(*body);
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
        if (hasText || mCurrent.isMethod)
            mEntries.push_back(std::move(mCurrent));
        mCurrent = Entry();
    }

    void walkChildren(const DocNode& n) {
        for (int i = 0; i < n.n_childs; i++)
            walk(*n.children[i]);
    }

    void appendBody(const char* text) {
        if (text)
            mCurrent.body += text;
    }

    void walk(const DocNode& n) {
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
            const char* text = n.text ? n.text : "";
            beginEntry(escapeAnchor(text), text);
            walkChildren(n);
        } else if (isNode(n, "CMETHOD")) {
            walkMethod(n, MethodKind::CLASS);
        } else if (isNode(n, "IMETHOD")) {
            walkMethod(n, MethodKind::INSTANCE);
        } else if (isNode(n, "METHOD")) {
            walkMethod(n, MethodKind::GENERIC);
        } else if (isNode(n, NODE_TEXT) || isNode(n, "CODE") || isNode(n, "TELETYPE") || isNode(n, "EMPHASIS")
                   || isNode(n, "STRONG") || isNode(n, "SOFT")) {
            appendBody(n.text);
        } else if (isNode(n, "LINK")) {
            if (n.text)
                mCurrent.body += extractLinkText(n.text);
        } else if (isNode(n, "CODEBLOCK") || isNode(n, "TELETYPEBLOCK")) {
            if (n.text) {
                mCurrent.code += n.text;
                mCurrent.code += '\n';
            }
        } else if (isNode(n, "KEYWORD")) {
            for (int i = 0; i < n.n_childs; i++) {
                mCurrent.body += ' ';
                appendBody(n.children[i]->text);
                mCurrent.body += ' ';
            }
        } else if (isNode(n, "ARGUMENT")) {
            mCurrent.body += ' ';
            if (n.text) {
                mCurrent.body += n.text;
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
    void walkMethod(const DocNode& n, const MethodKind kind) {
        if (n.n_childs < 1 || n.children[0]->n_childs < 1 || !n.children[0]->children[0]->text)
            return;
        const DocNode* names = n.children[0];
        beginEntry(std::string(1, anchorPrefix(kind)).append(names->children[0]->text), "");
        mCurrent.isMethod = true;
        // "method:: ar, kr" becomes the heading "SinOsc.ar, SinOsc.kr"
        for (int i = 0; i < names->n_childs; i++) {
            const char* name = names->children[i]->text;
            if (!name)
                continue;
            if (!mCurrent.heading.empty())
                mCurrent.heading += ", ";
            mCurrent.heading += methodHeading(kind, name);
        }
        // arguments of generic methods (outside class docs)
        if (n.text) {
            mCurrent.body += n.text;
            mCurrent.body += ' ';
        }
        if (n.n_childs > 1)
            walkChildren(*n.children[1]);
    }

    // see SCDocHTMLRenderer:renderMethod (methodCodePrefix)
    std::string methodHeading(const MethodKind kind, const std::string_view name) const {
        std::string heading;
        switch (kind) {
        case MethodKind::CLASS:
            heading.append(mTitle).append(".").append(name);
            break;
        case MethodKind::INSTANCE:
            // binary operator methods are written w/o '.', i.e. simply '+'
            if (!isBinaryOperator(name))
                heading.append(".");
            heading.append(name);
            break;
        case MethodKind::GENERIC:
            heading.append(name);
            break;
        }
        return heading;
    }

    // converts "link::Classes/Pbind::" to "Pbind" for the search index.
    // see also SCDocHTMLRenderer:htmlForLink
    static std::string extractLinkText(const std::string_view link) {
        // split "path#anchor#label", anchor and label are optional
        const auto first = link.find('#');
        const std::string_view path = link.substr(0, first);

        std::string_view anchor;
        std::string_view label;

        if (first != std::string_view::npos) {
            const auto rest = link.substr(first + 1);
            const auto second = rest.find('#');
            anchor = rest.substr(0, second);
            if (second != std::string_view::npos) {
                label = rest.substr(second + 1);
            }
        }

        if (!label.empty())
            return std::string(label);
        // like the renderer, show external links as they are
        if (path.find("://") != std::string::npos)
            return std::string(path);
        // "Classes/SinOsc" -> "SinOsc" (rfind returns npos if there is no slash, and npos + 1 == 0)
        const std::string_view name = path.substr(path.rfind('/') + 1);
        if (anchor.empty())
            return std::string(name);
        if (name.empty())
            return std::string(anchor);

        std::string result;
        result.reserve(name.size() + 2 + anchor.size());
        result.append(name).append(": ").append(anchor);
        return result;
    }

    std::string mTitle;
    Entry mCurrent;
    std::vector<Entry> mEntries;
};

const DocNode* findBody(const DocNode& root) {
    if (isNode(root, "BODY"))
        return &root; // partial parse
    for (int i = 0; i < root.n_childs; i++) {
        if (isNode(*root.children[i], "BODY"))
            return root.children[i];
    }
    return {};
}

// strip repeated whitespaces from string to reduce space of the search index since they are not relevant for the index
std::string stripWhitespace(const std::string_view str) {
    std::string res;
    res.reserve(str.size());
    bool pendingSpace = false;
    for (unsigned char c : str) {
        // we use <= here in order to catch any non-printable ascii chars as well
        // see https://en.cppreference.com/cpp/language/ascii
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
    out << YAML::Key << "installed" << YAML::Value << (doc.isExtension ? "extension" : "standard");
    out << YAML::Key << "body" << YAML::Value << stripWhitespace(entry.body);
    out << YAML::Key << "code" << YAML::Value << stripWhitespace(entry.code);
    out << YAML::EndMap;
}

int emitDocument(YAML::Emitter& out, const SCDocTextSource& doc, const std::string& file, bool isAddition) {
    DocNode* root = scdoc_parse_file(file, isAddition ? SCDOC_PARSE_PARTIAL : SCDOC_PARSE_FULL);
    if (!root)
        return 0;
    const std::vector<Entry> entries =
        TextExtractor().extract(findBody(*root), stripWhitespace(doc.title), isAddition ? "" : doc.summary);
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
