//# VERSION 3.16
// A syntax error in a statement drops only that statement: up to its `;`, or the bracket closing its block. The
// statements around it, and the rest of the method, are kept.
StatementRecovery {
    // expect: poisoned
    middle { var a = 1; a = ; ^3 }
    // expect: poisoned
    first { 1 + ; ^2 }
    // expect: poisoned
    last { var a = 4; ^a; 1 + }
    // expect: poisoned
    nested { |x| if (x) { 1 + ; ^1 }; ^2 }
    // expect: poisoned
    in_function { var f = { 2 * ; 3 }; ^f.value }
    // expect: poisoned
    only { ) }
    // expect: -> 5
    after { ^5 }
}
