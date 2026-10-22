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

SliceFolding {
    var x;
    var initial_array = #[1, 2, 3][1..];
    var initial_array_2 = [1, 2, 3][1..];
    var initial_array_3 = [1, 2, 3][-10..];
    var initial_string = "hello"[..1];
    var initial_not_constant = 1.foo[1..2];

    const const_array = [10, 20, 30, 40];
    var const_array_slice = const_array[2..];
    var const_array_slice_neg = const_array[-10..];

    array { ^[1, 2, 3, 4][1..2] }
    from_start { ^[1, 2, 3][..1] }
    to_end { ^[1, 2, 3][1..] }
    step { ^[0, 1, 2, 3, 4, 5][0, 2..5] }
    descending { ^[1, 2, 3][2..0] }
    clipped_end { ^[1, 2, 3][1..10] }
    clipped_start { ^[1, 2, 3][-5..1] }
    empty { ^[][..3] }
    string { ^"hello"[1..3] }
    string_descending { ^"hello"[3..1] }
    immutable { ^#[a, b, c][1..] }
    step_with_start_outside { ^[1, 2, 3, 4][-1, 1..3] }
    zero_step { ^[1, 2, 3][1, 1..2] }
    step_away { ^[1, 2, 3][2, 3..0] }
    receiver_not_constant { ^x[1..2] }
    bound_not_constant { ^[1, 2, 3][1..2.foo] }
}
