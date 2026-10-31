//# VERSION 3.16
// The token found is shortened in the message, which must not cut a multi byte character in half.
FoundTextUtf8 {
    // expect: poisoned
    // expect-error: Missing Operator.
    long_string { ^1 "éééééééééééééééééééé" }
}
