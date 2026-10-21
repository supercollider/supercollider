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

MethodArguments {
    var instance_var;

    read_argument { |a| ^a }
    assign_argument { |a| a = 1 }
    earlier_argument { |a, b = (a + 1)| ^b }
    later_argument { |a = (b), b| ^a }
    shadows_instance { |instance_var| ^instance_var }
    variadic { |a ... rest| ^rest }
    block_uses_argument { |a| ^{ a } }

    *class_read_argument { |a, b = (a)| ^b }
}
