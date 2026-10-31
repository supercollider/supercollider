//# VERSION 3.16
// The expected tokens are all listed, with expression and operator tokens named as a group, and what was found is
// shown as written. Missing operators and operands have their own messages, see missing_operator.sc.
ExpectedTokens {
    // expect: poisoned
    // expect-error: Unexpected Token.
    not_an_expression { ^(1, ) }
    // expect: poisoned
    // expect-error: Unexpected Token.
    not_an_operator { ^(1 = 2) }
    // A multiline token is shown up to its first line.
    // expect: poisoned
    // expect-error: Unexpected Token.
    multiline { var "first
second"; ^1 }
    // expect: poisoned
    // expect-error: Unexpected Token.
    in_nested_function { |x| if (x) { ^(1 = x) }; ^2 }
}
