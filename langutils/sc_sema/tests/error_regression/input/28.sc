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

Set {}

Collections {
    array_constant { ^Array[1, 2.5] }
    array_empty { ^Array[] }
    array_not_constant { ^Array[1, 2.foo] }
    array_nested { ^Array[[1], Array[2]] }
    event_constant { ^Event[a: 1, b: 2] }
    event_odd { ^Event[a: 1, \b] }
    event_not_constant { ^Event[a: 1.foo] }
    set_constant_elements { ^Set[1, 2] }
    set_empty { ^Set[] }
    nested_in_array { ^[Array[1], Set[2]] }
    undefined_class { ^NotAClass[1] }
}
