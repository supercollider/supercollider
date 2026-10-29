//# VERSION 3.16
// `legacy` is a version too, for a class (or file) that must not follow the later rules.
LegacyKeyword {
    //# VERSION legacy
    // expect: no diagnostics
    one { ^1 }
}

LegacyKeywordFile {
    // expect: no diagnostics
    two { ^2 }
}

// Anything after it is malformed, and the class is legacy.
LegacyKeywordTrailing {
    //# VERSION legacy 3.16
    three { ^3 }
}
