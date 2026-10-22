Foo {
    var bar = 10;
    var <getter, >setter;
    classvar class_var = 20;
    const constant = 30;

    // expect-reads: bar -> bar
    // expect: no diagnostics
    a_method { ^bar }
    // expect-reads: bar -> bar
    // expect: no result
    not_constant { ^bar }
    // expect-reads: getter -> getter
    with_getter { ^getter }
    // expect-reads: setter -> setter
    with_setter { ^setter }
    // expect-reads: class_var -> class_var
    class_variable { ^class_var }
    // expect-reads: constant -> constant
    constant_member { ^constant }
    // expect-reads: bar -> bar
    in_expression { ^bar + 1 }
    // expect-reads: bar -> bar
    after_assigning { bar = 1; ^bar }
    // expect-reads: bar -> bar
    in_block { ^{ bar } }
    // expect-reads: bar -> bar
    in_nested_block { ^{ { bar } } }
    // expect-reads: bar -> bar
    in_generator { ^{: bar + x, x <- [1, 2] } }
    // expect-reads: bar -> bar = 1
    hidden_by_variable { var bar = 1; ^bar }
    // expect-reads: bar -> bar
    hidden_by_argument { |bar| ^bar }
    // expect-reads: bar -> bar = 1; bar
    hidden_in_block_only { ^[{ var bar = 1; bar }, bar] }

    // expect-reads: class_var -> class_var
    *class_method { ^class_var }
    // expect-error: Variable not defined
    *instance_variable_in_class_method { ^bar }
}

Sub : Foo {
    var sub_var = 40;

    // expect-reads: bar -> bar
    inherited { ^bar }
    // expect-reads: class_var -> class_var
    inherited_class_variable { ^class_var }
    // expect-reads: sub_var -> sub_var
    // expect-reads: bar -> bar
    own_and_inherited { ^[sub_var, bar] }
}
