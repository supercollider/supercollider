Complex : Number {
	var <>real, <>imag;

	*new { arg real, imag;
		^super.newCopyArgs(real, imag);
	}

	+ { arg aNumber, adverb;
		if ( aNumber.isNumber, {
			^Complex.new(real + aNumber.real, imag + aNumber.imag)
		},{
			^aNumber.performBinaryOpOnComplex('+', this, adverb)
		});
	}
	- { arg aNumber, adverb;
		if ( aNumber.isNumber, {
			^Complex.new(real - aNumber.real, imag - aNumber.imag)
		},{
			^aNumber.performBinaryOpOnComplex('-', this, adverb)
		});
	}
	* { arg aNumber, adverb;
		if ( aNumber.isNumber, {
			^Complex.new(
				// these are implemented as additional message sends so that UGens can
				// optimize the 6 operations down to 2 UGen instances.
				(real * aNumber.real) - (imag * aNumber.imag),
				(real * aNumber.imag) + (imag * aNumber.real)
			)
		},{
			^aNumber.performBinaryOpOnComplex('*', this, adverb)
		});
	}
	/ { arg aNumber, adverb;
		var denom, yr, yi;
		if ( aNumber.isNumber, {
			yr = aNumber.real;
			yi = aNumber.imag;
			denom = 1.0 / (yr * yr + (yi * yi));
			^Complex.new(
				((real * yr) + (imag * yi)) * denom,
				((imag * yr) - (real * yi)) * denom)
		},{
			^aNumber.performBinaryOpOnComplex('/', this, adverb)
		});
	}

	< { arg aNumber, adverb;
		if ( aNumber.isNumber, {
			^real < aNumber.real
		},{
			^aNumber.performBinaryOpOnComplex('<', this, adverb)
		});
	}

	== { arg aNumber, adverb;
		if ( aNumber.isNumber, {
			^real == aNumber.real and: { imag == aNumber.imag }
		},{
			^aNumber.performBinaryOpOnComplex('==', this, adverb)
		});
	}
	hash {
		^real.hash bitXor: imag.hash
	}

	// double dispatch
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^aNumber.asComplex.perform(aSelector, this, adverb)
	}
	performBinaryOpOnSignal { arg aSelector, aSignal, adverb;
		^aSignal.asComplex.perform(aSelector, this)
	}
	performBinaryOpOnComplex { arg aSelector, aNumber, adverb;
		^error("Math operation failed.\n")
	}
	performBinaryOpOnUGen { arg aSelector, aUGen, adverb;
		^aUGen.asComplex.perform(aSelector, this, adverb)
	}

	neg { ^Complex.new(real.neg, imag.neg) }
	conjugate { ^Complex.new(real, imag.neg) }
	
	squared { ^this * this }
	cubed { ^this * this * this }
	exp { ^exp(real) * Complex.new(cos(imag), sin(imag)) }
	
	pow { arg aNumber;
		var res;
		aNumber = aNumber.asComplex;
		// complex to the power of a positive integer
		if(aNumber.imag == 0 and: { aNumber.real >= 0 } and: { aNumber.real.frac == 0 }) {
				res = Complex.new(1.0, 0.0);
				(aNumber.real).do { res = res * this };
				^res
		};
		// a real positive number to the power of a complex number
		if(imag == 0.0 and: { real >= 0 }) { 
			^exp(aNumber * log(real)) 
		} {
		// stop here for simplicity. Some solutions are ambiguous
		Error("pow not implemented or undefined for this combination of numbers").throw
		}
	}
	
	magnitude { ^hypot(real, imag) }
	abs { ^hypot(real, imag) }
	rho { ^hypot(real, imag) }

	magnitudeApx { ^hypotApx(real, imag) }

	angle { ^atan2(imag, real) }
	phase { ^atan2(imag, real) }
	theta { ^atan2(imag, real) }

	coerce { arg aNumber; ^aNumber.asComplex }

	asInteger { ^real.asInteger }
	asFloat { ^real.asFloat }

	asComplex { ^this }
	asPolar { ^Polar.new(this.rho, this.theta) }
	asPoint { ^Point.new(this.real, this.imag) }

	printOn { arg stream;
		stream << "Complex( " << real << ", " << imag << " )";
	}
}
