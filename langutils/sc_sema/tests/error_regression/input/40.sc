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

ConstantLocals {
    var instance_var = 1;

    constant { var a = 1; ^a }
    nil_by_default { var a; ^a }
    propagates { var a = 1; ^[a] }
    through_another { var a = 1, b = a; ^b }
    not_constant_value { var a = instance_var; ^a }
    assigned { var a = 1; a = 2; ^a }
    assigned_later { var a = 1; ^a; a = 2 }
    assigned_in_block { var a = 1; { a = 2 }; ^a }
    read_by_block { var a = 1; ^{ a } }
    read_by_nested_block { var a = 1; ^{ { a } } }
    shadowed_by_argument { var a = 1; { |a| a }; ^a }
    shadowed_by_variable { var a = 1; { var a = 2; a }; ^a }
    used_before_inner_declaration { var a = 1; { a; var a = 2 }; ^a }
    inner_default_reads_outer { var a = 1; { var b = a; b }; ^a }
    constant_in_block { ^{ var a = 1; a } }
    member_not_constant { ^instance_var }
    argument_not_constant { |x = 1| ^x }
}
