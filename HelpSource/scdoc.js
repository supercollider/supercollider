var storage;
var menubar;
var yPosBeforeClick;
var toggleButtonStatus = false;

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

function create_menubar_item(text, link, post_processing) {
    var a = $("<a>").text(text).addClass("menu-link").attr("href", link);
    var li = $("<li>").addClass("menuitem").append(a);
    $("#nav").append(li);
    if (post_processing) {
        post_processing(a, li);
    }
}

escape_regexp = function(str) {
  var specials = new RegExp("[.*+?|()\\[\\]{}\\\\]", "g"); // .*+?|()[]{}\
  return str.replace(specials, "\\$&");
}

function toggleCodeLineNumbers() {
  var text = toggleButtonStatus ? "show code line num" : "hide code line num";
  var editorTextareas = $("textarea.editor");
  toggleButtonStatus = !toggleButtonStatus;
  if (toggleButtonStatus) {
    editorTextareas.each(function() {
      var codeMirrorInstance = this.editor;
      codeMirrorInstance.setOption("lineNumbers", true);
      $(".CodeMirror").css("padding", "0 1em 0 0");
      $(".CodeMirror-gutters").css("margin-left", "-10px");
      $(".CodeMirror pre").css("padding", "0 4px 0 2px");
      $(".CodeMirror-scrollbar-filler").css("background-color", "transparent");
      $(".CodeMirror-gutter-filler").css("background-color", "transparent");
      $(".CodeMirror-linenumber").css("margin-left", "-10px;").css("padding", "0 3px 0 11px");
    });
  } else {
    editorTextareas.each(function() {
      var codeMirrorInstance = this.editor;
      codeMirrorInstance.setOption("lineNumbers", false);
      $(".CodeMirror").css("padding", "0 1em");
      $(".CodeMirror-gutters").css("margin-left", "0px");
      $(".CodeMirror pre").css("padding", "0 4px");
      $(".CodeMirror-scrollbar-filler").css("background-color", "white");
      $(".CodeMirror-gutter-filler").css("background-color", "white");
      $(".CodeMirror-linenumber").css("margin-left", "0").css("padding", "0 3px 0 5px");
    });
  };
};

