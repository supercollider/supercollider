TestKwargs_Obj {
    foo { |a, b...args, kwargs|
        var x = 100;
        var y = 101;
         ^(a: a, b: b, args: args, kwargs: kwargs)
    }
    doesNotUnderstand { |selector ...args, kwargs|
        ^(selector: selector, args: args, kwargs: kwargs)
    }
}

TestKwargs : UnitTest {
    test_basic_arg_passing {
        this.assertEquals(
            TestKwargs_Obj().foo(1),
            (a: 1, args: [], kwargs: [])
        );

        this.assertEquals(
            TestKwargs_Obj().foo(1, 2, 3, 4),
            (a: 1, b: 2, args: [3, 4], kwargs: [])
        );

        this.assertEquals(
            TestKwargs_Obj().foo(a: 1, b: 2),
            (a: 1, b: 2, args: [], kwargs: [])
        );

        this.assertEquals(
            TestKwargs_Obj().foo(1, 2, 3, e: 1, f: 2),
            (a: 1, b: 2, args: [3], kwargs: [\e, 1, \f, 2])
        );
    }

    test_doesNotUnderstand {
        this.assertEquals(
            TestKwargs_Obj().bar(1,2, a: 3, b: 4),
            (selector: \bar, args: [1, 2], kwargs: [a: 3, b: 4])
        )
    }

    test_function_perform_list {
        var f = {|a, b, c... args, kwargs| (a: a, b: b, c: c, args: args, kwargs: kwargs) };
        this.assertEquals(
            f.(1, *[2, 3, 4], foo: 10),
            (a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
        );
        this.assertEquals(
            f.performList(\value, 1, [2, 3, 4], foo: 10),
            (a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
        );
        this.assertEquals(
            f.functionPerformList(\value, 1, [2, 3, 4], foo: 10),
            (a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
        );
    }
}