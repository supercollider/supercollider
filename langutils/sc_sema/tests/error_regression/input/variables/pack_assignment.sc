PackAssign {
    var instance_var;
    classvar class_var;

    // expect-reads: a -> a
    // expect: no result
    assign_locals { var a, b; #a, b = [1, 2]; ^a }
    // expect-reads: c -> c
    assign_rest { var a, b, c; #a, b ...c = [1, 2, 3]; ^c }
    assign_members { #instance_var, class_var = [1, 2] }
    // expect-reads: x -> x
    assign_argument { |x| #x = [1]; ^x }
    assign_environment { var a; #~e, a ...~rest = [1, 2, 3] }
    // expect-reads: b -> b
    assign_parenthesised { var a, b; #a, b ([1, 2]); ^b }
    assign_result { var a, b; ^#a, b = [1, 2] }
    // expect-error: Variable not defined
    assign_undefined { var a; #a, undefined = [1, 2] }
    // expect-reads: a -> a = 1
    // expect: no result
    assign_makes_not_constant { var a = 1, b; #a, b = [2, 3]; ^a }
    // expect-reads: a -> a = 1
    // expect: no result
    assign_in_block { var a = 1, b; { #a, b = [2, 3] }; ^a }

    // expect-reads: a -> a
    // expect: -> 1
    declare { var #a, b = [1, 2]; ^a }
    // expect-reads: c -> c
    // expect: -> nil
    declare_past_end { var #a, b, c = [1, 2]; ^c }
    // expect-reads: b -> b
    // expect: no result
    declare_rest { var #a ...b = [1, 2, 3]; ^b }
    declare_rest_first_constant { var #a ...b = [1, 2, 3]; ^a }
    // expect-reads: a -> a
    // expect: no result
    declare_not_constant { var #a, b = instance_var; ^a }
    // expect: no result
    declare_mutable_element { var #a, b = [[1], 2]; ^[a, b] }
    // expect: no result
    declare_assigned_later { var #a, b = [1, 2]; a = 3; ^a }
    // expect-error: Environment variable declared
    declare_environment { var #a, ~b = [1, 2]; ^a }
    // expect-error: Variable already declared
    declare_duplicate { var #a, a = [1, 2]; ^a }
    // expect-error: Variable not defined
    declare_refers_to_itself { var #a, b = [a]; ^b }
    // expect-reads: x -> x = 1
    // expect-reads: a -> a
    // expect: -> 1
    declare_uses_earlier { var x = 1, #a, b = [x, 2]; ^a }
    // expect-reads: b -> b
    declare_in_function { ^{ var #a, b = [1, 2]; b } }
    declare_in_function_with_rest { ^{ var #a ...b = [1, 2]; a } }
    // expect-reads: a -> a; a = 1
    declare_hides_outer { var a = 1; { var #a = [2]; a }; ^a }
    // expect-reads: a -> a
    // expect: no result
    declare_captured { var #a, b = [1, 2]; ^{ a } }
    // expect-reads: a -> a
    // expect-reads: b -> b
    // expect: no result
    declare_with_non_local_return { var #a, b = [1, 2]; { ^a }.value; ^b }
    // expect-reads: b -> b
    // expect-reads: a -> a
    declare_with_nested_non_local_return { ^{ var #a, b = [1, 2]; { ^b }.value; a } }
}
