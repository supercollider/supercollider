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

Setters {
    var x;

    simple { x.foo = 10 }
    expression_receiver { 1.bar.foo = 10 }
    literal_receiver { 1.foo = 2 }
    chained_value { x.foo = x.bar = 2 }
    as_argument { ^3.baz(x.foo = 1) }
    functional { foo(x, 1) = 2 }
    functional_receiver_only { foo(x) = 2 }
    functional_keyword { foo(x, b: 1) = 2 }
    functional_no_receiver { foo(b: 1) = 2 }
    functional_variadic { foo(x, *x) = 2 }
    functional_variadic_receiver { foo(*x) = 2 }
    functional_variadic_after_arguments { foo(x, 1, *x) = 2 }
}
