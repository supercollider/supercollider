TestConstructorA {
    var a, b, c;
    *new { |...args, kwargs| ^super.performArgs(\newCopyArgs, args, kwargs) }
    value { ^[a, b, c] }
}

TestConstructor : UnitTest {
    test_kw_construct {
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
}
