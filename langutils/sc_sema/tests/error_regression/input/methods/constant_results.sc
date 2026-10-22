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

    method_explicit_return { ^1 }
    method_variable_return { var x = 2; ^x }
    method_implicit_return { 1 }
    method_empty { }
    method_with_argument { |x| ^1 }
    method_non_constant_item { instance_var; ^1 }
    method_primitive { _Primitive; ^1 }
    method_poisoned { ^undefined_variable }
    *class_method { ^\a }

    function_not_constant {
        if (3) { ^1 };
        ^2
    }

}
