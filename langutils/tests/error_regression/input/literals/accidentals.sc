//# VERSION 3.16
// A scale degree with an accidental is a Float, as the old compiler computed it. Each `s` (sharp) or `b` (flat) is a
// tenth, at most 4, and a single one followed by digits is that many thousandths, at most 499.
Accidentals {
    // expect: no diagnostics
    // expect: -> 3.1
    sharp { ^3s }
    // expect: -> 2.9
    flat { ^3b }
    // expect: -> 3.2
    double_sharp { ^3ss }
    // expect: -> 2.8
    double_flat { ^3bb }
    // At most four.
    // expect: -> 3.4
    many_sharps { ^3sssss }
    // expect: -> 3.04
    sharp_cents { ^3s40 }
    // expect: -> 2.96
    flat_cents { ^3b40 }
    // At most 499, so it can't reach the next degree.
    // expect: -> 3.499
    cents_clamped { ^3s999 }
    // Only the degree is negated, the accidental still raises or lowers it.
    // expect: -> -2.9
    negative_sharp { ^-3s }
    // expect: -> -3.1
    negative_flat { ^-3b }
    // expect: -> -2.96
    negative_cents { ^-3s40 }
    // A literal, so it can be in an immutable array, and is folded in legacy code too.
    // expect: no diagnostics
    immutable_array { ^#[1s, 2b] }
    // expect-error: Literal out of range
    // expect: poisoned
    degree_out_of_range { |x| if (x) { ^9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999s }; ^2 }
}
