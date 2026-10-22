// `+`, `-`, `*` and `/` on constant Integers and Floats are folded. Integers wrap like 32 bit integers, anything with a
// Float is a Float, and `/` always returns a Float.
Arithmetic {
    const sum = 1 + 2;
    const mixed = 2 * 1.5;

    // expect: -> 3
    integer_add { ^1 + 2 }
    // expect: -> -1
    integer_subtract { ^1 - 2 }
    // expect: -> 6
    integer_multiply { ^2 * 3 }
    // expect: -> 2.5
    integer_divide { ^5 / 2 }
    // expect: -> 2.0
    integer_divide_exact { ^4 / 2 }
    // expect: -> 3.5
    float_add { ^1.5 + 2 }
    // expect: -> 0.5
    float_subtract { ^2 - 1.5 }
    // expect: -> 0.75
    float_multiply { ^0.5 * 1.5 }
    // expect: -> 0.25
    float_divide { ^0.5 / 2 }
    // Binary operators are evaluated left to right, without precedence.
    // expect: -> 9
    left_to_right { ^1 + 2 * 3 }
    // expect: -> 7
    parenthesised { ^1 + (2 * 3) }

    // expect: -> -2147483648
    overflow_wraps { ^2147483647 + 1 }
    // expect: -> 0
    multiply_wraps { ^65536 * 65536 }

    // expect: -> 3
    constant_locals { var a = 1, b = 2; ^a + b }
    // The constants are folded, `sum = 3` and `mixed = 3.0`, but reading one isn't yet.
    // expect: no result
    class_constant { ^sum }

    // expect: no result
    divide_by_zero { ^1 / 0 }
    // expect: no result
    float_divide_by_zero { ^1.0 / 0.0 }
    // expect: no result
    with_adverb { ^1 +.x 2 }
    // expect: no result
    not_numbers { ^"a" + 1 }
    // expect: no result
    other_operator { ^5 % 2 }
    // expect: no result
    argument { |a| ^a + 1 }
    // expect: no result
    mutable_local { var a = 1; a = 2; ^a + 1 }

    // expect: no diagnostics
    // expect: no result
    non_local_return { |x| if (x) { ^1 + 1 }; ^2 * 2 }
}
