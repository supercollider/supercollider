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

PackAssign {
    var instance_var;
    classvar class_var;

    assign_locals { var a, b; #a, b = [1, 2]; ^a }
    assign_rest { var a, b, c; #a, b ...c = [1, 2, 3]; ^c }
    assign_members { #instance_var, class_var = [1, 2] }
    assign_argument { |x| #x = [1]; ^x }
    assign_environment { var a; #~e, a ...~rest = [1, 2, 3] }
    assign_parenthesised { var a, b; #a, b ([1, 2]); ^b }
    assign_result { var a, b; ^#a, b = [1, 2] }
    assign_undefined { var a; #a, undefined = [1, 2] }
    assign_makes_not_constant { var a = 1, b; #a, b = [2, 3]; ^a }
    assign_in_block { var a = 1, b; { #a, b = [2, 3] }; ^a }

    declare { var #a, b = [1, 2]; ^a }
    declare_past_end { var #a, b, c = [1, 2]; ^c }
    declare_rest { var #a ...b = [1, 2, 3]; ^b }
    declare_rest_first_constant { var #a ...b = [1, 2, 3]; ^a }
    declare_not_constant { var #a, b = instance_var; ^a }
    declare_mutable_element { var #a, b = [[1], 2]; ^[a, b] }
    declare_assigned_later { var #a, b = [1, 2]; a = 3; ^a }
    declare_environment { var #a, ~b = [1, 2]; ^a }
    declare_duplicate { var #a, a = [1, 2]; ^a }
    declare_refers_to_itself { var #a, b = [a]; ^b }
    declare_uses_earlier { var x = 1, #a, b = [x, 2]; ^a }
    declare_in_function { ^{ var #a, b = [1, 2]; b } }
    declare_in_function_with_rest { ^{ var #a ...b = [1, 2]; a } }
    declare_hides_outer { var a = 1; { var #a = [2]; a }; ^a }
    declare_captured { var #a, b = [1, 2]; ^{ a } }
    declare_with_non_local_return { var #a, b = [1, 2]; { ^a }.value; ^b }
    declare_with_nested_non_local_return { ^{ var #a, b = [1, 2]; { ^b }.value; a } }
}
