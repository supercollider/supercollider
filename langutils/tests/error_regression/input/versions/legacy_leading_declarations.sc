// Legacy code, as the old compiler's grammar, only declares variables at the start of a body, see
// LeadingDeclarations316 for the same methods in 3.16.
LegacyLeadingDeclarations {
    // expect: no diagnostics
    leading { var a = 1; var b = 2; ^a + b }
    // expect: no diagnostics
    only_declarations { var a; }
    // expect-error: Variable declared after an expression
    after_expression { 1; var a = 2; ^a }
    // expect-error: Variable declared after an expression
    in_function { ^{ 1; var a = 2; a } }
    // expect-error: Variable declared after an expression
    nested_with_return { |x| ^{ if (x) { ^1 }; var a = 2; a } }
    // A function's own leading declarations are fine after the method's expressions.
    // expect: no diagnostics
    function_after_expression { 1; ^{ var a = 2; a } }
    // The new grammar puts the primitive before the variables, so they are leading.
    // expect: no diagnostics
    primitive { |x| _BasicSwap var a = x; ^a }
}

LeadingDeclarations316 {
    //# VERSION 3.16
    // expect: no diagnostics
    after_expression { 1; var a = 2; ^a }
    // expect: no diagnostics
    in_function { ^{ 1; var a = 2; a } }
    // expect: no diagnostics
    nested_with_return { |x| ^{ if (x) { ^1 }; var a = 2; a } }
}
