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

RangeInitialisers {
    const bound = 3;
    const constant_range = (1..3);
    classvar class_range = (1, 3..9);
    var instance_range = (..2);
    var uses_const = (1..bound);
    var nested = [(1..2), (a: (3..4))];
    var immutable = #[(1..2)];
    var float_range = (1.0..3);
    var unbounded = (1..);
    var not_constant = (1..2.foo);
    var too_large = (1..2000);
}
