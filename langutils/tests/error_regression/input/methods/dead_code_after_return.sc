//# VERSION 3.16
// What follows a `^` never runs, so it doesn't change the method's constant result. It is still compiled and warned about.
DeadCodeAfterReturn {
    // expect: -> 1
    second_return { ^1; ^2 }
    // expect: -> 1
    constant_after { ^1; 2 }
    // expect: -> 1
    variable_after { ^1; var a = 2; a }
    // expect: no result
    return_not_constant { ^this.foo; ^1 }
    // expect: -> 1
    not_constant_after { ^1; this.foo }
    foo { }
}
