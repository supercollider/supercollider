//# VERSION 3.16
// Errors in declarations and argument lists, and that real errors in the statements kept are still reported.
StatementDeclarations {
    // expect: poisoned
    bad_var { var a = ; ^1 }
    // expect: poisoned
    bad_var_name { var ; ^1 }
    // The dropped declaration's variable isn't known, which is reported as well.
    // expect: poisoned
    uses_dropped { var a = ; ^a }
    // expect: poisoned
    bad_default { |x = | ^x }
    // expect: poisoned
    bad_default_then_statements { |x = , y| var z = 1; ^z }
    // expect: poisoned
    bad_arg { arg x = ; ^x }
    // Errors in the statements around the dropped one are reported too.
    // expect: poisoned
    // expect-error: Variable not defined
    later_error { 1 + ; ^undefined_variable }
    // expect: poisoned
    // expect-error: Variable already declared
    duplicate { var a; 1 + ; var a; ^a }
    // A good method after all of them.
    // expect: -> 7
    good { ^7 }
}
