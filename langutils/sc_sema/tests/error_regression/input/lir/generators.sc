// lir
//# VERSION 3.16
// Generators are lowered as the old grammar rewrote them: `{: v, x <- s }` is `{ s.do { |x| v.yield } }.r`, a
// range source is `forSeries`, `var` is `{ |x| ... }.value(v)`, a condition and `:while` are inlined `if`s.
LirGenerators {
    // expect: no diagnostics
    iterate { |a| ^{: x, x <- a } }
    // expect: no diagnostics
    range { |n| ^{: [x, i], x i <- (1, 3..n) } }
    // expect: no diagnostics
    declare { |a| ^{: y, x <- a, var y = x + 1 } }
    // expect: no diagnostics
    condition { |a| ^{: x, x <- a, x.odd, x.notNil } }
    // expect: no diagnostics
    effect_and_while { |a| ^{: x, x <- a, :: x.postln, :while x < 10 } }
    // expect: no diagnostics
    immediate { |a| ^{; x.postln, x <- a } }
    // expect: no diagnostics
    range_generator { |n| ^(: 1, 3..n) }
    // expect: no diagnostics
    non_local_return { |a| ^{: if (x) { ^1 } { 2 }, x <- a } }
}
