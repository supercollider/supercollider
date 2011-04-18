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
function popOutTOC(original_toc, p0) {
    var t = original_toc.cloneNode(true);
    t.id = "sidetoc";
    var c = document.getElementsByClassName("contents")[0];
    var left = c.style.marginLeft;
    c.style.marginLeft = "20.5em";
    document.body.insertBefore(t,c);

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
        sessionStorage.popToc = "no";
        return false;
    }
    var x = document.createElement("div");
    x.id = "toctitle";
    x.innerHTML = "Table of contents";
    t.insertBefore(x,p.nextSibling);
    p0.style.display = "none";

    sidetoc = t;
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
    if(!document.getElementById("filename")) return; // hackish trick, only class-docs has a 'filename' div.
    var doc = docmap["Classes/"+document.title];
    if(!doc) return;
    var sups = doc.superclasses;
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
        mets = s.methods;
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
                x.className = "inheritedmets_class";
                x.innerHTML = count+" methods from <a href='"+helpRoot+"/"+s.path+".html'>"+s.title+"</a>: ";
                divs[j].appendChild(x);
                (function(z) {
                    var a = document.createElement("a");
                    a.className = "inheritedmets_toggle";
                    a.setAttribute("href","#");
                    a.innerHTML = "[ show ]";
                    a.onclick = function() {
                        if(z.style.display == "none") {
                            z.style.display = "block";
                            a.innerHTML = "[ hide ]";
                        } else {
                            z.style.display = "none";
                            a.innerHTML = "[ show ]";
                        }
                        return false;
                    };
                    divs[j].appendChild(a);
                })(d[j]);
                divs[j].appendChild(d[j]);
            }
        }
    }
}

function fixTOC() {
// make all code examples editable!
    var x = document.getElementsByClassName("lang-sc");
    for(var i=0;i<x.length;i++) {
        var e = x[i];
        e.setAttribute("contentEditable",true);
/* FIXME: select whole line. hopefully this happens before WebView responds to the key event??
        e.onkeydown = function(ev) {
            if(ev.keycode == 13 && ev.ctrlKey == true || ev.shiftKey == true) {
                // s =  window.getSelection()
                // if(s.isCollapsed) {
                // find nodes at start and end of (or start of next) line
                // s.removeAllRanges();
                // r=document.createRange(); r.setStart(start,0); r.setEnd(end,0); s.addRange(r)
                // }
            }
        }
*/
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

    document.onclick = function() {
        if(openMenu && !inMenu) {
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
        x.appendChild(document.createTextNode(" - "));
        t.style.display = 'none';
        t.onclick = function() {
            t.style.display = 'none';
            openMenu = undefined;
            inMenu = false;
            return true;
        }
        var a = document.createElement("a");
        a.setAttribute("href","#");
        a.innerHTML = "Table of contents &#9660;";
        li.appendChild(a);
        a.onclick = function() {
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

