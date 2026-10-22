Poisoning {
    var instance_var;
    classvar class_var;

    good { ^1 + 2 }
    good_assignment { instance_var = 3 }
    undefined { undefined_var = 4 }
    no_receiver { ^foo(bar: 1) }
    nested_error { ^1.foo(2.perform(3)) }
    error_in_array { ^#[1, 2.foo] }
    two_errors { undefined_var = 5; ^foo(bar: 1) }
    after_errors { ^6 }

    *good { ^1 + 2 }
    *cannot_assign { instance_var = 3 }
    *undefined { undefined_var = 4 }
    *no_receiver { ^foo(bar: 1) }
    *nested_error { ^1.foo(2.perform(3)) }
    *error_in_array { ^#[1, 2.foo] }
    *two_errors { undefined_var = 5; ^foo(bar: 1) }
    *after_errors { ^6 }
    *good_class_assignment { class_var = 3 }
}
