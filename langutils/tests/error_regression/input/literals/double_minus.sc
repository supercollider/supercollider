//# VERSION 3.16
// A sign is only applied once, as in the old grammar's `MINUS INTEGER`, so a second is a syntax error, like it is for
// floats and accidentals. A minus before a binary operator's right operand is still a sign.
DoubleMinus {
    // expect: -> 8
    binary { ^5 - - 3 }
    // expect: -> -3
    single { ^-3 }
    // expect: poisoned
    integer { ^- - 3 }
    // expect: poisoned
    integer_three { ^- - - 3 }
    // expect: poisoned
    hexadecimal { ^- - 0xFF }
    // expect: poisoned
    float { ^- - 3.5 }
}
