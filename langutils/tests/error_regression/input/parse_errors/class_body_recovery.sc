//# VERSION 3.16
// A syntax error between a class's members and methods drops only the item it is in: up to the next method for junk
// between methods, up to the declaration's `;` for a member. The class and the methods around it compile.
JunkBetweenMethods {
    // expect: -> 1
    before { ^1 }
    // A name with no `{`.
    incomplete
    // expect: -> 2
    after { ^2 }
    ;
    // expect: -> 3
    *class_method { ^3 }
    + 3
    // expect: no result
    last { |x| if (x) { ^4 }; ^5 }
}

// An error right after the class's `{`, before any members.
StrayFirst {;
    // expect: -> 1
    method { ^1 }
}

BrokenMembers {
    var a = , b;
    classvar c, ;
    const d = 1 +;
    var e;
    // expect: -> 1
    method { ^1 }
    // expect: no result
    // expect-error: Variable not defined
    uses_dropped { ^a }
}

// The class's own `}` ends the skipping.
MissingMethodBody { x { ^1 } y }

+Object {
    fine { ^1 }
    junk
    other { ^2 }
}
