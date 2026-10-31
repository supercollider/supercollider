// frames
//# VERSION 3.16
// From 3.16 an inlined function's variables are slots of the frame it is inlined into, after that frame's own, unless
// one is captured, then the function isn't inlined.
FrameHoisting {
    // expect: no diagnostics
    ifVar { |a| var b; ^if (a) { var c = a.foo; c } { b } }
    // expect: no diagnostics
    siblings { |a| ^if (a) { var c = 1; c } { var c = 2; c } }
    // expect: no diagnostics
    nested { |a| ^{ while { a.next } { var c = a; if (c) { var d = c; d } } } }
    // expect: no diagnostics
    pack { |a| ^if (a) { var #c, d = a; c + d } }
    // expect: no diagnostics
    captured { |a| ^if (a) { var c = 1; { c } } }
    // Captured as sema sees it, before inlining, though the inner `if` is inlined.
    // expect: no diagnostics
    capturedByInlined { |a| ^if (a) { var c = 1; if (a) { c } } }
}
