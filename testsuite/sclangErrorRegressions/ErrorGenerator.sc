ErrorGenerator {

    *doesNotUnderstandErrorMethodImpl { |i, a...args, kwargs| 
        var var1 = 100.0.rand;
        if (i == 0) {
            1.meow(10, args: \woof)
        } {
            this.performArgs(\doesNotUnderstandErrorMethodImpl,[ i - 1], kwargs)
        }
    }

    *doesNotUnderstandErrorMethod {
        this.doesNotUnderstandErrorMethodImpl(10, 32, someThing: 10)
    }

    *doesNotUnderstandErrorFunction {
        var f = { |i, a...args, kwargs| 
            var var1 = 100.0.rand;
            if (i == 0) {
                1.meow(10, args: \woof)
            } {
                f.performArgs(\value,[ i - 1], kwargs)
            }
        };

        f.(10, \23423, 123, 32, foo: \bar)
    }


    *error {
        Error("This is a test error").throw
    }

    *errorWrapper {
        try {
            Error("some other error").throw;
        } { |er|
            ErrorWrapper(er, "Wrapping some error").throw
        }
    }

    *deprecate {
        this.deprecated(thisMethod, this.class.findMethod(\deprecatedAlternative))
    }

    *deprecatedAlternative {}

    *mustBeBoolean {
        if (\10) {} {}
    }

    *notYetImplementedError {
        this.notYetImplemented;
    }

    *immutable {
        var a = #[1];
        a[0] = 10;
    }

    *subclassResponsibilityError {
        this.subclassResponsibility(thisMethod);
    }

    *shouldNotImplementError {
        this.shouldNotImplement(thisMethod);
    }

    // leave these unset to throw an error
    *primitiveErr { |a, b|
        _ObjectCopyRange
    }

}
