// The lexer reports what is wrong with the token, rather than the parser reporting it as unexpected.
// TODO: recovery loses the methods, so they have no assertions.
LexerErrors {
    missing_exponent { ^1.5e+ }
    not_ascii { ^é }
    char_not_ascii { ^$é }
    in_nested_function { |x| if (x) { ^$	 }; ^2 }
}
