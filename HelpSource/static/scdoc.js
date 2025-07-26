
var storage;
var menubar;

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

var toc_items;
var firstMatch = null; // Variable to store the first match

function toc_search(search_string) {
    var re = RegExp("^"+escape_regexp(search_string),"i");
    firstMatch = null; // Reset first match on each search

    for(var i=0;i<toc_items.length;i++) {
        var li = toc_items[i];
        var a = li.firstChild;
        if(re.test(a.innerHTML)) {
            li.style.display = "";
            if(firstMatch === null) firstMatch = a; // Set first match

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

    $("#toc_search").on('keydown', function(event) {
        if (event.key === 'Escape') {
            $("#toc").toggle();
        };

        if (event.key === 'Enter' && firstMatch !== null) {
            firstMatch.click(); // Simulate a click on the first matched item
            $("#toc").toggle();
        }

        //TOC scroll with ctrl+{j,k}
        if (event.ctrlKey && event.key === "j") {
            $("#toc").scrollTop($("#toc").scrollTop() + 10);
        }
    
        if (event.ctrlKey && event.key === "k") {
            event.preventDefault();
            $("#toc").scrollTop($("#toc").scrollTop() - 10);
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

        var nav_items = ["Documents", "Classes", "ClassTree", "Methods"];
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
    "monokai",
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

        themesMenu.append($("<hr>"));

        buildLineNumberSwitch(themesMenu);

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

function buildLineNumberSwitch(themesMenu) {
    const lineNumberCheckbox = $("<input>", {
      type: "checkbox",
      id: "line-number-checkbox",
      checked: getLineNumberStorageValue(),
    }).on("click", () => {
      setLineNumberStorageValue(!getLineNumberStorageValue());
    });
    const lineNumberLabel = $("<label>", {
      text: "Line numbers",
      for: "line-number-checkbox",
    });
    const lineNumberSwitch = $("<div>").append(lineNumberCheckbox).append(lineNumberLabel);
    themesMenu.append(lineNumberSwitch);
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

function renderTex() {
    for (let aSpan of document.getElementsByClassName("math")) {
        const tex = aSpan.textContent;
        katex.render(tex, aSpan, {
            throwOnError: false,
            displayMode: aSpan.classList.contains("block"),
        });
    }
}

function copyButtonInCodeArea() {
    document.querySelectorAll('.codeMirrorContainer').forEach(container => {
        const button = container.querySelector('.copy-button');
        const editor = container.querySelector('.editor');

        button.addEventListener('click', () => {
            navigator.clipboard.writeText(editor.value).then(() => {
                button.classList.add('copied');

                setTimeout(() => {
                    button.classList.remove('copied');
                }, 1400);
            });
        });
    });
}


document.addEventListener("DOMContentLoaded", function () {
    copyButtonInCodeArea();
    renderTex();
});

function getLineNumberStorageValue() {
    return window.localStorage.getItem("showLineNumbers") === "true"
}

function setLineNumberStorageValue(v) {
    window.localStorage.setItem("showLineNumbers", v ? "true" : "false");
    toggleLineNumbers(v);
}

function toggleLineNumbers(v) {
    Array.from(document.querySelectorAll("textarea")).filter((t) => t.hasOwnProperty("editor")).forEach((t) => {
      t.editor.setOption("lineNumbers", v);
    });
}
