Variables {
    var instance_var;

    declare { var a; ^a }
    declare_with_value { var a = 1; ^a }
    declare_several { var a = 1, b, c = 3; ^b }
    uses_earlier { var a = 1, b = a + 1; ^b }
    bracketed_value { var a(1 + 2); ^a }
    refers_to_itself { var a = a; ^a }
    used_before_declared { a; var a = 1 }
    declared_after_expression { 1; var a = 2; ^a }
    duplicate { var a, a; ^a }
    duplicate_argument { |a| var a; ^a }
    shadows_instance { var instance_var = 1; ^instance_var }
    assign { var a; a = 2 }
    in_block { ^{ var a = 1; a } }
    block_shadows_outer { var a = 1; ^{ var a = 2; a } }
    block_reads_outer { var a = 1; ^{ a } }
    pack { var #a, b = [1, 2]; ^a }
}
