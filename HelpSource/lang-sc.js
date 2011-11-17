PR.registerLangHandler(
    PR.createSimpleLexer(
    [
        /* whitespace */
        [PR.PR_PLAIN,       /^[\t\n\r \xA0]+/, null, '\t\n\r \xA0'],
        /* strings */
        [PR.PR_STRING,      /^"(?:[^\\"]|\\.)*(?:"|$)/, null, '"'],
    ],
    [
        /* char literal */
        [PR.PR_LITERAL,     /^\$(\\)?./],
        /* symbols */
        [PR.PR_ATTRIB_NAME, /^\\\w+/],
        [PR.PR_ATTRIB_NAME, /^'[^']+'/],
        [PR.PR_ATTRIB_VALUE, /^~\w+/],
        /* special variables */
        [PR.PR_TAG,         /^(?:super|thisFunctionDef|thisFunction|thisMethod|thisProcess|thisThread|this)\b/],
        /* special values */
        [PR.PR_KEYWORD,     /^(?:true|false|nil|inf)\b/],
        /* variable declarations */
        [PR.PR_DECLARATION, /^(?:var|classvar|const|arg)\b/],
//        [PR.PR_DECLARATION, /^\|/],
        /* class names */
        [PR.PR_TYPE,        /^\b([A-Z][A-Za-z_0-9]+)\b/],
        [PR.PR_COMMENT,     /^\/(?:\/.*|\*(?:\/|\**[^*/])*(?:\*+\/?)?)/], //fixme: nested comments
        /* numbers */
        [PR.PR_LITERAL,     /^-?\d+r[\da-zA-Z]+(\.[\da-zA-Z]+)?/],
//        [PR.PR_LITERAL,     /^-?(?:(?:\d+(?:\.\d*)?)(?:e[+\-]?\d+)?)(pi)?|pi/],
        [PR.PR_LITERAL,     /^-?(?:(?:\d+(\.\d+)?)(?:e[+\-]?\d+)?(pi)?)|(?:pi\b)/],
        /* other stuff */
        [PR.PR_PLAIN, /^[a-z_]\w*/i],
//        [PR.PR_PUNCTUATION, /^[-.,;!?#$%&\|/+*<>=@()\[\]{}]/]
        [PR.PR_PUNCTUATION, /^[-.,;#()\[\]{}]/]
    ]),
    ['sc']);

