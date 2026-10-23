// A closed function, `#{ }`, in legacy code, the same lookup as 3.16, but a return only warns, as the old compiler
// compiled it, see closed_functions/lookup.sc and closed_functions/returns.sc.
LegacyClosedFunctions {
    var instance_var;

    // expect-reads: a -> a
    interpreter_member { ^#{ a } }
    // expect-error: Variable outside closed function
    outer_variable { var foo = 10; ^#{ foo } }
    // expect-error: Variable outside closed function
    instance_variable { ^#{ instance_var } }
    // Legacy code's frame holds `later` from the start, but a closed function can't see it.
    // expect-error: Variable outside closed function
    frame_variable { var f = #{ later }, later = 1; ^f }
    // expect-warning: Return in closed function
    // expect: not poisoned
    returns { ^#{ ^1 } }
    // expect-warning: Return in closed function
    // expect: not poisoned
    nested { ^#{ |x| if (x) { ^1 }; 2 } }
}
