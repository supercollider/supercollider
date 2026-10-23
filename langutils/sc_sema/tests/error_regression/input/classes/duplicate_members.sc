//# VERSION 3.16
Dup1 {
    var a, a;
}

Dup2B {
    var a;
}

Dup2D : Dup2B {
    var a;
}

// With initial values, which are declared by another path.
DupWithValues {
    var both = 1, both = 2;
    var first;
    var first = 1;
    var second = 1;
    var second;
    classvar shared = 1, shared = 2;
}
