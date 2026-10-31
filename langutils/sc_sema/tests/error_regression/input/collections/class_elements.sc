// bytecode
//# VERSION 3.16
// A class is only a constant by itself. In a constant collection it would be a symbol, as `#[Array]`'s is, so a
// collection with a class in it is built when it runs, and holds the class.
ClassElements {
    // expect: no result
    array { ^[Array, List] }
    // expect: no result
    nested { ^[1, [Array]] }
    // expect: no result
    event { ^(a: Array) }
    // expect: no result
    event_key { ^Event[Array, 1] }
    // expect: no result
    event_class { ^Event[\a, Array] }
    // expect: no result
    array_class { ^Array[Array] }
    // expect: -> [Array, 1]
    immutable { ^#[Array, 1] }
    // expect: -> Array
    alone { ^Array }
}
