//# VERSION 3.16
// frames

FrameGenerators {
    // expect: no diagnostics
    routine { |a| ^{: x + i + y + a, x i <- a, var y = x * 2, y > 1 } }
    // expect: no diagnostics
    immediate { |a| var total = 0; {; total = total + x, x <- a }; ^total }
}
