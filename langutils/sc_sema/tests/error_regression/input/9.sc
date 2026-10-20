AbstractObjectExperimental {}

Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}

Class {}

String {}

True {}

False {}

Nil {}

Float {}

Integer {}

Char {}

Symbol {}

Foo {
    var bar, car, tar;
    const foo_const1, foo_const_2;
}



Foo2 : Foo {
    var foo2_a, foo2_b;

    classvar foo2_class_var;

}
