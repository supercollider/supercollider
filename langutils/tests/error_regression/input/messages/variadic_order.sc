//# VERSION 3.16
VariadicOrder {
    var instance_var;

    variadic_last { ^1.foo(2, *3) }
    variadic_first { ^1.foo(*2, 3) }
    two_after { ^1.foo(*2, 3, 4) }
    keyword_after { ^1.foo(*2, bar: 3) }
    prefix { ^foo(1, *2, 3) }
    prefix_variadic_receiver { ^foo(*1, 2) }
    array_after { ^1.foo(*2, [3]) }
    put_variadic { instance_var[*0] = 1 }
    put_variadic_keyword { instance_var[*0, b: 1] = 2 }
    after_errors { ^5 }
}
