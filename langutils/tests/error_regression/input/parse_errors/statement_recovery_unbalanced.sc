//# VERSION 3.16
// Brackets that don't pair up, in a statement being dropped.
StatementUnbalanced {
    // expect: poisoned
    stray_closer { ] }
}

StatementUnbalancedCall {
    // expect: poisoned
    mismatched_in_call { foo(1 ] 2); ^3 }
}

StatementUnbalancedOpen {
    // expect: poisoned
    unclosed_in_array { 1 + ; [1, 2; ^5 }
}
