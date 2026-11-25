Foo : Bar {
    classvar <a,
        <b;
    classvar
        <e,
        <f;
    var <>c = 1;
    const d = 2;
    *new { |x|
        ^super.new
    }
    foo { arg y;
        _Prim
        ^this.primitiveFailed
    }
    bar
    {
        ^1 +
            2
    }
}
+ Bar {
    baz { ^1 }
}
