// The expected tokens are all listed, with expression and operator tokens named as a group, and what was found is
// shown as written.
ExpectedTokens {
    // expect: poisoned
    // expect-error: Unexpected Token.
    missing_operand { ^(1 + ) }
    // expect: poisoned
    // expect-error: Unexpected Token.
    adjacent { ^(1 "a string") }
    // A multiline token is shown up to its first line.
    // expect: poisoned
    // expect-error: Unexpected Token.
    multiline { ^(1 "first
second") }
    // expect: poisoned
    // expect-error: Unexpected Token.
    in_nested_function { |x| if (x) { ^(x 1) }; ^2 }
}
