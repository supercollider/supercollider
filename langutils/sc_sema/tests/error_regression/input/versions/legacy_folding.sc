// Legacy code only knows the values of literals, as the old compiler only put those in the frame, and computed
// everything else when the code ran. So nothing is folded, a variable is only constant when its initial value is a
// literal, and a member's initial value must be a literal.
LegacyFolding {
    var member = 1 + 2;
    classvar class_member = Object;
    const constant = #[1, a, [2]];

    // expect: -> 1
    literal { ^1 }
    // expect: -> -1
    negative { ^-1 }
    // expect: -> "a"
    string { ^"a" }
    // expect: -> \a
    symbol { ^\a }
    // expect: no diagnostics
    immutable_array { ^#[1, a, [2]] }
    // expect: -> nil
    without_initial_value { var a; ^a }
    // expect: -> 1
    literal_variable { var a = 1; ^a }

    // expect: no result
    arithmetic { ^1 + 1 }
    // expect: no result
    computed_variable { var a = 1 + 1; ^a }
    // expect: no result
    from_variable { var a = 1, b = a; ^b }
    // Only an initial value's brackets make it computed, elsewhere `(1)` is still the literal.
    // expect: -> 1
    bracketed { ^(1) }
    // expect: no result
    bracketed_sequence { ^(1; 2) }
    // expect: no result
    bracketed_value { var a = (1); ^a }
    // expect: no result
    bracketed_variable { var a(1); ^a }
    // expect: no result
    class_name { ^Object }
    // expect: no result
    range { ^(1..3) }
    // expect: no result
    slice { ^#[1, 2, 3][0..1] }
    // expect: no result
    array { ^[1, 2] }
    // expect: no result
    event { ^(a: 1) }
    // expect: no result
    pack { var #a, b = #[1, 2]; ^a }
    // expect: no result
    nested_return { |x| if (x) { ^1 }; ^2 }
}

// The same in 3.16, for comparison.
Folding {
    //# VERSION 3.16
    var member = 1 + 2;
    classvar class_member = Object;

    // expect: -> 2
    arithmetic { ^1 + 1 }
    // expect: -> 2
    computed_variable { var a = 1 + 1; ^a }
    // expect: -> 1
    from_variable { var a = 1, b = a; ^b }
    // expect: -> 1
    bracketed { ^(1) }
    // expect: -> 1
    bracketed_variable { var a(1); ^a }
    // expect: -> Object
    class_name { ^Object }
    // expect: -> 1
    pack { var #a, b = #[1, 2]; ^a }
    // expect: no result
    nested_return { |x| if (x) { ^1 }; ^2 }
}
