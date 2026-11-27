//# VERSION 3.16
// Two expressions next to each other are missing an operator, and an operator without an expression after it is
// missing one. The separator suggested is what could go there.
MissingOperator {
    // expect: poisoned
    // expect-error: Missing Operator.
    numbers { ^1 2 }
    // expect: poisoned
    // expect-error: Missing Operator.
    names { |a, b| ^a b }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_array { ^[1 2] }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_arguments { ^this.foo(1 2) }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_brackets { ^(1 2) }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_brackets_in_array { ^[(1 2)] }
    // expect: poisoned
    // expect-error: Missing Operator.
    after_call { ^1.foo(2) 3 }
    // A multiline token is shown up to its first line.
    // expect: poisoned
    // expect-error: Missing Operator.
    multiline { ^1 "first
second" }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_nested_function { |x| if (x) { ^1 2 }; ^2 }

    // expect: poisoned
    // expect-error: Missing Expression.
    operand_at_end { ^1 + }
    // expect: poisoned
    // expect-error: Missing Expression.
    operand_in_brackets { ^(1 * ) }
    // expect: poisoned
    // expect-error: Missing Expression.
    keyword_operator { ^1 max: ; }
    // A keyword argument is the same token as a keyword binary operator.
    // expect: poisoned
    // expect-error: Missing Expression.
    keyword_argument { ^this.foo(a: ) }
    // expect: poisoned
    // expect-error: Missing Expression.
    operand_in_nested_function { |x| if (x) { ^1 - }; ^2 }

    // An argument list's closing `|` isn't an operator.
    // expect: poisoned
    // expect-error: Unexpected Token.
    after_arguments { ^{ |a| * } }
}
