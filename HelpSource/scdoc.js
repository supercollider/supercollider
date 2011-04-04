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
            e.style.display = 'none';
        }
    }

// make all code examples editable!
    var x = document.getElementsByClassName("lang-sc");
    for(var i=0;i<x.length;i++) {
        x[i].setAttribute("contentEditable",true);
    }
}
