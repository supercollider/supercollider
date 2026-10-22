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

Slicing {
    var x;

    slice { ^x[1..3] }
    slice_step { ^x[1, 3..9] }
    slice_from_start { ^x[..3] }
    slice_to_end { ^x[1..] }
    slice_step_to_end { ^x[1, 3..] }
    dot_slice { ^x.[1..3] }
    slice_expression_bounds { ^x[1.foo..2.bar] }
    slice_keyword { ^x[a: 1..3] }
    slice_variadic { ^x[*1..3] }

    assign { x[1..3] = 5 }
    assign_step { x[1, 3..9] = 5 }
    assign_from_start { x[..3] = 5 }
    assign_to_end { x[1..] = 5 }
    dot_assign { x.[1..3] = 5 }
    assign_too_many { x[1, 2, 3..5] = 5 }
}
