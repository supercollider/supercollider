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

    test_wrappers {
        var inlined = { |func|
        	{ |...args, kwargs|
        		func.performArgs(\value, args, kwargs)
        	}
        };
        var asVar = { |func|
        	{ |...args, kwargs|
        		var val = func.performArgs(\value, args, kwargs);
        		val
        	}
        };

        this.assertEquals(
         inlined.({ |a, b=100, c| [a, b, c] }).([1, 2]),
         [[1, 2], 100, nil]
        );
        this.assertEquals(
         asVar.({ |a, b=100, c| [a, b, c] }).([1, 2]),
         [[1, 2], 100, nil]
        );

        this.assertEquals(
         inlined.({ |a, b=100, c| [a, b, c] }).([1, 2], 42),
         [[1, 2], 42, nil]
        );
        this.assertEquals(
         asVar.({ |a, b=100, c| [a, b, c] }).([1, 2], 42),
         [[1, 2], 42, nil]
        );

        this.assertEquals(
         inlined.({ |a, b=100, c| [a, b, c] }).([1, 2], 42, c: 23),
         [[1, 2], 42, 23]
        );
        this.assertEquals(
         asVar.({ |a, b=100, c| [a, b, c] }).([1, 2], 42, c: 23),
         [[1, 2], 42, 23]
        );
    }
}
