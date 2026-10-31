// compiled
// A method with a primitive names it. A shortcut method has no code.
Methods {
    var x;
    // expect: no diagnostics
    prim { _BasicNew ^this.primitiveFailed }
    // expect: no diagnostics
    primArgs { |a, b = 1| _ObjectPerform ^this.primitiveFailed }
    // expect: no diagnostics
    returnSelf { ^this }
    // expect: no diagnostics
    returnLiteral { ^1 }
    // expect: no diagnostics
    returnArgument { |a| ^a }
    // expect: no diagnostics
    returnInstance { ^x }
    // expect: no diagnostics
    code { |a| ^a + x }
}

+ Methods {
    // expect: no diagnostics
    extension { ^x + 1 }
}
