// full-text search over all help documents
// the data comes from doctext.js (written by SCDoc.exportDocTextJS during indexing), which holds one entry
// per section and method.
//
// The search index is built in the browser using elasticlunr.

// documents shown
const ftsMaxDocs = 100;
// sections shown per document before "more"
const ftsSectionsShown = 3;
const ftsTeaserWords = 30;
const ftsFields = { title: { boost: 3 }, heading: { boost: 2 }, body: { boost: 1 } };
const ftsCodeBoost = 0.5;
// shorter terms only match whole words; keep in sync with scdoc.js
const ftsMinPrefixLength = 3;
// documents with a matching title shown before the other results
const ftsMaxPinned = 20;

let ftsIndex = null;
let ftsEntries = null;

// split on whitespace and all ASCII punctuation, so that e.g. "SinOsc.ar" becomes "sinosc" and "ar"
const ftsSeparator = /[\s!-\/:-@\[-`{-~]+/;

function ftsTokenize(str) {
    return str.toLowerCase().split(ftsSeparator).filter((t) => t.length > 0);
}

// the words of the query that elasticlunr indexes, i.e. without stop words
function ftsQueryTerms(query) {
    return ftsTokenize(query).filter((t) => elasticlunr.stopWordFilter(t));
}

function ftsEscapeHtml(str) {
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
}

// same characters as SCDocHTMLRenderer.binaryOperatorCharacters
const ftsBinaryOperatorChars = "!@%&*-+=|<>?/";

// method entries in doctext.js have the method names instead of a heading
function ftsMethodHeading(entry) {
    return entry.method.map((name) => {
        switch (entry.anchor[0]) {
        case "*": return entry.title + "." + name;
        case "-": return [...name].some((c) => ftsBinaryOperatorChars.includes(c)) ? name : "." + name;
        default: return name;
        }
    }).join(", ");
}

function ftsBuildIndex(entries, done, progress) {
    elasticlunr.tokenizer.setSeperator(ftsSeparator);
    const index = elasticlunr(function () {
        this.setRef("ref");
        this.addField("title");
        this.addField("heading");
        this.addField("body");
        this.addField("code");
        this.saveDocument(false);
    });
    let i = 0;
    // build in chunks to keep the page responsive
    const step = () => {
        const end = Math.min(i + 500, entries.length);
        for (; i < end; i++) {
            const e = entries[i];
            if (e.method) e.heading = ftsMethodHeading(e);
            index.addDoc({ ref: i, title: e.title, heading: e.heading, body: e.body, code: e.code });
        }
        if (i < entries.length) {
            progress(i / entries.length);
            setTimeout(step, 0);
        } else {
            done(index);
        }
    };
    step();
}

// returns a list of { ref, score } for entries containing all query terms (in any field).
// terms are prefix-matched (if long enough), like the highlighting in the result pages.
function ftsSearch(terms, options) {
    if (terms.length === 0) return [];
    const fields = options.code ? { ...ftsFields, code: { boost: ftsCodeBoost } } : ftsFields;

    let scores = null;
    for (const term of terms) {
        const res = ftsIndex.search(term, { fields: fields, expand: term.length >= ftsMinPrefixLength });
        const next = new Map();
        for (const r of res) {
            if (scores === null) {
                next.set(r.ref, r.score);
            } else if (scores.has(r.ref)) {
                next.set(r.ref, scores.get(r.ref) + r.score);
            }
        }
        scores = next;
        if (scores.size === 0) break;
    }
    return Array.from(scores, ([ref, score]) => ({ ref: Number(ref), score: score }));
}

// whether the entry passes the document type filter and "include extensions"
function ftsIncluded(entry, options) {
    if (!options.extensions && entry.installed === "extension") return false;
    return options.filter === "all" || entry.path.split("/")[0].toLowerCase() === options.filter;
}

// group hits by document. the documents are ordered by their best section.
function ftsGroupByDoc(hits, options) {
    const groups = new Map();
    for (const hit of hits) {
        const e = ftsEntries[hit.ref];
        if (!ftsIncluded(e, options)) continue;
        let g = groups.get(e.path);
        if (!g) {
            g = { path: e.path, title: e.title, installed: e.installed, score: 0, hits: [] };
            groups.set(e.path, g);
        }
        g.hits.push(hit);
        g.score = Math.max(g.score, hit.score);
    }
    const res = Array.from(groups.values());
    res.forEach((g) => g.hits.sort((a, b) => b.score - a.score));
    res.sort((a, b) => b.score - a.score);
    return res;
}

// moves documents whose title is the query (e.g. SinOsc, or SinOsc.ar) to the top, since
// the scores alone can rank e.g. SinOscFB or a guide that often mentions SinOsc higher.
// For a single word, titles without spaces that start with it follow, shortest first, like
// the quick search in the menubar (In -> In, InTrig, ..., InFeedback).
// Such documents are added even if the search did not find them, e.g. for "In", which is
// removed from the query as a stop word.
function ftsPinTitles(groups, query, options) {
    const q = query.toLowerCase();
    const names = [q, q.replace(/\.\w*$/, "")];
    const isPrefixQuery = /^\w+$/.test(q);
    const rank = (title) => {
        title = title.toLowerCase();
        if (names.includes(title)) return 0;
        if (isPrefixQuery && title.startsWith(q) && !/\s/.test(title)) return title.length;
        return -1;
    };

    // collect all documents with a matching title, keeping their hits if the search found them
    const pinned = new Map();
    for (const g of groups) {
        if (rank(g.title) >= 0) pinned.set(g.path, g);
    }
    // documents the search did not find link to their first entry, usually the page top
    ftsEntries.forEach((e, i) => {
        if (pinned.has(e.path) || rank(e.title) < 0 || !ftsIncluded(e, options)) return;
        pinned.set(e.path, { path: e.path, title: e.title, installed: e.installed, score: 0, hits: [{ ref: i, score: 0 }] });
    });

    const top = Array.from(pinned.values())
        .sort((a, b) => rank(a.title) - rank(b.title) || b.score - a.score || a.title.localeCompare(b.title))
        .slice(0, ftsMaxPinned);
    const topPaths = new Set(top.map((g) => g.path));
    return top.concat(groups.filter((g) => !topPaths.has(g.path)));
}

// returns html for the part of text with the most query term matches, with matches in <mark>.
function ftsMakeTeaser(text, terms) {
    const stems = terms.map(elasticlunr.stemmer);
    const isHit = (token) => elasticlunr.stopWordFilter(token) && (
        terms.some((q) => q.length >= ftsMinPrefixLength ? token.startsWith(q) : token == q)
        || stems.includes(elasticlunr.stemmer(token))
    );
    // the text in doctext.js has single spaces between words
    const words = text.split(" ").map((w) => ({ text: w, hit: ftsTokenize(w).some(isHit) }));

    // find the window with the most hits
    let best = 0, bestCount = 0, count = 0;
    for (let i = 0; i < words.length; i++) {
        if (words[i].hit) count++;
        if (i >= ftsTeaserWords && words[i - ftsTeaserWords].hit) count--;
        if (count > bestCount) {
            bestCount = count;
            best = Math.max(0, i - ftsTeaserWords + 1);
        }
    }
    // start a few words before the first hit in the window, or at the beginning if there is none
    let start = 0;
    if (bestCount > 0) {
        start = best;
        while (!words[start].hit) start++;
        start = Math.max(0, start - 3);
    }
    const end = Math.min(words.length, start + ftsTeaserWords);

    let html = start > 0 ? "&hellip; " : "";
    for (let i = start; i < end; i++) {
        const w = ftsEscapeHtml(words[i].text);
        html += (i > start ? " " : "") + (words[i].hit ? "<mark>" + w + "</mark>" : w);
    }
    if (end < words.length) html += " &hellip;";
    return html;
}

function ftsEntryLink(entry, terms) {
    let link = helpRoot + "/" + entry.path + ".html";
    if (terms.length > 0) link += "?highlight=" + encodeURIComponent(terms.join(" "));
    if (entry.anchor) link += "#" + entry.anchor;
    return link;
}

function ftsRenderResults(groups, terms) {
    const node = $("#text_results").empty();
    const sections = groups.reduce((n, g) => n + g.hits.length, 0);
    $("#text_count").text(
        groups.length + " document" + (groups.length === 1 ? "" : "s") + ", "
        + sections + " section" + (sections === 1 ? "" : "s")
        + (groups.length > ftsMaxDocs ? " (showing the first " + ftsMaxDocs + " documents)" : "")
    );

    for (const g of groups.slice(0, ftsMaxDocs)) {
        const div = $("<div>", { class: "fts_doc" }).appendTo(node);
        const title = $("<div>", { class: "fts_doc_title" }).appendTo(div);
        $("<a>", { href: helpRoot + "/" + g.path + ".html", text: g.title }).appendTo(title);
        $("<span>", { class: "soft", text: " [" + g.path.split("/")[0] + (g.installed === "extension" ? " +" : "") + "]" }).appendTo(title);
        const doc = typeof docmap != "undefined" ? docmap[g.path] : null;
        if (doc && doc.summary) {
            $("<span>", { class: "fts_summary", text: " – " + doc.summary }).appendTo(title);
        }

        g.hits.forEach((hit, i) => {
            const e = ftsEntries[hit.ref];
            const item = $("<div>", { class: "fts_section" }).appendTo(div);
            if (i >= ftsSectionsShown) item.hide().addClass("fts_more");
            $("<a>", { href: ftsEntryLink(e, terms), text: e.heading || e.title }).appendTo(item);
            const text = e.body || e.code;
            if (text) $("<div>", { class: "fts_teaser" }).html(ftsMakeTeaser(text, terms)).appendTo(item);
        });
        if (g.hits.length > ftsSectionsShown) {
            const more = $("<a>", {
                href: "#", class: "fts_show_more",
                text: "… " + (g.hits.length - ftsSectionsShown) + " more in " + g.title,
            }).appendTo(div);
            more.on("click", (event) => {
                event.preventDefault();
                div.find(".fts_more").show();
                more.remove();
            });
        }
    }
}

// searches for the query, shows the results and returns the number of documents found,
// or null if the index is not built yet.
function ftsSearchAndShow(query, options) {
    if (!ftsIndex) return null;
    if (query === "") {
        $("#text_results").empty();
        $("#text_count").text("");
        return 0;
    }
    const terms = ftsQueryTerms(query);
    const groups = ftsPinTitles(ftsGroupByDoc(ftsSearch(terms, options), options), query, options);
    ftsRenderResults(groups, terms);
    return groups.length;
}

// loads doctext.js and builds the index, then calls done().
function ftsLoad(done) {
    // doctext.js is loaded only now, so that the page shows up while it is being read.
    // Note that fetch() does not work for file:// URLs, but <script> does.
    const script = document.createElement("script");
    script.src = "doctext.js";
    script.onload = () => {
        if (typeof doctext == "undefined") return ftsShowLoadError();
        ftsEntries = doctext;
        ftsBuildIndex(ftsEntries, (index) => {
            ftsIndex = index;
            $("#search_status").hide();
            done();
        }, (fraction) => {
            $("#search_status").text("Building search index..." + Math.round(fraction * 100) + "%");
        });
    };
    script.onerror = ftsShowLoadError;
    document.head.appendChild(script);
}

function ftsShowLoadError() {
    $("#search_status").html(
        "Could not load <code>doctext.js</code>. It is written when SuperCollider indexes the help files; "
        + "to create it, evaluate <code>SCDoc.indexAllDocuments</code> in SuperCollider and reload this page."
    ).addClass("fts_error");
}
