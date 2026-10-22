Variables {
    var instance_var;

    // expect-reads: a -> a
    // expect: -> nil
    declare { var a; ^a }
    // expect-reads: a -> a = 1
    // expect: -> 1
    declare_with_value { var a = 1; ^a }
    // expect-reads: b -> b
    // expect: -> nil
    declare_several { var a = 1, b, c = 3; ^b }
    // expect-reads: a -> a = 1
    // expect-reads: b -> b = a + 1
    uses_earlier { var a = 1, b = a + 1; ^b }
    // expect-reads: a -> a(1 + 2)
    bracketed_value { var a(1 + 2); ^a }
    // expect-error: Variable used in its own initial value
    // expect: poisoned
    refers_to_itself { var a = a; ^a }
    // expect-error: Variable not defined
    used_before_declared { a; var a = 1 }
    // expect-reads: a -> a = 2
    // expect: -> 2
    declared_after_expression { 1; var a = 2; ^a }
    // expect-error: Variable already declared
    duplicate { var a, a; ^a }
    // expect-error: Variable already declared
    duplicate_argument { |a| var a; ^a }
    // expect-reads: instance_var -> instance_var = 1
    // expect: -> 1
    shadows_instance { var instance_var = 1; ^instance_var }
    assign { var a; a = 2 }
    // expect-reads: a -> a = 1
    in_block { ^{ var a = 1; a } }
    // expect-reads: a -> a = 2
    block_shadows_outer { var a = 1; ^{ var a = 2; a } }
    // expect-reads: a -> a = 1
    // expect: no result
    block_reads_outer { var a = 1; ^{ a } }
    // expect-reads: a -> a
    // expect: -> 1
    pack { var #a, b = [1, 2]; ^a }
}
