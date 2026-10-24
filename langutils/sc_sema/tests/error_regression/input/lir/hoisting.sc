// lir
//# VERSION 3.16
// A hoisted variable is set each time the inlined body runs, nil without an initial value, so a loop's starts again on
// each iteration. A literal initial value isn't in the frame's prototype.
LirHoisting {
    // expect: no diagnostics
    ifVar { |a| ^if (a) { var c = a.foo; c } }
    // expect: no diagnostics
    literal { |a| ^if (a) { var c = 1; c } }
    // expect: no diagnostics
    loop { |a| while { var n = a.next; n.notNil } { var c; c = c ? 0 + 1 } }
    // expect: no diagnostics
    pack { |a| ^if (a) { var #c, d = a; c + d } }
    // expect: no diagnostics
    endsWithVar { |a| ^if (a) { a.foo; var c = 2 } }
    // expect: no diagnostics
    switch { |a| ^switch (a, 1, { var c = a.foo; c }, { var d; d }) }
    // expect: no diagnostics
    nonLocalReturn { |a| if (a) { var c = a.foo; if (c) { ^1 }; ^c }; ^2 }
    // expect: no diagnostics
    captured { |a| ^if (a) { var c = 1; { c } } }
}
