//# VERSION 3.16
// lir
// Calls with two variadic arguments aren't lowered yet, so their method isn't emitted.
LirUnimplemented {
    // expect: no diagnostics
    two_variadic { |a, b| ^this.foo(*a, *b) }
}
