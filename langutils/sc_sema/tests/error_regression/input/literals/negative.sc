NegativeLiterals {
    const negative_const = -3;

    integer { ^-5 }
    float { ^-2.5 }
    exponent { ^-1e3 }
    hexadecimal { ^-0x10 }
    radix { ^-2r101 }
    pi_alone { ^-pi }
    pi_multiple { ^-2pi }
    in_array { ^[-1, 2] }
    in_range { ^(-2..2) }
    binary_minus { ^1 - 2 }
}
