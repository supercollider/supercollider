function toggle_visibility(e) {
    if(e.style.display == 'none') {
        e.style.display = 'block';
        return e;
    } else {
        e.style.display = 'none';
        return undefined;
    }
}

/*
function createCookie(name,value,days) {
    if (days) {
        var date = new Date();
        date.setTime(date.getTime()+(days*24*60*60*1000));
        var expires = "; expires="+date.toGMTString();
    }
    else var expires = "";
    document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for(var i=0;i < ca.length;i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1,c.length);
        if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
    }
    return null;
}
*/

var sidetoc;
var toc;
var menubar;
var allItems;
function popOutTOC(original_toc, p0) {
    var t = original_toc.cloneNode(true);
    t.id = "sidetoc";
    var c = document.getElementsByClassName("contents")[0];
    var left = c.style.marginLeft;
    c.style.marginLeft = "20.5em";
    document.body.insertBefore(t,c);
    
    t.getElementsByClassName("toc_search")[0].getElementsByTagName("input")[0].onkeyup = toc_search;

    t.style.top = menubar.clientHeight;
    t.style.maxHeight = "none";
    t.style.display = "block";

    var p = t.getElementsByClassName("popoutlink")[0];
    p.innerHTML = "close";
    p.onclick = function() {
        t.parentNode.removeChild(t);
        c.style.marginLeft = left;
        p0.style.display = "";
        sidetoc = null;
        allItems = toc.getElementsByTagName("ul")[0].getElementsByTagName("li");
        sessionStorage.popToc = "no";
        return false;
    }
    var x = document.createElement("div");
    x.id = "toctitle";
    x.innerHTML = "Table of contents";
    t.insertBefore(x,p.nextSibling);
    p0.style.display = "none";

    sidetoc = t;
    allItems = t.getElementsByTagName("ul")[0].getElementsByTagName("li");
    resize_handler();
    sessionStorage.popToc = "yes";
}

function resize_handler() {
    var height = window.innerHeight - menubar.clientHeight - 20;
    if(sidetoc)
        sidetoc.style.height = height;
    if(toc)
        toc.style.maxHeight = height * 0.75;
}

function addInheritedMethods() {
    if(! /\/Classes\/[^\/]+/.test(window.location.pathname)) return; // skip this if not a class doc
    var doc = docmap["Classes/"+document.title];
    if(!doc) return;
    if(doc.implementor) {
        var sups = docmap["Classes/"+doc.implementor].superclasses;
    } else {
        var sups = doc.superclasses;
    }
    if(!sups) return;
    var divs = [document.getElementById("inheritedclassmets"), document.getElementById("inheritedinstmets")];
    for(var i=0;i<sups.length;i++) {
        var s = docmap["Classes/"+sups[i]];
        var d = [];
        for(var j=0;j<2;j++) {
            d[j] = document.createElement("ul");
            d[j].className = "inheritedmets";
            d[j].style.display = "none";
        }
        var mets = s.methods.sort();
        for(var j=0;j<mets.length;j++) {
            var m = mets[j];
            if(doc.methods.indexOf(m)<0) { // ignore methods already documented in this helpfile
                var li = document.createElement("li");
                li.innerHTML = "<a href='"+helpRoot+"/"+s.path+".html#"+m.slice(1)+"'>"+m.slice(2)+"</a>";
                if(m[1]=="*") {
                    d[0].appendChild(li);
                } else
                if(m[1]=="-") {
                    d[1].appendChild(li);
                }
            }
        }
        for(var j=0;j<2;j++) {
            var count = d[j].childElementCount;
            if(count) {
                var x = document.createElement("div");
                var show = "&#9658; show";
                var hide = "&#9660; hide";
                x.className = "inheritedmets_class";
                x.innerHTML = count+" methods from <a href='"+helpRoot+"/"+s.path+".html'>"+s.title+"</a> ";
                divs[j].appendChild(x);
                (function(z) {
                    var a = document.createElement("a");
                    a.className = "inheritedmets_toggle";
                    a.setAttribute("href","#");
                    a.innerHTML = show;
                    a.onclick = function() {
                        if(z.style.display == "none") {
                            z.style.display = "block";
                            a.innerHTML = hide;
                        } else {
                            z.style.display = "none";
                            a.innerHTML = show;
                        }
                        return false;
                    };
                    x.appendChild(a);
                })(d[j]);
                divs[j].appendChild(d[j]);
            }
        }
    }
}

function showAllSubclasses(a) {
    var e = document.getElementById("hiddensubclasses");
    e.style.display = "";
    a.style.display = "none";
}

