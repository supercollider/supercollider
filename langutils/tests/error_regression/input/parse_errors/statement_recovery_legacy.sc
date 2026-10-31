// A Legacy class, which has no version comment.
StatementLegacy {
    // expect: poisoned
    middle { var a = 1; a = ; ^3 }
    // expect: poisoned
    nested { |x| if (x) { 1 + ; ^1 }; ^2 }
    // expect: -> 4
    good { ^4 }
}
