// A module compiled before the tests that name it, see test.py. Unversioned, so legacy.
Poisoned { }

// Within one module, the order isn't defined, so neither is used.
+Poisoned {
    duplicated { ^1 }
}

+Poisoned {
    duplicated { ^2 }
}
