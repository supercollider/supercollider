//# VERSION 3.16
// `(a..).do(f)` is removed in 3.16, a range without an end is an error outside a generator.
UnboundedDo316 {
    postfix { |n| (0..).do { |i| if (i >= n) { ^i } }; ^nil }
    prefix { |n| do((n..), { |i| i.postln }); ^n }
    bounded { |n| (0..n).do { |i| if (i >= 2) { ^i } }; ^nil }
    generator { |n| {; if (i >= n) { ^i }, i <- (0..) }; ^nil }
}
