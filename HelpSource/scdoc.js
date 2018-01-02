function toggle_visibility(e) {
    if(e.style.display == 'none') {
        e.style.display = 'block';
        return e;
    } else {
        e.style.display = 'none';
        return undefined;
    }
}

var storage;
var sidetoc;
var toc;
var menubar;
var allItems;

function resize_handler() {
    var height = window.innerHeight - menubar.clientHeight - 20;
    if(sidetoc)
        sidetoc.style.height = height;
    if(toc)
        toc.style.maxHeight = height * 0.75;
}

function addInheritedMethods() {
    if(! /\/Classes\/[^\/]+/.test(window.location.pathname)) return; // skip this if not a class doc
    var doc = docmap["Classes/" + scdoc_title];
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
            if(doc.methods.indexOf("_"+m.slice(1))<0) { // ignore methods already documented in this helpfile
                var li = document.createElement("li");
                li.innerHTML = "<a href='"+helpRoot+"/"+s.path+".html#"+encodeURIComponent(m.slice(1))+"'>"+m.slice(2)+"</a>";
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

var code_click_node;
var code_click_pos;

function selectParens(ev) {
    var s =  window.getSelection();
    var r = s.getRangeAt();
    var r2 = document.createRange();
    var j;

    function countChars(char, counterChar, text, count, back)
    {
        var len = text.length;
        var i;
        if(back) {
            for(i=len-1; i>=0; i--) {
                var c = text.charAt(i);
                if(c == char) count--;
                else if(c == counterChar) count++;
                if( count < 1 ) break;
            }
        }
        else {
            for(i=0; i<len; i++) {
                var c = text.charAt(i);
                if(c == char) count--;
                else if(c == counterChar) count++;
                if( count < 1 ) break;
            }
        }
        return [count, i];
    }

    function advance(node, top, back) {
        var childs = node.childNodes;
        if(childs && childs.length) {
            var child = back ? childs[childs.length-1] : childs[0];
            return child;
        }
        else {
            var next;
            if(back) {
                while( node != top && !(next = node.previousSibling) )
                    node = node.parentNode;
            }
            else {
                while( node != top && !(next = node.nextSibling) )
                    node = node.parentNode;
            }
            return next;
        }
    }

    function findChar(char, counterChar, top, node, back, count) {
        do {
            var text = node.nodeValue;
            if(text) {
                var res = countChars(char, counterChar, text, count, back);
                count = res[0];
                if(count < 1) return [node, res[1]];
            }
            node = advance(node, top, back);
        } while(node);

        return null;
    }

    function findAdjacentChar(top, node, pos, chars) {
        var ok = false;
        var text, len;
        var i;

        // try right:
        text = node.nodeValue;
        len = text.length;
        if(text && pos < len) {
            if( (i = chars.indexOf(text[pos])) != -1 )
                return [chars[i], node, pos];
        }
        else {
            var n = node;
            do {
                n = advance(n, top, false);
            } while (n && !(text = n.nodeValue));
            if(n && (len=text.length)) {
                if( (i = chars.indexOf(text[0])) != -1 )
                    return [chars[i], n, 0];
            }
        }

        // try left:
        text = node.nodeValue;
        len = text.length;
        if(text && pos > 0) {
            if( (i = chars.indexOf(text[pos-1])) != -1 )
                return [chars[i], node, pos-1];
        }
        else {
            n = node;
            do {
                n = advance(n, top, true);
            } while (n && !(text = n.nodeValue));
            if(n && (len=text.length)) {
                if( (i = chars.indexOf(text[len-1])) != -1 )
                    return [chars[i], n, len-1];
            }
        }

        return null;
    }

    if(!code_click_node) return;

    var top = ev.target;
    while( top && top.className.indexOf("lang-sc") == -1 )
        top = top.parentNode;
    if(!top) return;

    var pos = code_click_pos;
    var node = code_click_node; code_click_node = null;
    var char;

    var startRes = findAdjacentChar(top, node, pos, "()");
    if(startRes) {
        char = startRes[0];
        node = startRes[1];
        pos = startRes[2];
    }
    else {
        return;
    }

    back = char == ")";
    counterChar = back ? "(" : ")";
    var count = 1, countRes;
    var rnode, rpos;

    // try same node
    var text = node.nodeValue;
    if( back) text = text.substring(0, pos);
    else text = text.substring(pos+1);
    countRes = countChars(counterChar, char, text, count, back);
    count = countRes[0];
    if( count < 1 ) {
        rpos = countRes[1];
        if(!back) rpos += pos + 1;
        rnode = node;
    }
    else {
        // try other nodes
        var n = advance(node, top, back);
        res = findChar(counterChar, char, top, n, back, count);
        if(res) {
            rnode = res[0];
            rpos = res[1];
        }
    }

    if(rnode) {
        if(back) {
            r2.setStart(rnode, rpos);
            r2.setEnd(node, pos+1);
        }
        else {
            r2.setStart(node, pos);
            r2.setEnd(rnode, rpos+1);
        }
        s.removeAllRanges();
        s.addRange(r2);
    }
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
        e.onclick = function(ev) {
            var r =  window.getSelection().getRangeAt(0);
            if(r.collapsed) {
                code_click_node = r.startContainer;
                code_click_pos = r.startOffset;
            }
        };
        e.ondblclick = selectParens;
    }

    addInheritedMethods();

    if('localStorage' in window) {
        storage = window.localStorage;
    } else {
        storage = {};
    }

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
    var nav = ["SuperCollider " + scdoc_sc_version, "Browse", "Search"];
    var url = ["Help.html","Browse.html","Search.html"];
    var nav_item;
    var a;
    for(var i=0;i<nav.length;i++) {
        nav_item = document.createElement("div");
        nav_item.className = "menuitem";
        a = document.createElement("a");
        a.innerHTML = nav[i];
        a.setAttribute("href",helpRoot+"/"+url[i]);
        a.className = "navlink";
        nav_item.appendChild(a);
        bar.appendChild(nav_item);
    }

    nav_item = document.createElement("div");
    nav_item.className = "menuitem";
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
    nav_item.appendChild(a);
    nav_item.appendChild(m1);
    bar.appendChild(nav_item);

    toc = document.getElementById("toc");
    if (toc) {
        allItems = toc.getElementsByTagName("ul")[0].getElementsByTagName("li");
        document.getElementById("toc_search").onkeyup = toc_search;

        toc.style.display = 'none';

        nav_item = document.createElement("div");
        nav_item.className = "menuitem";
        var toc_link = document.createElement("a");
        toc_link.setAttribute("href", "#");
        toc_link.className = "navlink";
        toc_link.innerHTML = "Table of contents";
        toc_link.onclick = function() {
            scroll(0,0);
            return false;
        }
        nav_item.appendChild(toc_link);
        bar.appendChild(nav_item);
        toc_link.onclick = function() {
            document.getElementById("toc_search").focus();
            toggle_toc();
            return false;
        };

        var close_link = document.createElement("a");
        close_link.setAttribute("href","#");
        close_link.className = "close-link";
        close_link.innerHTML = "X";
        close_link.addEventListener("click", function () {
            toggle_toc();
        }, false);
        toc.insertBefore(close_link,toc.firstChild);
        resize_handler();
        if (storage.tocOpen === "yes") {
            show_toc();
        }
    }
    window.onresize = resize_handler;
}

function show_toc() {
    toc.style.display = "block";
    document.querySelector(".contents").style.marginLeft = "20em";
}

function hide_toc() {
    toc.style.display = "none";
    document.querySelector(".contents").style.marginLeft = "0";
}

function toggle_toc() {
    if (storage.tocOpen === "yes") {
        hide_toc();
        storage.tocOpen = "no";
    } else {
        show_toc();
        storage.tocOpen = "yes";
    }
    resize_handler();
}
