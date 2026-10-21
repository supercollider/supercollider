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

IdentifierKeys {
    var instance_var;
    classvar class_var;

    identifier_key { ^(instance_var : 1) }
    keyword_key { ^(instance_var: 1) }
    identifier_key_undefined { ^(undefined_var : 1) }
    *class_identifier_key { ^(class_var : 1) }
}

Initialisers {
    const const_key = \k;
    const constant_event = (a: 1, b: [2, 3]);
    classvar class_event = (a: 1);
    var instance_event = (a: (b: 2));
    const constant_array = [1, (a: 2)];
    classvar class_array = [1, 2];
    var instance_array = #[x, y];
    var identifier_key_const = (const_key : 1);
    var identifier_key_not_const = (other : 1);
    var value_not_constant = (a: 1.foo);
}
