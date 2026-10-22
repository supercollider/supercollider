ConstantResults {
    var instance_var;

    function_integer { ^{ 1 } }
    function_empty { ^{} }
    function_symbol { ^{ \a } }
    function_last_value { ^{ 1; 2; 3 } }
    function_with_variable { ^{ var x = 2; x } }
    function_mutable_result { ^{ [1, 2] } }
    function_unused_mutable_variable { ^{ var x = [1, 2]; 3 } }
    function_ends_with_declaration { ^{ var x = 1 } }
    function_with_argument { ^{ |x| 1 } }
    function_with_variadic_argument { ^{ |...x| 1 } }
    function_non_constant_item { ^{ instance_var; 1 } }
    function_non_constant_variable { ^{ var x = instance_var; 1 } }
    function_returns { ^{ ^1 } }
    function_reads_outer { var a = 1; ^{ a } }
    function_nested { ^{ { 1 } } }
    switch_cases { |a| ^switch(a) { 1 } { \one } { 2 } { \two } }

    // expect: -> 1
    method_explicit_return { ^1 }
    // expect: -> 2
    method_variable_return { var x = 2; ^x }
    // expect: no result
    method_implicit_return { 1 }
    // expect: no result
    method_empty { }
    // expect: no result
    method_with_argument { |x| ^1 }
    // expect: no result
    method_non_constant_item { instance_var; ^1 }
    // expect: no result
    method_primitive { _Primitive; ^1 }
    // expect-error: Variable not defined
    method_poisoned { ^undefined_variable }
    // expect: -> \a
    *class_method { ^\a }

    // expect: no result
    function_not_constant {
        if (3) { ^1 };
        ^2
    }

}