/*
This key event handler selects the whole line when pressing shift/ctrl-enter with no selection.
But the problem is that it does not update the selection sent to the client.
This is probably because the WebView catches the key event before javascript does!
A fix might be to expose a function to JS that evaluates selection, and call it here.
Or can the WebView make sure that JS has responded to all key events before getting the selection?
*/
function selectLine() {
    var s =  window.getSelection();
    var r = s.getRangeAt();

    function findleft(p) {
        var y, j;
        while(p) {
            if(p.nodeName == "BR")
                return [p,j];
            if(p.childNodes.length>0) {
                for(var i=p.childNodes.length-1;i>=0;i--) {
                    y = findleft(p.childNodes[i]);
                    if(y) return y;
                }
            }
            p = p.previousSibling;
        }
        return null;
    };

    function findright(p) {
        var y, j;
        while(p) {
            if(p.nodeName == "BR")
                return [p,j];
            for(var i=0;i<p.childNodes.length;i++) {
                y = findright(p.childNodes[i]);
                if(y) return y;
            }
            p = p.nextSibling;
        }
        return null;
    };


    if(r.collapsed) {
        var r2 = document.createRange();
        var top = r.startContainer;
        while(top && top.nodeName != "PRE")
            top = top.parentNode;

        var p = r.startContainer;
        while(!p.previousSibling && p != top) {
            p = p.parentNode;
        }
        if(p==top) {
            r2.setStartBefore(top.firstChild);
        } else {
            var found = findleft(p.previousSibling);
            if(found) {
                r2.setStartAfter(found[0]);
            } else {
                r2.setStartBefore(top.firstChild);
            }
        }
        var p = r.startContainer;
        while(!p.nextSibling && p != top) {
            p = p.parentNode;
        }
        if(p==top) {
            r2.setEndAfter(top.lastChild);
        } else {
            var found = findright(p.nextSibling);
            if(found) {
                r2.setEndBefore(found[0]);
            } else {
                r2.setEndAfter(top.lastChild);
            }
        }
        s.removeAllRanges();
        s.addRange(r2);
    }
}

function countChar(str,chr) {
    var x = 0, a, b;
    for(var i=0;i<str.length;i++) {
        if(str[i]==chr) {
            if(a==undefined) a = i;
            b = i;
            x++;
        }
    }
    // return count, first occurence and last occurence
    return [x,a,b];
}

function selectParens(ev) {
    var s =  window.getSelection();
    var r = s.getRangeAt();
    var r2 = document.createRange();
    var j;

    // FIXME: it always selects from the left paren, so clicking on the right-par does not select from the matching left-par
    // need to abort lpar search if rpar was found and then start with the rpar to the right (or closest) instead
    function findlpar(x) {
        var p = x;
        var y, j;
        while(p) {
            if(j = p.nodeValue) {
                j = countChar(j,"(");
                if(j[0]>0) {
                    return [p, j[2]];
                }
            }
            for(var i=0;i<p.childNodes.length;i++) {
                y = findlpar(p.childNodes[i]);
                if(y) return y;
            }
            p = p.previousSibling;
        }
        return null;
    }

    function findrpar(x,count) {
        var p = x;
        var y, j;
        count = count || [0];
        while(p) {
            if(j = p.nodeValue) {
                count[0] += countChar(j,"(")[0];
                j = countChar(j,")");
                if(j[0]>0) {
                    if(count[0]==0)
                        return [p,j[1]];
                    else
                        count[0] -= j[0];
                }
            }
            for(var i=0;i<p.childNodes.length;i++) {
                y = findrpar(p.childNodes[i],count);
                if(y) return y;
            }
            p = p.nextSibling;
        }
        return null;
    }

    var p = r.startContainer;
    if(p.nodeValue && (j = p.nodeValue.indexOf("("))>=0) {
        r2.setStart(p,j+1);
        p = p.parentNode.nextSibling;
    } else {
        while(!p.previousSibling && p != ev.target) {
            p = p.parentNode;
        }
        if(p==ev.target)
            return;
        var found = findlpar(p);
        if(found)
            r2.setStart(found[0],found[1]+1);
    }

    var found = findrpar(p);
    if(found)
        r2.setEnd(found[0],found[1]);

    s.removeAllRanges();
    s.addRange(r2);
}

escape_regexp = function(str) {
  var specials = new RegExp("[.*+?|()\\[\\]{}\\\\]", "g"); // .*+?|()[]{}\
  return str.replace(specials, "\\$&");
}

