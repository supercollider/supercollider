Char : Magnitude {

	const <nl = $\n ;
	const <ret = $\r ;
	const <vtab = $\v ;
	const <ff = $\f ;
	const <tab = $\t ;
	const <space = $  ;
	const <comma = $, ;
	const <bullet = $* ;
	const <binaryOpCharacters = "!@%&*-+=|<>?/";

	*new { ^this.shouldNotImplement(thisMethod) }
	// to create a Char use the Integer methods asAscii or asDigit

	hash { _ObjectHash; ^this.primitiveFailed }

	ascii {
		// returns the ascii value of a character as an Integer
		_AsciiValue
		^this.primitiveFailed
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
		^this.primitiveFailed
	}
	toLower {
		_ToLower
		^this.primitiveFailed
	}

	// tests return Boolean:
	isAlpha {
		// is an alphabetic character
		_IsAlpha
		^this.primitiveFailed
	}
	isAlphaNum {
		// is an alphabetic character or decimal digit
		_IsAlphaNum
		^this.primitiveFailed
	}
	isPrint {
		// is printable
		_IsPrint
		^this.primitiveFailed
	}
	isPunct {
		// is punctuation
		_IsPunct
		^this.primitiveFailed
	}
	isControl {
		// is a control character
		_IsControl
		^this.primitiveFailed
	}
	isSpace {
		// is white space
		_IsSpace
		^this.primitiveFailed
	}
	isVowel {
		^"AEIOU".includes(this.toUpper);
	}
	isDecDigit {
		// is a decimal digit 0-9
		_IsDecDigit
		^this.primitiveFailed
	}
	isUpper {
		// is upper case alphabetic character
		_IsUpper
		^this.primitiveFailed
	}
	isLower {
		// is lower case alphabetic character
		_IsLower
		^this.primitiveFailed
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

	printOn { arg stream;
		stream.put(this);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}

	archiveAsCompileString { ^true }
}
