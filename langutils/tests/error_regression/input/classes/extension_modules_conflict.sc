// module: extensions_legacy
// From 3.16, extensions in different modules can't define the same method. When either is 3.16 it is an error, and the
// earlier one, in the module before, is kept, poisoned. The legacy extensions of the module before are unaffected.
+Foo {
    //# VERSION 3.16

    conflicting { ^2 }

    // Overriding the class's own method is still fine.
    // expect: no diagnostics
    // expect: -> 2
    own { ^2 }
}
