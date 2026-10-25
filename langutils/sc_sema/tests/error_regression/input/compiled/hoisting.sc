// compiled
//# VERSION 3.16
// An inlined function's variables are hoisted into the frame it is inlined into, after the frame's own. They are set
// each time the function runs, so have no prototype, even with a literal initial value.
Hoisting {
    // expect: no diagnostics
    method { |a| var b = 1; if (a) { var c = 2, d; c + d } { var e = 3; e }; ^b }
    // expect: no diagnostics
    loop { var i = 0; while { i < 3 } { var j = 1; i = i + j }; ^i }
}
