// A version that isn't `major.minor`, or is newer than the compiler, is an error, and the class is legacy. As it isn't
// in a method, no method is poisoned. Something that only looks like a version is a comment, as code already has them.
BadMalformed {
    //# VERSION 3
    // expect: not poisoned
    one { ^1 }
}

BadTrailing {
    //# VERSION 3.16 // a comment after it
    one { ^1 }
}

BadUnknown {
    //# VERSION 99.0
    one { ^1 }
}

// The last of two is used.
BadDuplicate {
    //# VERSION 3.13
    //# VERSION 3.16
    one { ^1 }
}

NotAVersion {
    //# version 3.16
    //#VERSION 3.16
    //# VERSIONS 3.16
    // expect: no diagnostics
    one { ^1 }
}
