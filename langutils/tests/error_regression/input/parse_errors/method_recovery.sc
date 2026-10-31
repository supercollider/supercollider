//# VERSION 3.16
// A syntax error outside brackets keeps the method, poisoned, and parsing carries on after its closing `}`, so the
// methods around it compile and the next one's error is reported too.
MethodRecovery {
    // expect: -> 1
    before { ^1 }
    // expect: poisoned
    // expect-error: Missing Operator.
    missing_operator { ^1 2 }
    // expect: poisoned
    // expect-error: Missing Expression.
    missing_operand { ^1 + }
    // expect: poisoned
    // expect-error: Missing Operator.
    with_arguments { |a, b| ^a b }
    // expect: poisoned
    // expect-error: Unexpected Token.
    in_immutable_array { ^#[1, ^2] }
    // expect: poisoned
    // expect-error: Missing Operator.
    in_nested_function { |x| if (x) { ^1 }; ^x 2 }
    // expect: poisoned
    // expect-error: Missing Operator.
    *class_method { ^1 2 }
    // expect: -> 3
    after { ^3 }
}
