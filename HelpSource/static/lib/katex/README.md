# KaTeX

SCDocs uses [KaTeX](https://github.com/KaTeX/KaTeX) to render tex code into formulas.

In order to reduce the footprint of KaTex, a custom build of version `0.16.0` is used which only includes the `woff2` fontfiles using

```shell
USE_TTF="false" USE_WOFF="false" USE_WOFF2="true" yarn build
```

`woff2` is a sufficient font type, see <https://caniuse.com/woff2>.

## License

KaTeX is published under MIT license.
The bundled fonts are licensed under SIL Open Font License, see <https://github.com/KaTeX/KaTeX/issues/339#issuecomment-135518223>.
