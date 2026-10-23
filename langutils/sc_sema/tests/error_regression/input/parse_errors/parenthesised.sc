//# VERSION 3.16
// An error inside brackets is recovered from at the matching bracket, so the rest of the method and class still parse.
// The error poisons the method, the parser has already reported it.
ParenthesisedError {
    // expect: poisoned
    // expect-error: Missing Expression.
    missing_operand { ^(1 +) }
    // expect: poisoned
    // expect-error: Missing Operator.
    missing_operator { ^(1 2) }
    // The nested brackets are skipped, recovery stops at the bracket matching the error's.
    // expect: poisoned
    // expect-error: Missing Operator.
    nested { ^(1 + (2 3) * 4) }
    // expect: poisoned
    // expect-error: Missing Operator.
    skips_nested { ^(1 2 (3 + 4)) + 5 }
    // Each error is reported, as recovery resumes at the matching bracket.
    // expect: poisoned
    // expect-error: Missing Operator.
    two { ^[(1 2), (3 4)] }
    // expect: poisoned
    // expect-error: Missing Operator.
    as_receiver { ^(1 2).foo }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_variable { var a = (1 2); ^a }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_range { ^(1..2 3) }
    // expect: poisoned
    // expect-error: Missing Expression.
    in_nested_function { |x| if (x) { ^(1 +) }; ^2 }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_nested_return { |x| ^{ if (x) { ^(1 2) }; 3 } }

    // expect: -> 3
    // expect: not poisoned
    after_errors { ^1 + 2 }
}
