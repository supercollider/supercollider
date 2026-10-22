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

Generators {
    var instance_var;

    routine { ^{: x, x <- [1, 2, 3] } }
    immediate { ^{; x.postln, x <- [1, 2, 3] } }
    with_index { ^{: [x, i], x i <- [1, 2, 3] } }
    duplicate_index { ^{: x, x x <- [1, 2, 3] } }
    nested_iterate { ^{: [x, y], x <- [1, 2], y <- [x, 3] } }
    declare { ^{: y, x <- [1, 2], var y = x * 2 } }
    declare_bracketed { ^{: y, var y (2) } }
    condition { ^{: x, x <- (1..10), x.odd } }
    side_effect { ^{: x, x <- [1, 2], :: x.postln } }
    while_guard { ^{: x, x <- (1..), :while x < 5 } }
    all_qualifiers { ^{: [x, y], x <- (1..), x.even, var y = x + 1, :: y.postln, :while y < 20 } }
    shadows_earlier { ^{: x, x <- [1, 2], x <- [x, 3] } }
    source_cannot_see_element { ^{: x, x <- x } }
    unbounded_source { ^{: x, x <- (1..) } }
    unbounded_source_step { ^{: x, x <- (1, 3..) } }
    unbounded_range_outside { var x = (1..); ^x }
    reads_member { ^{: x + instance_var, x <- [1, 2] } }
    captures_outer { var a = 1; ^{: a, x <- [1, 2] } }
    shadows_outer { var x = 1; { {: x, x <- [2] } }; ^x }
    source_reads_outer { var x = 1; { {: x, x <- [x] } }; ^x }
    non_local_return_value { ^{: { ^x }.value, x <- [1, 2] } }
    non_local_return_qualifier { {; x, x <- [1, 2], :: { ^x }.value }; ^1 }

    range_generator { ^(: 1..5) }
    range_generator_step { ^(: 1, 3..9) }
    range_generator_from_zero { ^(: ..3) }
    range_generator_unbounded { ^(: 1..) }
    range_generator_unbounded_step { ^(: 1, 3..) }
    range_generator_not_constant { ^(: 1..instance_var) }
}
