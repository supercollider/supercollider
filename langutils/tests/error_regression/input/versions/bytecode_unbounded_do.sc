// bytecode
// `(a..).do(f)` is one forSeries, so the range is never made and can be unbounded, as in old sclang.
BytecodeUnboundedDo {
    postfix { |n| (0..).do { |i| if (i >= n) { ^i } }; ^nil }
    step { |n| (1, 3..).do({ |i| if (i >= n) { ^i } }); ^nil }
    prefix { |n| do((n..), { |i| i.postln }); ^n }
    bounded { |n| (0..n).do { |i| i.postln }; ^n }
}
