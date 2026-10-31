//# VERSION 3.16
Arrays {
    constant { ^[1, 2.5, "three"] }
    empty { ^[] }
    nested_constant { ^[[1, 2], [\a]] }
    keyword_elements { ^[a: 1, b: 2] }
    not_constant { ^[1, 2.foo] }
    nested_not_constant { ^[1, [2.foo]] }
    immutable { ^#[1, name, \symbol] }
    immutable_not_literal { ^#[1, 2.foo] }
    immutable_nested { ^#[[a, 1], b] }
}
