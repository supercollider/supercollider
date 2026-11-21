// module: extensions_legacy
// Unversioned, so legacy, as is the module before it. Extensions in different modules can define the same method,
// the later module's is used, as in the old compiler, with a warning.
+Foo {
    // expect-warning: Extension overwritten
    // expect: -> 2
    overwritten { ^2 }
    // expect-warning: Extension overwritten
    // expect: -> 2
    conflicting { ^2 }
    // Overriding the class's own method is fine.
    // expect: no diagnostics
    // expect: -> 2
    own { ^2 }
    // expect-warning: Extension overwritten
    // expect: -> 2
    *class_overwritten { ^2 }

    // Within one module, the order isn't defined, so neither is used, in any version.
    // expect-error: Duplicate extension
    // expect: poisoned
    duplicated { ^1 }
}

+Foo {
    duplicated { ^2 }
}
