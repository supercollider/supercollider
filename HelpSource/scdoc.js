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

function eraseCookie(name) {
	createCookie(name,"",-1);
}

function restoreMenu() {
    var a = document.getElementsByClassName("hidetoggle")[0];
    hidemenu(a,readCookie("leftpanestate") || "show");
}

function fixTOC() {
// Hide toc if empty
/*    var e = document.getElementById("toc");
    if(e) {
        var x = e.getElementsByTagName("ul")[0];
        if(!x.childNodes.length) {
            document.getElementById("toclabel").style.display = 'none';
        }
    }*/

// make all code examples editable!
    var x = document.getElementsByClassName("lang-sc");
    for(var i=0;i<x.length;i++) {
        x[i].setAttribute("contentEditable",true);
    }

    restoreMenu();
}

function hidemenu(a,state) {
    var e = document.getElementById("leftpane");
    var t = document.getElementById("toc");
    if(!state) {
        state = !e.style.left ? "hide":"show";
    }

    if(state == "hide") {
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
    createCookie("leftpanestate",state);
    return false;
}
