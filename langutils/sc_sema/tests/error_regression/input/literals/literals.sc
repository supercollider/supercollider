Literals {
    int { ^1 }
    int_radix { ^2r101 }
    float { ^2.5 }
    float_exponent { ^1e3 }
    pi_alone { ^pi }
    pi_multiple { ^2pi }
    char { ^$a }
    string { ^"hello" }
    string_concatenated { ^"hello" " world" }
    boolean_true { ^true }
    boolean_false { ^false }
    nil_value { ^nil }
    symbol_quote { ^'a symbol' }
    symbol_slash { ^\symbol }
    accidental { ^2s }
}
