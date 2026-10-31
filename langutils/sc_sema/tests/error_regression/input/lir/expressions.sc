// lir
//# VERSION 3.16
// Straight-line code lowered to the backend's lir: reads and stores by frame depth and slot, members by where they
// are stored, sends with their argument counts, and the sends the old grammar made, e.g., `~x` and `(a..b)`.
LirExpressions {
    var inst;
    classvar shared;
    const k = 7;

    // expect: no diagnostics
    locals { |a, b| var x = a; x = b; ^x }
    // expect: no diagnostics
    members { inst = 1; shared = inst; ^k }
    // expect: no diagnostics
    sends { |a| ^a.foo(1, key: 2) }
    // expect: no diagnostics
    supers { ^super.foo(1) }
    // expect: no diagnostics
    adverb { |a| ^a +.x 1 }
    // expect: no diagnostics
    variadic { |a, b| ^a.foo(1, *b) }
    // expect: no diagnostics
    super_variadic { |b| ^super.foo(*b) }
    // expect: no diagnostics
    environment { ~x = 3; ^~x }
    // expect: no diagnostics
    range { |n| ^(1..n) }
    // expect: no diagnostics
    specials { ^[this, thisProcess, thisFunction] }
    // expect: no diagnostics
    expression_list { |a| ^(a.foo; a.bar) }
}
