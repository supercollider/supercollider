//# VERSION 3.16
// The lexer reports what is wrong with the token, rather than the parser reporting it as unexpected.
// The method is kept, poisoned.
LexerErrors {
    // expect: poisoned
    // expect-error: Missing Exponent.
    missing_exponent { ^1.5e+ }
    // expect: poisoned
    // expect-error: Unexpected Character.
    not_ascii { ^é }
    // expect: poisoned
    // expect-error: Invalid Character.
    char_not_ascii { ^$é }
    // expect: poisoned
    // expect-error: Invalid Character.
    in_nested_function { |x| if (x) { ^$	 }; ^2 }
}
