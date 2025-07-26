# CSS Themes

In order to adapt the theme of the IDE also for the documentation the CSS of the documentation was refactored to use CSS variables.
The values of these CSS variables are stored in the CSS files of this directory and are injected with JavaScript.

The active theme is set by setting the variable `scDocsTheme` within the [`localStorage`](https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage) of the website, which reflects the filename of the `css` files. The implementation can be found in `scdoc.js`.

## Development

When modifying the layout of the documentation it is important to keep in mind that the styles have to work via the `file://` access of the IDE Browser (see [Qt WebEngine](https://doc.qt.io/qt-5/qtwebengine-overview.html) for more specific details) as well as access via the internet in a web server environment (`https://`).
Although the *Qt WebEngine* browser [is based on Chromium](https://wiki.qt.io/QtWebEngine#Relationship_to_Chromium), it does not support all latest CSS and JS tricks, so it is advised to code conservatively here in order to not force updates of Qt.

A way to tweak the CSS design is to compile SuperCollider and compile the `.schelp` files to static `.html` files via

```supercollider
// render all docs
SCDoc.renderAll;

// open folder with compiled HTML docs
SCDoc.helpTargetDir.openOS;
```

Use this directory to try out any tweaks which then have to be back-ported to the `/HelpSource` directory of the repository.
Rinse and repeat til done.
