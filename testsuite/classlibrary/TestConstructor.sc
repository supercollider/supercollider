TestConstructorA {
    var a, b, c;
    *new { |...args, kwargs| ^super.performArgs(\newCopyArgs, args, kwargs) }
    value { ^[a, b, c] }
}

TestConstructorBase {
    var a, b, c;
}
TestConstructorDerived : TestConstructorBase {
    var d, e, f;
    *new { |d, e, f| ^super.newCopyArgs(1, 2, 3, d, e, f) }
    *newKw { |d, e, f| ^super.newCopyArgs(c: 3, d: d, e: e, f: f) }
	value { ^[a, b, c, d, e, f] }
}

TestConstructor : UnitTest {
    test_kw_constructor_with_performArg {
        this.assertEquals(
            TestConstructorA(1, b: 10, c: 20).(),
            [1, 10, 20]
        );
        this.assertEquals(
            TestConstructorA(b: 10, c: 20).(),
            [nil, 10, 20]
        );
        this.assertEquals(
            TestConstructorA(1, 2, 3, 4, 5).(),
            [1, 2, 3]
        );
        this.assertEquals(
            TestConstructorA(e: 20).(),
            [nil, nil, nil]
        );
    }
    test_kw_constructor {
        this.assertEquals(
            TestConstructorDerived.new(10, 11, 12).(),
            [1, 2, 3, 10, 11, 12]
        );
        this.assertEquals(
            TestConstructorDerived.newKw(10, 11, 12).(),
            [nil, nil, 3, 10, 11, 12]
        );
    }
}
