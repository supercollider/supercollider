MathJax.Hub.Config({
  tex2jax: {
    skipTags: ["script","noscript","style","textarea","pre","code"],
    ignoreClass: ".*",
    processClass: "math",
    processEnvironments: false,
  },
  MathMenu: {
    showRenderer: false,
    showFontMenu: false,
    showContext:  false,
  },
  "HTML-CSS": {
    scale: 110,
    availableFonts: []
  }
});

MathJax.Ajax.loadComplete("[MathJax]/config/scmathjax.js");

