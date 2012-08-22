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
		^real.hash << 1 bitXor: imag.hash
	}

	// double dispatch
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^aNumber.asComplex.perform(aSelector, this, adverb)
	}
	performBinaryOpOnSignal { arg aSelector, aSignal, adverb;
		^aSignal.asComplex.perform(aSelector, this)
	}
	performBinaryOpOnComplex { arg aSelector, aNumber, adverb;
		BinaryOpFailureError(this, aSelector, [aNumber, adverb]).throw;
	}
	performBinaryOpOnUGen { arg aSelector, aUGen, adverb;
		^aUGen.asComplex.perform(aSelector, this, adverb)
	}

	neg { ^Complex.new(real.neg, imag.neg) }
	conjugate { ^Complex.new(real, imag.neg) }
	
	squared { ^this * this }
	cubed { ^this * this * this }
	exp { ^exp(real) * Complex.new(cos(imag), sin(imag)) }
	
	pow { arg aNumber; // return(this ** aNumber)
		
		// Notation below:
		// t=this, p=power, i=sqrt(-1)
		// Derivation:
		// t ** p = exp(p*log(t)) = ... = r*exp(i*a)
		
		var p_real, p_imag, t_mag, t_phase, t_maglog;
		var mag, phase;
		
		aNumber = aNumber.asComplex;
		
		p_real = aNumber.real;
		p_imag = aNumber.imag;
		if(p_real == 0.0 and: { p_imag == 0 }) { ^Complex(1.0, 0.0) };
		if(p_imag == 0.0 and: { imag == 0.0 } and: { real > 0.0 }) {
			^Complex(real ** p_real, 0.0)
		};
		
		t_mag = this.magnitude;
		if(t_mag == 0.0) { ^Complex(0.0, 0.0) };
		t_maglog = 0.5 * log(t_mag);
		t_phase = this.phase;
		
		mag = exp((p_real * t_maglog) - (p_imag * t_phase));
		phase = (p_imag * t_maglog) + (p_real * t_phase);
		
		^Complex(mag * cos(phase), mag * sin(phase))
	}
	 	
	magnitude { ^hypot(real, imag) }
	abs { ^hypot(real, imag) }
	rho { ^hypot(real, imag) }

	magnitudeApx { ^hypotApx(real, imag) }

	angle { ^atan2(imag, real) }
	phase { ^atan2(imag, real) }
	theta { ^atan2(imag, real) }

	coerce { arg aNumber; ^aNumber.asComplex }
	round { arg aNumber = 1.0;
		^Complex(real.round(aNumber), imag.round(aNumber))
	}

	asInteger { ^real.asInteger }
	asFloat { ^real.asFloat }

	asComplex { ^this }
	asPolar { ^Polar.new(this.rho, this.theta) }
	asPoint { ^Point.new(this.real, this.imag) }

	printOn { arg stream;
		stream << "Complex( " << real << ", " << imag << " )";
	}
}
