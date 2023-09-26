const scDocsTheme = "scDocsTheme";

function getTheme() {
    return localStorage.getItem(scDocsTheme) ?? "default";
}

function setTheme(themeName) {
    console.log("Set theme to", e.target.text);
    localStorage.setItem(scDocsTheme, themeName);
}

function loadTheme(theme=null) {
    const themeName = theme ?? getTheme();
    const scriptLocation = document.currentScript.src;

    var link = document.createElement("link");
    link.href = `${scriptLocation.split('/').slice(0, -1).join("/")}/themes/${themeName}.css`;
    link.type = "text/css";
    link.rel = "stylesheet";
    document.getElementsByTagName("head")[0].appendChild(link);

    // console.log(`Loaded theme ${themeName}`);
}

loadTheme();
