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

ArgumentDefaults {
    var instance_var;

    block_constant { ^{ |a = 1| a } }
    block_constant_array { ^{ |a = #[1, 2], b = (c: 3)| a } }
    block_bracketed { ^{ |a = (1.foo)| a } }
    block_earlier_argument { ^{ |a, b = (a + 1)| b } }
    block_later_argument { ^{ |a = (b), b| a } }
    block_itself { ^{ |a = (a)| a } }

    method_constant { |a = 1| ^1 }
    method_instance_var { |a = (instance_var)| ^1 }
    method_other_argument { |a, b = (a)| ^1 }
}