function toc_search(ev) {
//TODO: on enter, go to first match
    var re = RegExp("^"+escape_regexp(ev.target.value),"i");
    for(var i=0;i<allItems.length;i++) {
        var li = allItems[i];
        var a = li.firstChild;
        if(re.test(a.innerHTML)) {
            li.style.display = "";
            var lev = li.className[3];
            for(var i2 = i-1;i2>=0;i2--) {
                var e = allItems[i2];
                if(e.className[3]<lev) {
                    e.style.display = "";
                    lev -= 1;
                }
                if(e.className[3]==1) break;
            }
        } else {
            li.style.display = "none";
        }
    }
}

function fixTOC() {
    var x = document.getElementsByClassName("lang-sc");
    for(var i=0;i<x.length;i++) {
        var e = x[i];

        // make all code examples editable!
        e.setAttribute("contentEditable",true);

        // select parenthesis on double-click
        e.ondblclick = selectParens;
    }

    addInheritedMethods();

    if(sessionStorage == undefined)
        sessionStorage = {};

    var openMenu;
    var inMenu = false;

    var toggleMenu = function(e) {
        if(openMenu) {
            openMenu.style.display = 'none';
        }
        if(e != openMenu) {
            e.style.display = 'block';
            openMenu = e;
        } else {
            openMenu = undefined;
        }
        inMenu = true;
    }

    document.onclick = function(e) {
        if(openMenu && !inMenu && e.target.id!="toc_search") {
            openMenu.style.display = 'none';
            openMenu = undefined;
        }
        inMenu = false;
        return true;
    }

// make header menu
    var bar = document.getElementById("menubar");
    menubar = bar;
    var nav = ["Home","Browse","Search"];
    var url = ["Help.html","Browse.html","Search.html"];
    for(var i=0;i<nav.length;i++) {
        var li = document.createElement("li");
        li.className = "menuitem";
        var a = document.createElement("a");
        a.innerHTML = nav[i];
        a.setAttribute("href",helpRoot+"/"+url[i]);
        a.className = "navLink";
        li.appendChild(a);
        bar.appendChild(li);
    }

    var li = document.createElement("li");
    li.className = "menuitem";
    var a = document.createElement("a");
    a.innerHTML = "Indexes &#9660;";
    a.setAttribute("href","#");
    var m1 = document.createElement("div");
    m1.className = "submenu";
    m1.style.display = "none";
    a.onclick = function() {
        toggleMenu(m1);
        return false;
    };
    var nav = ["Documents","Classes","Methods"];
    for(var i=0;i<nav.length;i++) {
        var b = document.createElement("a");
        b.setAttribute("href",helpRoot+"/Overviews/"+nav[i]+".html");
        b.innerHTML = nav[i];
        m1.appendChild(b);
    }
    li.appendChild(a);
    li.appendChild(m1);
    bar.appendChild(li);

    var li = document.createElement("li");
    li.className = "menuitem";
    var x = document.createElement("span");
    x.id = "topdoctitle";
    x.appendChild(document.createTextNode(document.title));
    x.onclick = function() {
        scroll(0,0);
        return false;
    }
    li.appendChild(x)
    bar.appendChild(li);

    var t = document.getElementById("toc");
    toc = t;
    if(t) {
        var div = document.createElement("span");
        div.className = "toc_search";
        var ts = document.createElement("input");
        ts.type = "text";
        ts.id = "toc_search";
        ts.value = "";
        ts.style.border = "1px solid #ddd";
        allItems = toc.getElementsByTagName("ul")[0].getElementsByTagName("li");
        ts.onkeyup = toc_search;
        div.appendChild(document.createTextNode("Find:"));
        div.appendChild(ts);
        t.insertBefore(div,t.firstChild);

        x.appendChild(document.createTextNode(" - "));
        t.style.display = 'none';

        var a = document.createElement("a");
        a.setAttribute("href","#");
        a.innerHTML = "Table of contents &#9660;";
        li.appendChild(a);
        a.onclick = function() {
            ts.focus();
            toggleMenu(t);
            return false;
        };
        li.appendChild(t.parentNode.removeChild(t));
        var p = document.createElement("a");
        p.setAttribute("href","#");
        p.className = "popoutlink";
        p.innerHTML = "pop out";
        p.onclick = function() {
            if(!sidetoc)
                popOutTOC(t,a);
            return false;
        }
        t.insertBefore(p,t.firstChild);
        resize_handler();
        if(sessionStorage.popToc == "yes") {
            popOutTOC(t,a);
        }
    }
    window.onresize = resize_handler;
}

