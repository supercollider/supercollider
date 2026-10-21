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

AssignmentAt {
    var instance_var;

    single { instance_var[0] = 1 }
    empty { instance_var[] = 1 }
    dot { instance_var.[0] = 1 }
    dot_empty { instance_var.[] = 1 }
    multiple { instance_var[0, 1] = 2 }
    keyword { instance_var[0, b: 1] = 2 }
    variadic { instance_var[*0] = 1 }
    expression_receiver { 1.foo[0] = 2 }
    nested_value { instance_var[0] = instance_var[1] = 2 }
    as_argument { ^3.bar(instance_var[0] = 1) }
    undefined_value { instance_var[0] = undefined_var }
}
