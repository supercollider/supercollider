//# VERSION 3.16
EscapedLiterals {
    char_newline { ^$\n }
    char_tab { ^$\t }
    char_quote { ^$" }
    char_backslash { ^$\\ }
    string_escapes { ^"tab\there \"quoted\" back\\slash" }
    string_newline { ^"line one\nline two" }
    symbol_quote_escaped { ^'it\'s' }
}
