// A module compiled before the tests that name it, see test.py. Unversioned, so legacy.
Foo {
    own { ^1 }
}

+Foo {
    overwritten { ^1 }
    conflicting { ^1 }
    *class_overwritten { ^1 }
}
