
Float : SimpleNumber {
	isFloat { ^true }	
	asFloat { ^this }	

	+ { arg aNumber, adverb; _AddFloat; ^aNumber.performBinaryOpOnSimpleNumber('+', this, adverb) }
	- { arg aNumber, adverb; _SubFloat; ^aNumber.performBinaryOpOnSimpleNumber('-', this, adverb) }
	* { arg aNumber, adverb; _MulFloat; ^aNumber.performBinaryOpOnSimpleNumber('*', this, adverb) }
	
	clip { arg lo, hi; _ClipFloat; ^this.primitiveFailed }
	wrap { arg lo, hi; _WrapFloat; ^this.primitiveFailed }
	fold { arg lo, hi; _FoldFloat; ^this.primitiveFailed }
	
	coin { ^1.0.rand < this }
	xrand2 { ^this.rand2 }
	

	// returns an Integer which is the bit pattern of this as a 
	// 32bit single precision float
	as32Bits { _As32Bits } 

	// returns an Integer which is the bit pattern of high
	// 32 bits of the 64 bit double precision floating point value
	high32Bits { _High32Bits } 
	low32Bits { _Low32Bits } 
	
	*from32Bits { arg word;
		_From32Bits
		^this.primitiveFailed
	}
	*from64Bits { arg hiWord, loWord;
		_From64Bits
		^this.primitiveFailed
	}
	
	// iteration
	do { arg function;
		// iterates function from 0 to this-1 
		// special byte codes inserted by compiler for this method
		var i = 0.0;
		while ({ (i + 0.5) < this }, { function.value(i, i); i = i + 1.0; });
	}

	reverseDo { arg function;
		// iterates function from this-1 to 0
		// special byte codes inserted by compiler for this method
		var i, j=0.0;
		i = this - 1.0;
		while ({ (i + 0.5) >= 0.0 }, { function.value(i, j); i = i - 1.0; j = j + 1.0; });
	}
	
	asStringPrec { arg precision;
		_Float_AsStringPrec
		^this.primitiveFailed
	}

	archiveAsCompileString { ^true }
}
