Char : Magnitude {

	const <nl = $\n ;
	const <ff = $\f ;
	const <tab = $\t ;
	const <space = $  ;
	const <comma = $\, ;


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
	// this returns the single unicode value.
	// ascii is a subset of unicode
	asUnicode { ^this.ascii }
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
	isUpper {
		// is upper case alphabetic character
		_IsUpper
	}
	isLower {
		// is lower case alphabetic character
		_IsLower
	}
	isFileSafe {
		if(this.isPrint.not,{ ^false });
		^this.ascii != 47 and: {this.ascii != 58} and: {this.ascii != 34}
	}
	isPathSeparator {
		^thisProcess.platform.isPathSeparator(this)
	}
	< { arg aChar;
		^this.ascii < aChar.ascii
	}
	== { arg aChar;  ^aChar respondsTo: \ascii and: { this.ascii == aChar.ascii } }

	++ { |that| ^this.asString ++ that }

	*bullet { ^if(Platform.ideName=="scapp",165.asAscii,$*) }

	printOn { arg stream;
		stream.put(this);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}

	archiveAsCompileString { ^true }
}
