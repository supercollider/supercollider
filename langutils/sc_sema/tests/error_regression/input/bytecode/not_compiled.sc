// bytecode
//# VERSION 3.16
// Calls with two variadic arguments aren't lowered yet, so their method can't be emitted and is reported.
NotCompiled {
    // expect-error: Not compiled
    two_variadic { |a, b| ^this.foo(*a, *b) }
    // expect: no diagnostics
    fine { ^1 }
}
