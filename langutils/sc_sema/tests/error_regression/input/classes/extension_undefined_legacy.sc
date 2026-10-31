// The old compiler skipped an extension of a class that isn't defined without a word, and legacy code does that on
// purpose, e.g., to extend a class of an optional quark.
Defined { }

+ Undefined {
    method { ^1 }
}

+ Defined {
    // expect: -> 2
    method { ^2 }
}
