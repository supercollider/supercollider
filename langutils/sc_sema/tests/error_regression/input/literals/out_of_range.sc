//# VERSION 3.16
// A number literal must be representable: Integers are 32 bit, a hexadecimal literal is the 32 bits, and a Float must
// not overflow or underflow to zero. There is no limit on how long the literal is, only on its value.
OutOfRange {
    // expect: -> 2147483647
    int_largest { ^2147483647 }
    // expect-error: Literal out of range
    // expect: poisoned
    int_too_large { ^2147483648 }
    // As the sign is part of the value, this fits.
    // expect: -> -2147483648
    int_smallest { ^-2147483648 }
    // expect-error: Literal out of range
    int_too_small { ^-2147483649 }
    // expect-error: Literal out of range
    int_many_digits { ^99999999999999999999 }

    // expect: -> 2147483647
    hex_largest_positive { ^0x7FFFFFFF }
    // The 32 bits, so this is -1.
    // expect: -> -1
    hex_all_bits { ^0xFFFFFFFF }
    // expect-error: Literal out of range
    hex_too_many_bits { ^0x100000000 }

    // expect: -> 2147483647
    radix_largest { ^16r7FFFFFFF }
    // Unlike hexadecimal, a radix literal is a value.
    // expect-error: Literal out of range
    radix_too_large { ^16r80000000 }
    // expect-error: Literal out of range
    radix_base_too_large { ^99999999999999999999999r1 }

    // expect: no diagnostics
    float_large { ^1e308 }
    // expect-error: Literal out of range
    float_overflow { ^1e309 }
    // expect-error: Literal out of range
    float_negative_overflow { ^-1e309 }
    // Subnormal, but not zero.
    // expect: no diagnostics
    float_small { ^1e-310 }
    // expect-error: Literal out of range
    float_underflow { ^1e-400 }
    // Zero is exact, not an underflow.
    // expect: -> 0.0
    float_zero_exponent { ^0e5 }
    // expect: no diagnostics
    float_inf { ^inf }

    // A long literal whose value is representable is fine.
    // expect: -> 1.0
    float_long { ^1.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 }
    // expect: -> 10.5
    radix_float { ^16rA.8 }
    // expect: -> 1.0
    radix_float_long { ^16r1.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 }
    // 16 to the 300.
    // expect-error: Literal out of range
    radix_float_overflow { ^16r1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0 }

    // The product with pi must be representable too.
    // expect-error: Literal out of range
    pi_overflow { ^1e308pi }
    // expect-error: Literal out of range
    pi_multiplier_out_of_range { ^2147483648pi }

    // In an array, the method is poisoned. A nested return's literal is checked too.
    // expect-error: Literal out of range
    // expect: poisoned
    in_array { |x| if (x) { ^[1, 1e999] }; ^2 }
}
