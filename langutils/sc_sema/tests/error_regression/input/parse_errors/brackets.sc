//# VERSION 3.16
// Brackets are paired before parsing. Those that don't match are reported and repaired, so the rest of the method,
// and the methods after it, are parsed as written.
Brackets {
    // An extra closing bracket is dropped.
    // expect: poisoned
    // expect-error: Unmatched Bracket.
    extra { ^(1 + 2]) }
    // One of the wrong kind closes the innermost bracket.
    // expect: poisoned
    // expect-error: Mismatched Bracket.
    wrong_kind { ^(1 + 2] }
    // An unclosed bracket is closed before the bracket around it closes.
    // expect: poisoned
    // expect-error: Unclosed Bracket.
    unclosed { ^(1 + 2 }
    // expect: poisoned
    // expect-error: Unclosed Bracket.
    unclosed_nested { |x| if (x) { ^[1, (2 }; ^3 }
    // expect: poisoned
    // expect-error: Mismatched Bracket.
    wrong_kind_nested { |x| if (x) { ^(1] }; ^2 }
    // A syntax error inside the repaired bracket is still reported.
    // expect: poisoned
    // expect-error: Unclosed Bracket.
    // expect-error: Unexpected Token.
    unclosed_with_error { ^(1 2 }
    // Parsed as written.
    // expect: -> 3
    after { ^3 }
}
