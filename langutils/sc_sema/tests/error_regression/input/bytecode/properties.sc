// bytecode
//# VERSION 3.16
// The rules are legacy's, see versions/bytecode_properties. A captured variable is never folded, so a function using
// one always has a context.
BytecodeProperties316 {
    // expect: no diagnostics
    captured { var a = 1; ^{ a + 1 } }
    // A function in a body with hoisted variables is created by the method's frame.
    // expect: no diagnostics
    inHoisted { |b| ^if (b) { var a = b.foo; [a, { b }] } }
    // Not inlined, as `a` is captured.
    // expect: no diagnostics
    captureStopsInlining { |b| ^if (b) { var a = b; { a } } }
    // expect: no diagnostics
    nonLocalReturn { |b| ^{ if (b) { ^1 }; ^2 } }
}
