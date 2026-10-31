//# VERSION 3.16
// A version on the file's first line is used by every class without its own.
FileDefault {
    // expect: no diagnostics
    // expect: -> 1
    one { ^1 }
    // expect: no result
    nested_return { |x| if (x) { ^1 }; ^2 }
}

FileDefaultSecond {
    // expect: -> 2
    two { ^2 }
}
