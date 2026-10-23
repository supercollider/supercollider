// A version anywhere else than the first thing in the file or in a class's body applies to nothing. It's a warning, so
// it doesn't poison the method it's in.
Misplaced {
    var a;
    //# VERSION 3.16

    // expect-warning: Misplaced version
    // expect: not poisoned
    in_method {
        //# VERSION 3.16
        if (a) { ^1 }; ^2
    }

    // expect-warning: Misplaced version
    // expect: not poisoned
    in_nested_function { |x|
        ^{
            //# VERSION 3.16
            if (x) { ^1 }; ^2
        }
    }
}

//# VERSION 3.16
MisplacedAfter {
    one { ^1 }
}

// Before the body's `{`, rather than the first thing in it.
MisplacedBeforeBody : Object
//# VERSION 3.16
{
    one { ^1 }
}

//# VERSION 3.16
