Foo {
    var instance_var;
    classvar class_var;

    // expect: no diagnostics
    assign_instance { instance_var = 1 }
    assign_class { class_var = 2.0 }
    // expect: no diagnostics
    assign_env { ~env_var = 3 }
    // expect-error: Variable not defined
    // expect: poisoned
    assign_undefined { undefined_var = 4 }
    assign_chain { instance_var = class_var = 5 }

    // expect: no diagnostics
    *class_method_assign_class { class_var = 6 }
    *class_method_assign_instance { instance_var = 7 }
}

Bar : Foo {
    // expect: no diagnostics
    assign_inherited { instance_var = 8 }
}
