Complex : Number {
	var <>real, <>imag;	

	*new { arg real, imag;
		^super.newCopyArgs(real, imag);
	}
		
	+ { arg aNumber;
		if ( aNumber.isNumber, {
			^Complex.new(real + aNumber.real, imag + aNumber.imag)
		},{
			^aNumber.performBinaryOpOnComplex('+', this)
		});
	}
	- { arg aNumber; 
		if ( aNumber.isNumber, {
			^Complex.new(real - aNumber.real, imag - aNumber.imag)
		},{
			^aNumber.performBinaryOpOnComplex('-', this)
		});
	}
	* { arg aNumber; 
		if ( aNumber.isNumber, {
			^Complex.new(
				// these are implemented as additional message sends so that UGens can
				// optimize the 6 operations down to 2 UGen instances.
				(real * aNumber.real) - (imag * aNumber.imag),
				(real * aNumber.imag) + (imag * aNumber.real)
			)
		},{
			^aNumber.performBinaryOpOnComplex('*', this)
		});
	}
	/ { arg aNumber; 
		var denom, yr, yi;
		if ( aNumber.isNumber, {
			yr = aNumber.real; 
			yi = aNumber.imag;
			denom = 1.0 / (yr * yr + (yi * yi));
			^Complex.new(
				((real * yr) + (imag * yi)) * denom, 
				((imag * yr) - (real * yi)) * denom)
		},{
			^aNumber.performBinaryOpOnComplex('/', this)
		});
	}

	< { arg aNumber; 
		if ( aNumber.isNumber, {
			^real < aNumber.real
		},{
			^aNumber.performBinaryOpOnComplex('<', this)
		});
	}

	== { arg aNumber; 
		if ( aNumber.isNumber, {
			^real == aNumber.real and: { imag == aNumber.imag }
		},{
			^aNumber.performBinaryOpOnComplex('==', this)
		});
	}
	hash {
		^real.hash bitXor: imag.hash
	}
	
	// double dispatch
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; 
		^aNumber.asComplex.perform(aSelector, this) 
	}
	performBinaryOpOnSignal { arg aSelector, aSignal; ^aSignal.asComplex.perform(aSelector, this) }
	performBinaryOpOnComplex { arg aSelector, aNumber; ^error("Math operation failed.\n") }
	performBinaryOpOnUGen { arg aSelector, aUGen;
		^aUGen.asComplex.perform(aSelector, this)
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
	storeOn { arg stream;
		stream << "Complex( " <<< real << ", " <<< imag << " )";
	}
}
