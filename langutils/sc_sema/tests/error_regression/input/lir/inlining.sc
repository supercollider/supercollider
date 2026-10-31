// lir
//# VERSION 3.16
// An inlined send's bodies are sub-regions of the function it is in, the tested value on the stack before it.
LirInlining {
    // expect: no diagnostics
    ifElse { |a| ^if (a) { a.foo } { ^2 } }
    // expect: no diagnostics
    isNil { |a| ^if (a.isNil) { 1 } }
    // expect: no diagnostics
    andOr { |a| ^(a and: { a.foo }) or: { a.bar } ?? { 1 } !? { 2 } }
    // expect: no diagnostics
    loops { |a| while { a.next } { a.foo }; loop { ^a } }
    // expect: no diagnostics
    case { |a| ^case { a == 1 } { 1 } { a.notNil } { 2 } { 3 } }
    // expect: no diagnostics
    switch { |a| ^switch (a, \x, { 1 }, { 2 }) }
    // expect: no diagnostics
    questionMark { |a| ^a ? { 1 } }
}
