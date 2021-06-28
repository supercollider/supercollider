# CSS themes

In order to have a consistent style between the Qt SC IDE themes and
the HTML docs [PR #5487](https://github.com/supercollider/supercollider/pull/5487)
introduced a switch from `.css` to `.scss`.

SCSS is a superset of CSS and introduces variables which come
in handy when working with multiple color styles.

As the `.scss` files needs to be compiled to `.css` in order to work in
a HTML file you need to have a
[SASS compiler](https://sass-lang.com/install).

The important thing to keep in mind: do NOT modify the CSS files directly
anymore as they are automatically generated/compiled out of the SCSS files.

The compiled CSS files are also included in the repository so
an SASS compiler does not become a build dependency - so it is your
responsibility to update all CSS themed files when modifying a style sheet.

To build all necessary CSS files simply run `./build_css.sh`.
