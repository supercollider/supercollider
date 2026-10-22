// The classes the compiler requires, shared by every test that doesn't start with '// no-prelude'.
// They are compiled, but not included in the output.
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
