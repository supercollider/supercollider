// lir
//# VERSION 3.16
// A literal is shared, as it is immutable. A folded collection or message makes a new value each time, so a mutable
// one is pushed and copied, with each mutable element copied too. A folded expression list is its last expression.
LirConstants {
    // expect: -> "text"
    literal_string { ^"text" }
    // expect: -> [1, 2]
    literal_array { ^#[1, 2] }
    // expect: -> [1, 2]
    flat { ^[1, 2] }
    // expect: -> [1, [2, 3], "four"]
    nested { ^[1, [2, 3], "four"] }
    // expect: -> "text"
    expression_list { ^("text") }
    // expect: -> (a: [1])
    event { ^(a: [1]) }
    // expect: -> 3
    immutable { ^1 + 2 }
}
