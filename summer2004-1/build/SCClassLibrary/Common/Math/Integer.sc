Integer : SimpleNumber {
	isInteger { ^true }	
		
	+ { arg aNumber, adverb; _AddInt; ^aNumber.performBinaryOpOnSimpleNumber('+', this, adverb) }
	- { arg aNumber, adverb; _SubInt; ^aNumber.performBinaryOpOnSimpleNumber('-', this, adverb) }
	* { arg aNumber, adverb; _MulInt; ^aNumber.performBinaryOpOnSimpleNumber('*', this, adverb) }
	
	clip { arg lo, hi; _ClipInt; ^this.primitiveFailed }
	wrap { arg lo, hi; _WrapInt; ^this.primitiveFailed }
	fold { arg lo, hi; _FoldInt; ^this.primitiveFailed }

	even { ^(this & 1) == 0 }
	odd { ^(this & 1) == 1 }
	
	xrand { arg exclude=0; 
		^(exclude + (this - 1).rand + 1) % this;
	}
	xrand2 { arg exclude=0; 
		var res;
		res = (2 * this).rand - this;
		if (res == exclude, { ^this },{ ^res });
	}
	
	// iteration
	do { arg function;
		// iterates function from 0 to this-1 
		// special byte codes inserted by compiler for this method
		var i = 0;
		while ({ i < this }, { function.value(i, i); i = i + 1; });
	}

	reverseDo { arg function;
		// iterates function from 0 to this-1 
		// special byte codes inserted by compiler for this method
		var i=0, j=0;
		i = this - 1;
		while ({ i >= 0 }, { function.value(i, j); i = i - 1; j = j + 1; });
	}
	for { arg endval, function;
		// iterates function from this to endval (inclusive)
		// special byte codes inserted by compiler for this method
		var i, j=0, stepval=1;
		i = this;
		while ({ i <= endval }, { function.value(i, j); i = i + 1; j = j + 1; });
	}
	forBy { arg endval, stepval, function;
		// iterates function from this to endval (inclusive) by step
		// special byte codes inserted by compiler for this method
		var i, j=0;
		i = this;
		while ({ i <= endval }, { function.value(i, j); i = i + stepval; j = j + 1; });
	}

	to { arg hi, step=1;
		^Interval.new(this, hi, step)
	}
		
	// conversions to Char
	asAscii {
		// must be 0 <= this <= 255
		_AsAscii
		^this.primitiveFailed
	}
	asDigit {
		// must be 0 <= this <= 35
		_AsDigit
		^this.primitiveFailed
	}
	asBinaryDigits { arg numDigits = 8;
		var array;
		array = Array.new(numDigits);
		numDigits.do({ arg i;
			array.addFirst(this >> i & 1) 
		});
		^array
	}
	
	nextPowerOfTwo { _NextPowerOfTwo }
	isPowerOfTwo { _IsPowerOfTwo }
	leadingZeroes { _CLZ }
	trailingZeroes { _CTZ }
	numBits { _NumBits }
	log2Ceil { _Log2Ceil }
	setBit { arg bitNumber, bool = true; _SetBit ^this.primitiveFailed }

	nthPrime { _NthPrime }
	prevPrime { _PrevPrime }
	nextPrime { _NextPrime }
	
	
	isPrime {
		_IsPrime
		/*
		var sqrtThis;
		if ( this <= 2, {
			if (this == 2, { ^true }, { ^false });
		});
		sqrtThis = this.sqrt.asInteger;
		256.do({ arg i; var p;
			p = i.nthPrime;
			if (this % p == 0, { ^false });
			if (p >= sqrtThis, { ^true });
		});
		^nil
		*/
	}

	// code gen
	outputType { ^\INT }
	
	// exit the program and return the result code to unix shell
	exit { _Exit }

	
	asHexString { | width=8 |
		var x, string;
		x = this;
		string = String.newClear(width);
		width.do { |i|
			string.put(width-i-1, (x & 15).asDigit);
			x = x >> 4;
		};
		^string
	}

	asIPString {
	    ^((this >> 24) & 255).asString ++ "." ++
		((this >> 16) & 255).asString ++ "." ++
		((this >> 8) & 255).asString ++ "." ++
		(this & 255).asString
	}
}

