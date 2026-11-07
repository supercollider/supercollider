// used in Search.html
// tne search field with two views of the results, "Full text" and the classic "Lookup".
// The query is kept in the URL hash (Search.html#query).
// A tab view can be forde through the GET argument ?view, e.g. `Search.html?view=lookup#query`

const spViews = ["text", "lookup"];
let spView = "text";
let spTimer = null;

function spQuery() {
    return $("#search_input").val();
}

function spTextOptions() {
    return {
        code: $("#check_code").prop("checked"),
        extensions: $("#check_extensions").prop("checked"),
        filter: $("#menu_filter").val(),
    };
}

function spSetTabCount(view, count) {
    $("#" + view + "_tab_count").text(spQuery().trim() == "" ? "" : "(" + count + ")");
}

function spSearchText() {
    const count = ftsSearchAndShow(spQuery().trim(), spTextOptions());
    spSetTabCount("text", count === null ? "…" : count);
}

function spSearchLookup() {
    spSetTabCount("lookup", lookupSearch(spQuery()));
    if (spView == "lookup") lookupPrint();
}

function spSearch() {
    // keep the query in the URL, so that it survives back/forward navigation
    const hash = "#" + encodeURIComponent(spQuery());
    if (window.location.hash != hash && (window.location.hash || spQuery())) {
        history.replaceState(null, "", hash);
    }
    spSearchText();
    spSearchLookup();
}

function spShowView(view, remember) {
    if (!spViews.includes(view)) view = "text";
    spView = view;
    for (const v of spViews) {
        $(".search_view_" + v).toggle(v == view);
    }
    $("#search_tabs button").each(function () {
        $(this).attr("aria-selected", $(this).data("view") == view ? "true" : "false");
    });
    if (remember) {
        try {
            storage.scdocSearchView = view;
        } catch (e) {}
    }
    if (view == "lookup") lookupPrint();
}

// called by randomDoc() in search.js, which shows its result in the lookup view
function spShowRandom() {
    clearTimeout(spTimer);
    history.replaceState(null, "", "#");
    spShowView("lookup");
    ftsSearchAndShow("", spTextOptions());
    spSetTabCount("text", 0);
    spSetTabCount("lookup", 0);
}

function spReadHash() {
    const query = decodeURIComponent(window.location.hash.slice(1));
    if (query != spQuery()) {
        $("#search_input").val(query);
        spSearch();
    }
}

function spOnLoad() {
    helpRoot = ".";
    fixTOC();

    lookupInit(spSearchLookup);
    let view = new URLSearchParams(window.location.search).get("view");
    if (!view) {
        try {
            view = storage.scdocSearchView;
        } catch (e) {}
    }
    spShowView(view);
    $("#search_tabs button").on("click", function () {
        spShowView($(this).data("view"), true);
    });

    $("#search_input").val(decodeURIComponent(window.location.hash.slice(1))).focus();
    spSearch();
    ftsLoad(spSearchText);

    $("#search_input").on("input", () => {
        clearTimeout(spTimer);
        spTimer = setTimeout(spSearch, 150);
    });
    $("#menu_filter, #check_extensions").on("change", spSearch);
    $("#check_code").on("change", spSearchText);
    $(window).on("hashchange", spReadHash);
}
