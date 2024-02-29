/*
	TestRational.run
*/

Rational : Number {
	var <numerator, <denominator;

	*new { arg numerator=1.0, denominator=1.0;
		if(numerator.isKindOf(String)) { ^numerator.asRational };
		^super.newCopyArgs(numerator, denominator).reduce
	}

	reduce {
		var d;
		// check for bad terms
        if (numerator == inf) { ^inf };
        if (numerator == -inf) { ^-inf };
        if (denominator == inf) { ^this.class.new(0, 1) };
        if (denominator == -inf) { ^1 / -inf };
        if (numerator.isNaN || denominator.isNaN) { ^0/0 };

        // at least one Rational
        if (this.numerator.isKindOf(Rational) || this.denominator.isKindOf(Rational)) {
            ^(numerator.asRational / denominator.asRational)
        };

        // Ints or rounded Floats
        ^if (this.numerator.frac == 0 && this.denominator.frac == 0) {

            // check for bad terms
            if (denominator == 0) {
                "Rational has zero denominator".error
            };

            d = this.factor;
            numerator = ((this.numerator / d).abs * d.sign).round.asFloat;
            denominator = (this.denominator / d).abs.round.asFloat;
            Rational.fromReducedTerms(numerator, denominator)
        } {
            // other Number cases
            (this.numerator / this.denominator).asRational
        }
    }

    *fromReducedTerms { arg numerator=1.0, denominator=1.0;
        ^super.newCopyArgs(numerator, denominator);
    }

    *newFrom { arg that; ^that.asRational }

    factor {
        var d = gcd(this.numerator.asInteger, this.denominator.asInteger).abs;
        if (denominator < 0) { d = d.neg };
        if (numerator   < 0) { d = d.neg };
        ^d.asFloat
    }

    sign {
        if (this.numerator == 0 ) { ^0    };
        if (this.numerator >  0 ) { ^1    };
        if (this.numerator <  0 ) { ^(-1) };
    }

    numerator_ { arg newNumerator=1.0; numerator = newNumerator; ^this.reduce }

    denominator_ { arg newDenominator=1.0; denominator = newDenominator; ^this.reduce }

    isNumeratorPowerOfTwo { ^this.numerator.asInteger.isPowerOfTwo }

    isDenominatorPowerOfTwo { ^this.denominator.asInteger.isPowerOfTwo }

    performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
        ^aNumber.asRational.perform(aSelector, this, adverb)
    }

    isRational { ^true }

    %/ { arg aNumber; ^this.class.new(this, aNumber) }

    asRational { ^this }

    asFloat { ^(this.numerator / this.denominator).asFloat }

    asInteger { ^(this.numerator / this.denominator).asInteger }

    asInt { ^this.asInteger }

    reciprocal { ^this.class.new(this.denominator, this.numerator) }

    printOn { arg stream;
        stream <<  numerator.asString.replace(".0","") << "%/" << denominator.asString.replace(".0","")
    }

    storeOn { arg stream;
        stream <<  numerator.asString.replace(".0","") << "%/" << denominator.asString.replace(".0","")
    }

    isNaN { ^false }

    hash { ^this.instVarHash }

    + { arg aNumber, adverb;
        ^this.class.new(
            (this.numerator * aNumber.denominator) + (this.denominator * aNumber.numerator),
            this.denominator * aNumber.denominator)
    }

    - { arg aNumber, adverb;
        ^this.class.new(
            (this.numerator * aNumber.denominator) - (this.denominator * aNumber.numerator),
            this.denominator * aNumber.denominator)
    }

    * { arg aNumber, adverb;
        ^this.class.new(
            this.numerator * aNumber.numerator,
            this.denominator * aNumber.denominator)
    }

    / { arg aNumber, adverb;
        ^this.class.new(
            this.numerator * aNumber.denominator,
            this.denominator * aNumber.numerator)
    }

    == { arg aNumber, adverb;
        ^(this.numerator * aNumber.denominator) == (this.denominator * aNumber.numerator)
    }

    != { arg aNumber, adverb;
        ^(this.numerator * aNumber.denominator) != (this.denominator * aNumber.numerator)
    }

    pow { arg n;

        ^case
        { n == 0 } { this.class.new(1,1) }

        { n > 0 } {
            this.class.new(
                this.numerator.pow(n),
                this.denominator.pow(n)
            )
        }

        { n < 0  } {
            if((this.numerator == 0).not) {
                this.reciprocal.pow(n.abs)
            } {
                "Rational has zero denominator.".error
            }
        }
    }

    simplify { arg maxDenominator=20, fasterBetter=false;
        ^this.asFloat.asRational(maxDenominator,fasterBetter)
    }

    < { arg aNumber;
        ^(this.numerator * aNumber.denominator) < (this.denominator * aNumber.numerator)
    }

    > { arg aNumber;
        ^(this.numerator * aNumber.denominator) > (this.denominator * aNumber.numerator)
    }

    <= { arg aNumber;
        ^(this.numerator * aNumber.denominator) <= (this.denominator * aNumber.numerator)
    }

    >= { arg aNumber;
        ^(this.numerator * aNumber.denominator) >= (this.denominator * aNumber.numerator)
    }

    isNegative { ^this.numerator.isNegative }

    isPositive { ^this.numerator.isPositive }

    neg { ^this * -1 }

    squared { ^this.pow(2) }

    cubed { ^this.pow(3) }

    abs { ^this.class.new(numerator.abs, denominator) }

}


