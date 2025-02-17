var categorytree = null;
var path = [];

function GotoPath(p) {
    path = p.split(">");
    var x = escape(p);
    if(window.location.hash != x)
        window.location.hash = x;
    updateTree();
}

function updateTree() {
    var el = document.getElementById("browser");
    var res = "<tr><td>";
    var lev = 0;
    var tree = {entries:[],subcats:categorytree};
    var p;
    var done = 0;
    var sel;
    var colors = {
        "Classes": "#7ab",
        "Reference": "#7b9",
        "Overviews": "#ca6",
        "Guides": "#b87",
        "Tutorials": "#b77",
    };
    link = "";

    while(1) {
        res += "<div class='result'>";
        p=path[lev++];

        var l = [];
        for(var k in tree.subcats)
            l.push(k);
        l = l.sort();

        sel = "";
        for(var i=0;i<l.length;i++) {
            var k = l[i];
            if(k==p) {
                res += "<div class='cat_selected'>";
                sel = k;
            } else
                res += "<div class='category'>";
            res += "<a href='javascript:GotoPath(\""+link+k+"\")'>"+k+"</a>";
            res += " <span class='cat_count'>("+tree.subcats[k].count+")</span>";
            if(k==p)
                res += "<span class='cat_arrow'> &#9658;</span>";
            res += "</div>";
        }

        for(var i=0;i<tree.entries.length;i++) {
            var v = tree.entries[i];
            var x = v.path.split("/");

            res += "<div class='result_doc'><span class='doc_kind' ";
            var clr = colors[x[0]];
            if(clr) {
                res += "style='color:"+clr+";'";
            };
            res += ">";
            if(v.installed=="extension")
                res += "+";
            else if(v.installed=="missing")
                res += "(not installed) ";
            var docLink = v.hasOwnProperty("oldhelp")?v.oldhelp:(v.path+".html");
            res += x[0].toUpperCase()+"</span><a href='"+docLink+"'>"+v.title+"</a></div><div class='result_summary'>"+v.summary+"</div>";
        }

        res += "</div>";

        if(!p)
            break;
        tree = tree.subcats[p];
        link += p+">";
        res += "<td>";
        res += "<div class='cat_header'>"+sel+"</div>";
        if(!tree) {
            res += "<div class='result_summary'>&#9658; Category not found: "+p+"</div>";
            break;
        }
    }
    el.innerHTML = res;
}

function countTree(t) {
    var x = 0;
    for(var k in t.subcats)
        x += countTree(t.subcats[k]);
    x += t.entries.length;
    return t.count = x;
}

function buildCategoryTree() {
    var cats = {};
    for(var k in docmap) {
        var v = docmap[k];

        if(v.installed=="extension" && !check_extensions.checked)
            continue;

        if(filter.value != "all" && v.path.split("/")[0].toLowerCase() != filter.value)
            continue;

        var c2 = v.categories.match(/[^, ]+[^,]*[^, ]+/g) || ["Uncategorized"];
        for(var i=0;i<c2.length;i++) {
            var c = c2[i];
            if(!cats[c])
                cats[c]=[];
            cats[c].push(v);
        }
    }

    var tree = {};
    var p,l,e,a;
    for(var cat in cats) {
        var files = cats[cat];

        p=tree;
        l=cat.split(">");

        for(var i=0;i<l.length;i++) {
            var c = l[i];

            if(!p[c]) {
                p[c]={};
                p[c].subcats = {};
                p[c].entries = [];
                p[c].count = 0;
            }
            e=p[c];
            p=p[c].subcats;
        }

        for(var i=0;i<files.length;i++)
            e.entries.push(files[i]);

        e.entries = e.entries.sort(function(a,b) {
            a=a.title; b=b.title;
            if(a<b) return -1;
            else
            if(a>b) return +1;
            else
            return 0;
        });
    }

    categorytree = tree;

    document.getElementById("total_count").innerHTML = countTree({subcats:tree,entries:[],count:0}) + " documents";
}

var check_extensions;
var filter;

window.onload = function() {
//    restoreMenu();
    helpRoot="."; fixTOC();
    var onChange = function() {
        buildCategoryTree();
        updateTree();
    };

    check_extensions = document.getElementById("check_extensions");
    check_extensions.onchange = onChange;
    filter = document.getElementById("menu_filter");
    filter.onchange = onChange;

    buildCategoryTree();
    GotoPath(unescape(window.location.hash.slice(1)));
}

window.onhashchange = function() {
    GotoPath(unescape(window.location.hash.slice(1)));
}
