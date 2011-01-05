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

function showTOC(a) {
    if(toggle_visibility("toc"))
        a.innerHTML = "[hide]";
    else
        a.innerHTML = "[show]";

    return false;
}

window.onload = function() {
    document.getElementById("toc_toggle").innerHTML = "[hide]";

// Hide toc if empty    
    var e = document.getElementById("toc");
    var x = e.getElementsByTagName("ul")[0];
    if(!x.childNodes.length) {
        e.style.display = 'none';
        document.getElementById("toctitle").style.display = 'none';
    }
}
