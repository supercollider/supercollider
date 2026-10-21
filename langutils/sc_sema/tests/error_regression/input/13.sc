AbstractObjectExperimental {}

Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}

Class {}

String {}

True {}

False {}

Nil {}

Float {}

Integer {}

Char {}

Symbol {}

Ref {}

Array {}

Event {}

EscapedLiterals {
    char_newline { ^$\n }
    char_tab { ^$\t }
    char_quote { ^$" }
    char_backslash { ^$\\ }
    string_escapes { ^"tab\there \"quoted\" back\\slash" }
    string_newline { ^"line one\nline two" }
    symbol_quote_escaped { ^'it\'s' }
}
