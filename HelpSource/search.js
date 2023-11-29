var input = "";
var newinput = "";
var checkboxes;
var fields = ["title","summary","categories"];
var filter;
//var docmapdict = {};

escape_regexp = function(str) {
  var specials = new RegExp("[.*+?|()\\[\\]{}\\\\]", "g"); // .*+?|()[]{}\
  return str.replace(specials, "\\$&");
}

function debug(str) {
    document.getElementById("js_error").innerHTML += str + "<br>"
}

var methods;
function buildMethodList() {
    methods = {};
    for(var k in docmap) {
        var doc = docmap[k];
        var mets = doc.methods;
        if(mets) {
            for(var j=0;j<mets.length;j++) {
                var m = mets[j];
                var mname = m.slice(2);
                var pfx = m[1];
                var undoc = m[0]=="?";
                if(!methods.hasOwnProperty(mname))
                    methods[mname] = [];
                if(!methods[mname].push) {
                    debug("Problem with method '"+mname+"' in "+doc.path);
                    debug(methods[mname].toString());
                } else {
                    methods[mname].push({doc:doc,pfx:pfx,undoc:undoc});
                }
            }
        }
    }
}

function getQuerystring(key)
{
    key = key.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
    var regex = new RegExp("[\\?&]"+key+"=([^&#]*)");
    var qs = regex.exec(window.location.href);
    if(qs == null)
        return null;
    else
        return qs[1];
}

function onLoad() {
    helpRoot="."; fixTOC();
    newinput = document.getElementById("search_input").value;

    checkboxes = [];
    for(var i=0;i<fields.length;i++) {
        checkboxes[i] = document.getElementById("check_"+fields[i]);
    }

    var x = ["check_extensions", "check_methods","ignore_case","check_word","check_prefix","check_suffix","check_title","check_summary","check_categories"];
    for(var i=0;i<x.length;i++) {
        document.getElementById(x[i]).onchange = doSearch;
    }

    for(var i=0;i<x.length;i++) {
        var name = x[i];
        var val = getQuerystring(name);
        if(val == "true")
            document.getElementById(name).checked = true;
        else if(val == "false")
            document.getElementById(name).checked = false;
    }

    document.getElementById("check_regexp").onchange = function(e) {
        var x = document.getElementsByClassName("nonregex");
        for(var i=0;i<x.length;i++) {
            x[i].disabled = e.target.checked;
        }
        doSearch();
    }

    document.getElementById("search_input").focus();

    node = document.getElementById("search_results");

    checkHash();

    buildMethodList();

    filter = document.getElementById("menu_filter");
    filter.onchange = doSearch;

    setInterval(delayed_update,250);
}

function checkHash() {
    var x = unescape(window.location.hash.slice(1));
    if(x!="" && newinput!=x) {
        document.getElementById("search_input").value = x;
        newinput = x;
//        window.location.hash = "";
    }
}

function SearchKeyPressed(e) {
    newinput = e.value;
    if(newinput != input) {
        document.getElementById("search_results").innerHTML = "Searching...";
    }
}

/*function updateHash() {
    if(window.location.hash != input)
        window.location.hash = input;
    return true;
}*/

function matchDoc(doc,file,folder,re) {
    doc.matched_kw = null;

    if(filter.value != "all") {
        if(folder.toLowerCase() != filter.value)
            return false;
    }

    if(doc.installed=="extension" && !document.getElementById("check_extensions").checked)
        return false;

    for(var i=0;i<fields.length;i++) {
        var f = fields[i];
        if(checkboxes[i].checked && doc.hasOwnProperty(f) && re.test(doc[f])) {
            doc.match_weight = i;
            return true;
        }
    }

    if(doc.hasOwnProperty('path') && checkboxes[0].checked && re.test(file)) {
        return true;
    }

    if(doc.hasOwnProperty('keywords')) {
        for(var i=0;i<doc.keywords.length;i++) {
            var kw = doc.keywords[i];
            if(re.test(kw)) {
                doc.matched_kw = kw;
                doc.match_weight = fields.length;
                return true;
            }
        }
    }

    return false;
}

var cindex;
var cindex2;
var mindex;
var inProgress = false;
var foundMets;
var mets;
var docs;
var re;
var node;
var subclassCache = {};

function strCmp(a,b) {
    if(a<b) return -1;
    else
    if(a>b) return 1;
    else
    return 0;
}

function getSubClasses(doc) {
    var a = [];
    if(doc.subclasses) {
        for(var i=0;i<doc.subclasses.length;i++) {
            var x = doc.subclasses[i];
            var v = docmap["Classes/"+x];
            a.push(x);
            if(v)
                a = a.concat(getSubClasses(v));
        }
    }
    return a;
}

