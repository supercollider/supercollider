//# VERSION 3.16
// An argument's default is only folded when it is bracketed, see the `= 2` on its message in the dump.
ArgumentDefaultFolding {
    unbracketed { arg a = 1 + 1; ^a }
    bracketed { arg a = (1 + 1); ^a }
    parenthesised { arg a(1 + 1); ^a }
    pipes_parenthesised { |a(1 + 1)| ^a }
    pipes_bracketed { |a = (1 + 1)| ^a }

    // Nothing in an unbracketed default is folded, even a bracketed default inside it.
    unbracketed_collection { arg a = [1 + 1, (2 + 2)]; ^a }
    unbracketed_function { arg f = { |b = (1 + 1)| var c = 2 + 2; c }; ^f }
    bracketed_function { arg f = ({ |b = (1 + 1)| var c = 2 + 2; c }); ^f }

    // A body's own constants still fold.
    body_folds { arg a = 1 + 1; ^2 + 2 }

    nested_return { arg a = 1 + 1, b = (2 + 2); if (a) { ^b }; ^a }
}
