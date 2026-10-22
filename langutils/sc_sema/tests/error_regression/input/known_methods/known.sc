// no-prelude
AbstractObjectExperimental {}

Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
    perform { ^nil }
}

Class {}

ArrayedCollection {
    copySeries { ^nil }
    add { ^nil }
}

String : ArrayedCollection {}

True {}

False {}

Nil {}

Float {}

SimpleNumber {
    series { ^nil }
}

Integer : SimpleNumber {}

Char {}

Symbol {}

Ref {}

Array : ArrayedCollection {}

Dictionary {
    add { ^nil }
}

Event : Dictionary {}

KnownMethods {
    range { ^(1..3) }
    slice { ^[1, 2, 3][1..] }
    string_slice { ^"hello"[1..3] }
    array_collection { ^Array[1, 2] }
    event_collection { ^Event[a: 1] }
    perform_rewrite { ^1.perform(\foo) }
}
