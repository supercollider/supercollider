//# VERSION 3.16
// An extension of a class that isn't defined is skipped, with a warning as it is likely a typo.
Defined { }

+ Undefined {
    method { ^1 }
}

+ Defined {
    // expect: -> 2
    method { ^2 }
}
