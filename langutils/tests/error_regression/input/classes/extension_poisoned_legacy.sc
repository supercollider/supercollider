// module: extension_poisoned_legacy
// Unversioned, so legacy. A later module replaces an earlier module's extension, but not the error of the earlier
// module defining it twice, so the method stays poisoned.
+Poisoned {
    // expect-warning: Extension overwritten
    // expect: poisoned
    duplicated { ^3 }
}
