// A version as the first thing in the file is used by every class without its own. Comments before it are fine.
//# VERSION 3.16
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
