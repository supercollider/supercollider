Integer : SimpleNumber {
	isInteger { ^true }	
	
	hash { ^this.asFloat.hash }
		
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
	// override 'do'
	generate { arg function; function.value(this) }

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
	asUnicode { ^this }
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
	grayCode { _BinaryGrayCode }
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
	
	// exit the program and return the result code to unix shell
	exit { _Exit }

	asStringToBase { | base=10, width=8 |
		var rest = this, string, mask;
		if (base.inclusivelyBetween(2, 36).not) {
			base = clip(base, 2, 36);
			warn(thisMethod + ": base not between 2 and 36");
		};
		string = String.newClear(width);
		if (base.isPowerOfTwo) {
			mask = base - 1;
			base = base.trailingZeroes;
			width.do { | i |
				string.put(width-i-1, (rest & mask).asDigit);
				rest = rest >> base;
			};
		} {
			width.do { | i |
				string.put(width-i-1, (rest mod: base).asDigit);
				rest = rest div: base;
			};
		};
		^string
	}

	asBinaryString { | width=8 |
		^this.asStringToBase(2, width)
	}

	asHexString { | width=8 |
		^this.asStringToBase(16, width)
	}

	asIPString {
	    ^((this >> 24) & 255).asString ++ "." ++
		((this >> 16) & 255).asString ++ "." ++
		((this >> 8) & 255).asString ++ "." ++
		(this & 255).asString
	}

	archiveAsCompileString { ^true }
	
	geom { arg start, grow;
		^Array.geom(this, start, grow);
	}
	fib { arg a=0.0, b=1.0;
		^Array.fib(this, a, b);
	}
	
	factors {
		var num, array, prime;
		num = this.abs;
		// there are 6542 16 bit primes from 2 to 65521
		6542.do {|i|
			prime = i.nthPrime;
			while { (num mod: prime) == 0 }{
				array = array.add(prime);
				num = num div: prime;
				if (num == 1) {^array}
			};
			if (prime.squared > num) {
				array = array.add(num);
				^array
			};
		};
		// because Integer is 32 bit, and we have tested all 16 bit primes,
		// any remaining number must be a prime.
		array = array.add(num);
		^array
	}
	
}

