TestPerformListObj {
    f1 {|a, b, c| ^(a: a, b: b, c: c) }
    f2 {|a, b, c, selector| ^(selector: selector, a: a, b: b, c: c) }
}

TestPerformList : UnitTest {

    test_basic {
        this.assertEquals(
            TestPerformListObj().performList(\f1, 1, 2, 3),
            (a: 1, b: 2, c: 3)
        );
        this.assertEquals(
            TestPerformListObj().performList(\f1, 1, 2, 3, (0..100000)),
            (a: 1, b: 2, c: 3)
        );
        this.assertEquals(
            TestPerformListObj().performList(\f1, 1, 2, 3, 4, 5, 6),
            (a: 1, b: 2, c: 3)
        );
        this.assertEquals(
            TestPerformListObj().performList(\f1, 1, 2, c: 3),
            (a: 1, b: 2, c: 3)
        );
    }

    test_selectorAsArg {
        this.assertEquals(
            TestPerformListObj().performList(\f2, 1, 2, 3, selector: 4),
            (a: 1, b: 2, c: 3, selector: 4)
        );
    }

    test_normalCall {
        this.assertEquals(
            TestPerformListObj().f2(1, 2, 3, 4),
            (a: 1, b: 2, c: 3, selector: 4)
        );
        this.assertEquals(
            TestPerformListObj().f2(1, 2, 3, selector: 4),
            (a: 1, b: 2, c: 3, selector: 4)
        );
        this.assertEquals(
            TestPerformListObj().f2(*[1, 2, 3, 4]),
            (a: 1, b: 2, c: 3, selector: 4)
        );
        this.assertEquals(
            TestPerformListObj().f2(*[1, 2, 3], selector: 4),
            (a: 1, b: 2, c: 3, selector: 4)
        );
    }
}
