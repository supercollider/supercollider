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

WhitespaceChars {
    char_space { ^$  }
    char_tab { ^$	 }
    char_newline { ^$
 }
    char_carriage_return { ^$ }
    char_carriage_return_newline { ^$
 }
}
