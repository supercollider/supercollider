// no-prelude
//# VERSION 3.16
// Without the Interpreter, a closed function can't be compiled, an open one can. The prelude, but no Interpreter.
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

NoInterpreter {
    // expect-error: Interpreter missing
    // expect: poisoned
    closed { ^#{ 1 } }
    // expect-error: Interpreter missing
    // expect: poisoned
    nested { ^{ #{ 1 } } }
    // expect: not poisoned
    open { ^{ 1 } }
}
