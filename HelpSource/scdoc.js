function toggle_visibility(id) {
    var e = document.getElementById(id);
    if(e.style.display == 'none') {
        e.style.display = 'block';
        return true;
    } else {
        e.style.display = 'none';
        return false;
    }
}

function fixTOC() {
// Hide toc if empty    
    var e = document.getElementById("toc");
    if(e) {
        var x = e.getElementsByTagName("ul")[0];
        if(!x.childNodes.length) {
            document.getElementById("toclabel").style.display = 'none';
        }
    }

// make all code examples editable!
    var x = document.getElementsByClassName("lang-sc");
    for(var i=0;i<x.length;i++) {
        x[i].setAttribute("contentEditable",true);
    }
}

function hidemenu(a) {
    var e = document.getElementById("leftpane");
    var t = document.getElementById("toc");
    if(!e.style.left) {
        e.style.left = "-18em";
        if(t)
            t.style.display = 'none';
        a.innerHTML = "&gt;&gt;";
        document.body.style.marginLeft = "2em";
    } else {
        e.style.left = "";
        if(t)
            t.style.display = 'block';
        a.innerHTML = "&lt;&lt;";
        document.body.style.marginLeft = "20em";
    }
    return false;
}