function doPrint() {
    var now = Date.now();
    inProgress = false;
    for(; cindex<docs.length; cindex++) {
        var k = docs[cindex][0];
        var c = docs[cindex][1];
        if(cindex2==0) {
            var div = document.createElement("div");
            div.className="result_category";
            if(!k) k = "(uncategorized)";
            div.innerHTML = k;
            node.appendChild(div);
        }
        for(;cindex2<c.length;cindex2++) {
            var v = c[cindex2];
            var x = v.path.split("/");
            var div = document.createElement("div");
            div.className = "result_item";

            var n = document.createElement("a");
            if(v.matched_kw) {
                n.setAttribute("href",v.path+".html#kw_"+v.matched_kw);
            } else {
                var link = v.hasOwnProperty("oldhelp")?v.oldhelp:(v.path+".html");
                n.setAttribute("href",link);
            }
            n.innerHTML = v.title;
            div.appendChild(n);
            if(/Undocumented classes/.test(v.categories)) {
                n.className = "undoc";
                div.appendChild(document.createTextNode(" -  "));
            } else {
                div.appendChild(document.createTextNode(" - "+v.summary+" "));
            }

            if(v.installed!="standard") {
                var n = document.createElement("span");
                n.className = "soft";
                var res = "[";
                if(v.installed=="extension")
                    res += "+";
                else if(v.installed=="missing")
                    res += "missing!";
                n.innerHTML = res+"]";

                div.appendChild(n);
            }

            node.appendChild(div);

            if(Date.now()-now > 200) {
                inProgress = true;
                return;
            }

        }
        cindex2=0;
    }

    if(foundMets) {
        if(mindex==0) {
            var div = document.createElement("div");
            div.className="result_category";
            div.innerHTML = "Methods";
            node.appendChild(div);
        }

        for(;mindex<mets.length; mindex++) {
            var mname = mets[mindex][0]; //name
            var div = document.createElement("div");
            div.className = "result_item";
            var x = document.createElement("div");
            x.appendChild(document.createTextNode(mname));
            x.className = "result_met";
            div.appendChild(x);
            var cnt = document.createElement("span");
            cnt.className = "soft";
            cnt.innerHTML = " ("+mets[mindex][1].length+" matches)";
            x.appendChild(cnt);

            var x = mets[mindex][1].sort(function(a,b) { //document list
                    a = a.pfx + a.doc.title;
                    b = b.pfx + b.doc.title;
                    return strCmp(a,b);
            });

            for(var i=0;i<x.length;i++) {
                var m = x[i];
                var v = m.doc;

                var div2 = document.createElement("div");

                div2.className = "met_docs";

                div.appendChild(div2);

                var p = m.pfx;
                if(p==".") p=" ";
                div2.appendChild(document.createTextNode(p+" "));
                var a = document.createElement("a");
                a.setAttribute("href",helpRoot+"/"+v.path+".html#"+m.pfx+mname);
                a.innerHTML = v.title;
                if(m.undoc)
                    a.className = "undoc";
                div2.appendChild(a);

                if(/Undocumented classes/.test(v.categories)) {
                    div2.appendChild(document.createTextNode(" -  "));
                } else {
                    div2.appendChild(document.createTextNode(" - "+v.summary+" "));
                }

                var n = document.createElement("span");
                n.className = "soft";
                var res = "["+v.path.split("/")[0];
                if(v.installed=="extension")
                    res += " +";
                else if(v.installed=="missing")
                    res += " - missing!";
                n.innerHTML = res+"]";
                div2.appendChild(n);
                if(v.path=="Classes/Object") {
                    var n = document.createElement("div");
                    n.className = "met_subclasses";
                    n.appendChild(document.createTextNode("Inherited by: All classes"));
                    div2.appendChild(n);
                } else {
                    if(v.subclasses && v.subclasses.length>0) {
                        var n;
                        if(subclassCache[v.path]) {
                            n = subclassCache[v.path].cloneNode(true);
                        } else {
                            var subs = getSubClasses(v);
                            n = document.createElement("div");
                            var m = n;
                            n.className = "met_subclasses";
                            n.appendChild(document.createTextNode("Inherited by: "));
                            for(var j=0;j<subs.length;j++) {
                                var a = document.createElement("a");
                                a.setAttribute("href", helpRoot+"/Classes/"+subs[j]+".html");
                                if(j==10 && subs.length > 14) {
                                    m = document.createElement("span");
                                    m.className = "hiddenspan";
                                    m.style.display = "none";
                                    n.appendChild(m);
                                    var a2 = document.createElement("a");
                                    a2.setAttribute("href","#");
                                    a2.className = "seemore";
                                    a2.innerHTML = "&hellip; see "+(subs.length-10)+" more";
                                    n.appendChild(document.createTextNode(" "));
                                    n.appendChild(a2);
                                }
                                if(j>0)
                                    m.appendChild(document.createTextNode(", "));
                                a.innerHTML = subs[j];
                                m.appendChild(a);
                            }
                            subclassCache[v.path] = n;
                        }
                        (function() {
                            var a = n.getElementsByClassName("seemore")[0];
                            if(a) {
                                var m = n.getElementsByClassName("hiddenspan")[0];
                                a.onclick = function() {
                                    m.style.display = "";
                                    a.parentNode.removeChild(a);
                                    return false;
                                }
                            }
                        })();
                        div2.appendChild(n);
                    }
                }
            }

            node.appendChild(div);

            if(Date.now()-now > 200) {
                inProgress = true;
                return;
            }
        }
    }
}

