// In legacy code, as in the old compiler, a block's arguments and variables are all in its frame, so every argument
// default and initial value can see all of them. Argument defaults run first, then the variables' initial values, in
// order. A literal initial value is already in the frame before anything runs, anything else is nil until it does.
// 3.16 only allows a name after its declaration.
LegacyLookup {
    // `x1` isn't a literal, so it is nil when `a1`'s default runs, and `a1` is nil.
    // expect-warning: Variable used before its declaration
    // expect-reads: x1 -> x1 =2 + 2
    // expect: not poisoned
    // expect: no result
    argument_sees_computed_variable {
        ^{
            arg a1 = x1;
            var x1 =2 + 2;
            a1
        }.()
    }

    // `x1` is a literal, so it is already in the frame, and `a1` is 4, but `a1` isn't a literal so it isn't known.
    // TODO: check this returns 4 once code can be run.
    // expect-warning: Variable used before its declaration
    // expect-reads: x1 -> x1 = 4
    // expect: not poisoned
    // expect: no result
    argument_sees_literal_variable {
        ^{
            arg a1 = x1;
            var x1 = 4;
            a1
        }.()
    }
}

LookupThreeSixteen {
    //# VERSION 3.16

    // expect-error: Variable not defined
    // expect: poisoned
    argument_sees_computed_variable {
        ^{
            arg a1 = x1;
            var x1 =2 + 2;
            a1
        }.()
    }

    // expect-error: Variable not defined
    // expect: poisoned
    argument_sees_literal_variable {
        ^{
            arg a1 = x1;
            var x1 = 4;
            a1
        }.()
    }
}
