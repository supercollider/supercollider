// `(a..).do(f)` is one forSeries, so the range can be unbounded, with a warning as 3.16 removes it. Any other send
// of do would make the range.
UnboundedDo {
    postfix { |n| (0..).do { |i| if (i >= n) { ^i } }; ^nil }
    prefix { |n| do((n..), { |i| i.postln }); ^n }
    noFunction { ^(0..).do }
    keyword { |f| ^(0..).do(function: f) }
    variadic { |f| ^(0..).do(*f) }
    adverb { |f| ^(0..).do.x(f) }
    otherSelector { |f| ^(0..).collect(f) }
    infix { |f| ^((0..) do: f) }
}