var toc_items;
function toc_search(search_string) {
//TODO: on enter, go to first match
    var re = RegExp("^"+escape_regexp(search_string),"i");

    for(var i=0;i<toc_items.length;i++) {
        var li = toc_items[i];
        var a = li.firstChild;
        if(re.test(a.innerHTML)) {
            li.style.display = "";
            var lev = li.className[3];
            for(var i2 = i-1;i2>=0;i2--) {
                var e = toc_items[i2];
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


function set_up_toc() {
    var toc_container = $("<div>", {id: "toc-container"})
        .insertBefore($("#menubar").children().first());

    var toc_link = $("<a>", {
        href: "#",
        class: "menu-link toc-link",
        html: "T<span>able </span>O<span>f </span>C<span>ontents</span> \u25bc"
    }).appendTo(toc_container);

    $("#toc").appendTo(toc_container);

    toc_items = $("#toc ul").first().find("li");
    $("#toc_search").on("keyup", function (e) {
        var search_string = $("#toc_search").val();
        toc_search(search_string);
    });

    $(document).on("click", function (e) {
        if (!$(e.target).closest(toc_container).length) {
            $("#toc").hide();
        }
    });

    toc_link.on("click", function (e) {
        e.preventDefault();
        $("#toc").toggle();
        if ($("#toc").is(":visible")) {
            $("#toc_search").focus();
        }
    });
}

function fixTOC() {
    addInheritedMethods();

    if('localStorage' in window) {
        storage = window.localStorage;
    } else {
        storage = {};
    }

    $("#menubar").append($("<ul>", {id: "nav"}));

    create_menubar_item("", helpRoot + "/Help.html", function (a, li) {
        a.addClass("home");
        $('<span>', { 
            text: "SuperCollider"
        }).appendTo(a);
    });

    create_menubar_item("Browse", helpRoot + "/Browse.html");
    create_menubar_item("Search", helpRoot + "/Search.html");

    create_menubar_item("Indexes \u25bc", "#", function (a, li) {
        var indexes_menu = $("<div>", {class: "submenu"}).hide()
            .appendTo(li);

        var nav_items = ["Documents", "Classes", "Methods"];
        nav_items.forEach(function (item) {
            $("<a>", {
                text: item,
                href: helpRoot + "/Overviews/" + item + ".html"
            }).appendTo(indexes_menu);
        });

        a.on("click", function (e) {
            e.preventDefault();
            indexes_menu.toggle();
        });

        $(document).on("click", function (e) {
            if (!$(e.target).closest(li).length) {
                indexes_menu.hide();
            }
        });
    });
  
    buildThemeSwitcher();

    create_menubar_item("123", "#", function(a, li) {
        var originalText = a.text();
        var isStrikethrough = true;
        a.css("text-decoration", "line-through")
        .click(function() {
            yPosBeforeClick = window.scrollY;
            toggleCodeLineNumbers();
            if (isStrikethrough) {
                a.css("text-decoration", "none");
            } else {
                a.css("text-decoration", "line-through");
            }
            isStrikethrough = !isStrikethrough;
            setTimeout(function() {
                if (yPosBeforeClick != window.scrollY) {
                    window.scrollTo(0, yPosBeforeClick);
                }
            }, 0);
        });
    });

    if ($("#toc").length) {
        set_up_toc();
    }
}

// theming
const scDocsThemeStorageKey = "scDocsTheme";
const scriptLocation = document.currentScript.src;
const themeNames = [
    "classic",
    "dark",
    "default",
    "dracula",
    "solarizedDark",
    "solarizedLight"
];

function buildThemeSwitcher() {
    create_menubar_item("Theme \u25bc", "#", function (a, li) {
        const themesMenu = $("<div>", { class: "submenu" }).hide()
            .appendTo(li);


        themeNames.forEach(function (themeName) {
            var themeLink = $("<a>", {
                text: themeName,
                href: "#"
            });
            themeLink.on("click", (e) => {
                setTheme(e.target.text);
            });
            themeLink.appendTo(themesMenu);
        });

        a.on("click", function (e) {
            e.preventDefault();
            themesMenu.toggle();
        });

        $(document).on("click", function (e) {
            if (!$(e.target).closest(li).length) {
                themesMenu.hide();
            }
        });
    });
}

function setTheme(themeName) {
    localStorage.setItem(scDocsThemeStorageKey, themeName);
    applyTheme();
}

function applyTheme(theme=null) {
    const themeName = theme ?? localStorage.getItem(scDocsThemeStorageKey) ?? "default";

    const cssThemeTag = document.getElementById("scdoc-theme");
    if(cssThemeTag===null) {
        console.log(`Could not find scdoc-theme css tag! Can not apply theme ${themeName}`);
        return;
    }
    
    cssThemeTag.href = `${scriptLocation.split('/').slice(0, -1).join("/")}/themes/${themeName}.css`;
}

// scdoc.js is loaded after css themes so it's safe to call it here:
// run immediately, don't wait for document load, to prevent "unstyled flash"
applyTheme();

// Set up a QWebChannel for communicating with C++ IDE objects. The main app publishes a handle
// to IDE functionality at "IDE" which is made globally available here after the page and
// WebSocket have loaded.

// This function is called by the IDE, and will not be called otherwise.
function setUpWebChannel(port) {
    if (typeof QWebChannel === "undefined") {
        return;
    }
    var baseUrl = `ws://localhost:${port}`;
    var socket = new WebSocket(baseUrl);
    socket.onclose = function() { };
    socket.onerror = function(error) {
        console.error("WebChannel error: " + error);
    };
    socket.onopen = function() {
        new QWebChannel(socket, function(channel) {
            // make help browser globally accessible
            window.IDE = channel.objects.IDE;
        });
    }
}
