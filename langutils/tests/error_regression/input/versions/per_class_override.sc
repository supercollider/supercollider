//# VERSION 3.13
// The first thing in a class's body is its version, used instead of the file's, here legacy, as 3.13 is before the
// first release that changed the rules.
PerClass {
    //# VERSION 3.16
    // expect: no diagnostics
    one { ^1 }
}

PerClassFile {
    // expect: no diagnostics
    two { ^2 }
}