function randomDoc() {
    while(node.hasChildNodes())
        node.removeChild(node.lastChild);

    cindex = 0;
    cindex2 = 0;
    mindex = 0;
    foundMets = false;
    mets = [];

    document.getElementById("search_input").value = "";

    var list = [];
    for(var k in docmap) {
        if(docmap.hasOwnProperty(k))
            list.push(docmap[k]);
    }

    for(var i=0;i<100;i++) {
        var v = list[Math.floor(Math.random()*list.length)];
        if(! /Undocumented classes/.test(v.categories)) {
            docs = [[v.path.split("/")[0],[v]]];
            break;
        }
    }

    doPrint();

    document.getElementById("search_count").innerHTML = "random document";
}

function doSearch() {
    var count = 0;

    while(node.hasChildNodes())
        node.removeChild(node.lastChild);

    cindex = 0;
    cindex2 = 0;
    mindex = 0;
    foundMets = false;
    cats = [];
    mets = [];

    if(input!="") {

        // setup regexp
        try {
            var str = input;
            if(!document.getElementById("check_regexp").checked) {
                str = escape_regexp(str); //escape special chars
                if(document.getElementById("check_word").checked)
                    str = "\\b" + str + "\\b";
                if(document.getElementById("check_prefix").checked)
                    str = "^" + str;
                if(document.getElementById("check_suffix").checked)
                    str = str + "$";
            }
            re = RegExp(str,document.getElementById("ignore_case").checked?"i":"");
        } catch(err) {
            document.getElementById("search_count").innerHTML = "Invalid regexp";
            document.getElementById("search_results").innerHTML = err;
            return;
        }

        // search docmap
        docs = {};
        for(var k in docmap) {
            var v = docmap[k];
            var x = v.path.split("/");
            var folder = x[0];
            var file = x[x.length-1];

            if(matchDoc(v,file,folder,re)) {
              if(!docs.hasOwnProperty(folder))
                    docs[folder]=[];
                docs[folder].push(v);
                count++;
            }
        }
        var tmp = []; for(var k in docs) tmp.push(
            [k, docs[k].sort(function(a,b) {
                a = a.title;
                b = b.title;
                return strCmp(a,b);
            })]
        );
        var order = { Classes:1, Reference:2, Overviews:3, Guides:4, Tutorials:5 };
        docs = tmp.sort(function(a,b) {
            a = order[a[0]] || 10;
            b = order[b[0]] || 10;
            return strCmp(a,b);
        });

        // search methods
        var include_ext_classes = document.getElementById("check_extensions").checked;
        var filter_val = filter.value;
        if(document.getElementById("check_methods").checked) {
            for(var mname in methods) {
                var m = methods[mname];
                if(re.test(mname)) {
                    var c = [];
                    for(var i=0;i<m.length;i++) {
                        var doc = m[i].doc;
                        if(!doc) continue;
                        if(doc.installed == "extension" && !include_ext_classes) continue;
                        if(filter_val != "all" && doc.path.split("/")[0].toLowerCase() != filter_val) continue
                        c.push(m[i]);
                    }
                    if(c.length>0) {
                        mets.push([mname,c]);
                        foundMets = true;
                        count++;
                    }
                }
            }
            mets = mets.sort(function(a,b) {
                a = a[0];
                b = b[0];
                return strCmp(a,b);
            });
        }

        doPrint();
    }

    document.getElementById("search_count").innerHTML = count+" results";
}

function delayed_update() {
    if(input != newinput) {
        input = newinput;
        doSearch();
    } else if(inProgress) {
        doPrint();
    }
}
window.onhashchange = checkHash;