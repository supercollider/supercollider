// bytecode
//# VERSION 3.16
// From 3.16 an inlined function's uncaptured variables are slots of the method's frame, set as the body runs, see
// versions/bytecode_hoisting for legacy, which sends these.
BytecodeHoisting {
    ifVar { |a| ^if (a) { var c = a.foo; c } { 2 } }
    loop { |a| while { a.next } { var c; c = c ? 0 + 1 } }
    nonLocalReturn { |a| if (a) { var c = a.foo; if (c) { ^1 }; ^c }; ^2 }
    captured { |a| ^if (a) { var c = 1; { c } } }
    // A hoisted variable is in the method's frame, but is still only in scope in its function.
    // expect-error: Variable not defined
    outOfScope { |a|
        if (a) {
            var b = 10;
            b
        };
        ^b
    }
}
