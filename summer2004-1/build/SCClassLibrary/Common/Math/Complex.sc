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
	
	magnitude { ^hypot(real, imag) }
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
