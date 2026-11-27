// An extension's methods use its version, not its class's, as an extension is often by another author.
Extended {
    //# VERSION 3.16
    own { ^1 }
    *class_own { ^1 }
}

+ Extended {
    // expect: no diagnostics
    legacy_extension { ^2 }
}

+ Extended {
    //# VERSION 3.16
    versioned_extension { ^3 }
    *class_versioned_extension { ^3 }
}

LegacyExtended {
    own { ^1 }
}

+ LegacyExtended {
    //# VERSION 3.16
    versioned_extension { ^2 }
}
