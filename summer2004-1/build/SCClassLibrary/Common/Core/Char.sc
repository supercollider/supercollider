Char : Magnitude {
	*new { ^this.shouldNotImplement(thisMethod) }
	// to create a Char use the Integer methods asAscii or asDigit

	hash { _ObjectHash; ^this.primitiveFailed }
	
	ascii {
		// returns the ascii value of a character as an Integer
		_AsciiValue
	}
	digit {
		// returns the digit value of a character as an Integer
		_DigitValue
		^this.primitiveFailed
	}
	asAscii { ^this }
	
	// case conversion
	toUpper {
		_ToUpper
	}
	toLower {
		_ToLower
	}
	
	// tests return Boolean:
	isAlpha {
		// is an alphabetic character
		_IsAlpha
	}
	isAlphaNum {
		// is an alphabetic character or decimal digit
		_IsAlphaNum
	}
	isPrint {
		// is printable
		_IsPrint
	}
	isPunct {
		// is punctuation
		_IsPunct
	}
	isControl {
		// is a control character
		_IsControl
	}
	isSpace {
		_IsSpace
		// is white space
	}
	isVowel {	
		^"AEIOU".includes(this.toUpper);
	}
	isDecDigit {
		// is a decimal digit 0-9
		_IsDecDigit
	}
	isFileSafe {
		if(this.isPrint.not,{ ^false });
		^this.ascii != 47 and: {this.ascii != 58} and: {this.ascii != 34}
	}
	< { arg aChar;
		^this.ascii < aChar.ascii
	}
	== { arg aChar;  ^this.ascii == aChar.ascii }
	
	++ { |that| ^this.asString ++ that }

	*nl { ^$\n }
	*ff { ^$\f }
	*tab { ^$\t }
	*space { ^$  }
	*comma { ^$\, }

	printOn { arg stream;
		stream.put(this);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}
}

