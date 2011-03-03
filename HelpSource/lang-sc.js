/*
SC.app has: class, text, stringColor, commentColor, symbolColor, numberColor

        'PR_COMMENT': PR_COMMENT,
        'PR_DECLARATION': PR_DECLARATION,
        'PR_KEYWORD': PR_KEYWORD,
        'PR_LITERAL': PR_LITERAL,
        'PR_NOCODE': PR_NOCODE,
        'PR_PLAIN': PR_PLAIN,
        'PR_PUNCTUATION': PR_PUNCTUATION,
        'PR_SOURCE': PR_SOURCE,
        'PR_STRING': PR_STRING,
        'PR_TAG': PR_TAG,
        'PR_TYPE': PR_TYPE

*/
PR.registerLangHandler(
    PR.createSimpleLexer(
        [
         [PR.PR_PLAIN,       /^[\t\n\r \xA0]+/, null, '\t\n\r \xA0'],
         [PR.PR_STRING,     /^"(?:[^\\"]|\\.)*(?:"|$)/, null, '"'],
        ],
        [
        [PR.PR_LITERAL,      /^\$./],
         [PR.PR_LITERAL,     /^\\\w+/],
         
        [PR.PR_TAG, /^(?:super|thisFunctionDef|thisFunction|thisMethod|thisProcess|thisThread|this)\b/],
      
         [PR.PR_KEYWORD,     /^(?:true|false|nil|inf)\b/],
         [PR.PR_DECLARATION, /^(?:var|classvar|const|arg)\b/],
          [PR.PR_DECLARATION, /^\|/],
                  
         [PR.PR_LITERAL,     /^(?:(?:0(?:[0-7]+|X[0-9A-F]+))L?|(?:(?:0|[1-9][0-9]*)(?:(?:\.[0-9]+)?(?:E[+\-]?[0-9]+)?F?|L?))|\\.[0-9]+(?:E[+\-]?[0-9]+)?F?)/i],
/*         [PR.PR_TYPE,        /^\b([A-Z][A-Za-z_0-9]*){2,}\b/, null], //not working as it should.. does not match Bag and Pan2*/
         [PR.PR_TYPE,        /^\b([A-Z][A-Za-z_0-9]+)\b/],
         [PR.PR_COMMENT,     /^\/(?:\/.*|\*(?:\/|\**[^*/])*(?:\*+\/?)?)/],
         [PR.PR_PUNCTUATION, /^[-.,;!#$/+*<>=@()\[\]{}]/]
        ]),
    ['sc']);
