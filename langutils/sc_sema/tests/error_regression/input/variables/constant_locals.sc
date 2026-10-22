ConstantLocals {
    var instance_var = 1;

    // expect: -> 1
    constant { var a = 1; ^a }
    // expect: -> nil
    nil_by_default { var a; ^a }
    // expect: -> [1]
    propagates { var a = 1; ^[a] }
    // expect: -> 1
    through_another { var a = 1, b = a; ^b }
    // expect: no result
    not_constant_value { var a = instance_var; ^a }
    // expect: no result
    assigned { var a = 1; a = 2; ^a }
    // expect: no result
    assigned_later { var a = 1; ^a; a = 2 }
    // expect: no result
    assigned_in_block { var a = 1; { a = 2 }; ^a }
    // expect: no result
    read_by_block { var a = 1; ^{ a } }
    // expect: no result
    read_by_nested_block { var a = 1; ^{ { a } } }
    // expect: no result
    shadowed_by_argument { var a = 1; { |a| a }; ^a }
    // expect: no result
    shadowed_by_variable { var a = 1; { var a = 2; a }; ^a }
    // expect: no result
    used_before_inner_declaration { var a = 1; { a; var a = 2 }; ^a }
    // expect: no result
    inner_default_reads_outer { var a = 1; { var b = a; b }; ^a }
    constant_in_block { ^{ var a = 1; a } }
    // expect: no result
    member_not_constant { ^instance_var }
    // expect: no result
    argument_not_constant { |x = 1| ^x }
}
