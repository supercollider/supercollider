// frames
//# VERSION 3.16
// An inlined function has no frame, its body is in the frame it is inlined into, so the variables around it are as
// deep as they are outside it. A function that isn't inlined, even inside an inlined body, has its own.
FrameInlining {
    // expect: no diagnostics
    inlined { |a| var b = 1; ^if (a) { b } { a } }
    // expect: no diagnostics
    insideFunction { |a| ^{ |c| while { c.notNil } { c = c.next; a } } }
    // expect: no diagnostics
    functionInInlined { |a| ^if (a) { { a } } }
    // expect: no diagnostics
    notInlined { |a| ^if (a) { |x| x } }
    // expect: no diagnostics
    caseAndSwitch { |a| ^case { a.isNil } { switch (a, 1, { a }, { 2 }) } { a } }
}
