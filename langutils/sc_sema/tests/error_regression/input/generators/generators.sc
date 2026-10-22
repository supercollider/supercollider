Generators {
    var instance_var;

    // expect-reads: x -> x <- [1, 2, 3]
    // expect: no result
    routine { ^{: x, x <- [1, 2, 3] } }
    // expect-reads: x -> x <- [1, 2, 3]
    immediate { ^{; x.postln, x <- [1, 2, 3] } }
    // expect-reads: x -> x i <- [1, 2, 3]
    // expect-reads: i -> x i <- [1, 2, 3]
    with_index { ^{: [x, i], x i <- [1, 2, 3] } }
    // expect-error: Variable already declared
    // expect-reads: x -> x x <- [1, 2, 3]
    duplicate_index { ^{: x, x x <- [1, 2, 3] } }
    // expect-reads: x -> x <- [1, 2]
    // expect-reads: y -> y <- [x, 3]
    nested_iterate { ^{: [x, y], x <- [1, 2], y <- [x, 3] } }
    // expect-reads: y -> var y = x * 2
    // expect-reads: x -> x <- [1, 2]
    declare { ^{: y, x <- [1, 2], var y = x * 2 } }
    // expect-reads: y -> var y (2)
    declare_bracketed { ^{: y, var y (2) } }
    // expect-reads: x -> x <- (1..10)
    condition { ^{: x, x <- (1..10), x.odd } }
    // expect-reads: x -> x <- [1, 2]
    side_effect { ^{: x, x <- [1, 2], :: x.postln } }
    // expect-reads: x -> x <- (1..)
    // expect: no diagnostics
    while_guard { ^{: x, x <- (1..), :while x < 5 } }
    // expect-reads: x -> x <- (1..)
    // expect-reads: y -> var y = x + 1
    all_qualifiers { ^{: [x, y], x <- (1..), x.even, var y = x + 1, :: y.postln, :while y < 20 } }
    // expect-reads: x -> x <- [x, 3]; x <- [1, 2]
    shadows_earlier { ^{: x, x <- [1, 2], x <- [x, 3] } }
    // expect-error: Variable not defined
    // expect: poisoned
    source_cannot_see_element { ^{: x, x <- x } }
    // expect: no diagnostics
    unbounded_source { ^{: x, x <- (1..) } }
    // expect: no diagnostics
    unbounded_source_step { ^{: x, x <- (1, 3..) } }
    // expect-error: Range has no end
    unbounded_range_outside { var x = (1..); ^x }
    // expect-reads: x -> x <- [1, 2]
    // expect-reads: instance_var -> instance_var
    reads_member { ^{: x + instance_var, x <- [1, 2] } }
    // expect-reads: a -> a = 1
    // expect: no result
    captures_outer { var a = 1; ^{: a, x <- [1, 2] } }
    // expect-reads: x -> x <- [2]; x = 1
    shadows_outer { var x = 1; { {: x, x <- [2] } }; ^x }
    // expect-reads: x -> x <- [x]; x = 1; x = 1
    // expect: no result
    source_reads_outer { var x = 1; { {: x, x <- [x] } }; ^x }
    // expect-reads: x -> x <- [1, 2]
    non_local_return_value { ^{: { ^x }.value, x <- [1, 2] } }
    // expect-reads: x -> x <- [1, 2]
    // expect: no result
    non_local_return_qualifier { {; x, x <- [1, 2], :: { ^x }.value }; ^1 }

    // expect: no result
    range_generator { ^(: 1..5) }
    range_generator_step { ^(: 1, 3..9) }
    range_generator_from_zero { ^(: ..3) }
    // expect: no diagnostics
    // expect: no result
    range_generator_unbounded { ^(: 1..) }
    range_generator_unbounded_step { ^(: 1, 3..) }
    // expect-reads: instance_var -> instance_var
    range_generator_not_constant { ^(: 1..instance_var) }
}
