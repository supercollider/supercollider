Foo {
    var instance_var;
    classvar class_var;

    assign_instance { instance_var = 1 }
    assign_class { class_var = 2.0 }
    assign_env { ~env_var = 3 }
    assign_undefined { undefined_var = 4 }
    assign_chain { instance_var = class_var = 5 }

    *class_method_assign_class { class_var = 6 }
    *class_method_assign_instance { instance_var = 7 }
}

Bar : Foo {
    assign_inherited { instance_var = 8 }
}
