Symbol {
	*new { ^this.shouldNotImplement(thisMethod) }

	// conversion
	asSymbol { ^this }
	asInteger {
		_Symbol_AsInteger
		^this.primitiveFailed
	}
	asFloat {
		_Symbol_AsFloat
		^this.primitiveFailed
	}
	ascii { ^this.asString.ascii }

	// the primitive fails to escape '
	asCompileString { ^("'" ++ super.asString.escapeChar($') ++ "'") }
	asClass {
		_SymbolClass
		// if Symbol represents a class name then return the class, else return nil.
	}
	asSetter {
		_SymbolAsSetter
		^this.primitiveFailed
	}
	asGetter {
		_SymbolAsGetter
		^this.primitiveFailed
	}
	asSpec { ^Spec.specs.at(this) }
	asWarp { arg spec; ^Warp.warps.at(this).new(spec) }
	asTuning { ^TuningInfo.at(this) }
	asScale { ^ScaleInfo.at(this) }
	// testing
	isSetter {
		// returns true if last character of symbol is an underscore
		_SymbolIsSetter
		^this.primitiveFailed
	}
	isClassName {
		_SymbolIsClassName
		// returns true if first character of symbol is a capital letter
	}
	isMetaClassName {
		_SymbolIsMetaClassName
		// returns true if there is a meta class by this name
	}
	isPrefix { | other |
		_SymbolIsPrefix
	}
	isPrimitiveName {
		// returns true if symbol is a valid primitive name
		^this.isPrefix(\_)
	}
	isPrimitive {
		// returns true if symbol names a bound primitive
		^this.isPrimitiveName and: { this.primitiveIndex > 0 }
	}
	isMap {
		_Symbol_IsMap
		// returns true if symbol starts with 'a' or 'c' followed by a number
	}
	isRest { ^this.isMap.not }

	// Environment support
	// The compiler translates use of an Environment variable like ~myvar
	// to a call to one of these methods, for example:
	// 			~myvar = 5;  translates to:  'myvar'.envirPut(5);
	// the implementations have been replaced by primitives
	envirGet {
		_Symbol_envirGet
		^currentEnvironment.at(this)
	}
	envirPut { arg aValue;
		_Symbol_envirPut
		currentEnvironment.put(this, aValue);
		^aValue
	}

	blend { // Envelopes may call this on the curves inst var.
		^this
	}

	++ { arg aString; ^this.asString ++ aString }

	asBinOpString {
		var res;
		res = this.asString;
		^if(res[0].isAlphaNum) { res ++ ":" } { res }
	}

	applyTo { arg firstArg ... args;
		^firstArg.performList(this, args)
	}

	// support for math on symbols

	performBinaryOpOnSomething { ^this }

	// unary ops
	neg { ^this }
	bitNot { ^this }
	abs { ^this }
	ceil { ^this }
	floor { ^this }
	frac { ^this }
	sign { ^this }
	sqrt { ^this }
	exp { ^this }
	midicps { ^this }
	cpsmidi { ^this }
	midiratio { ^this }
	ratiomidi { ^this }
	ampdb { ^this }
	dbamp { ^this }
	octcps { ^this }
	cpsoct { ^this }
	log { ^this }
	log2 { ^this }
	log10 { ^this }
	sin { ^this }
	cos { ^this }
	tan { ^this }
	asin { ^this }
	acos { ^this }
	atan { ^this }
	sinh { ^this }
	cosh { ^this }
	tanh { ^this }
	rand { ^this }
	rand2 { ^this }
	linrand { ^this }
	bilinrand { ^this }
	sum3rand { ^this }

	distort { ^this }
	softclip { ^this }
	coin { ^this }
	even { ^this }
	odd { ^this }

	rectWindow { ^this }
	hanWindow { ^this }
	welWindow { ^this }
	triWindow { ^this }

	scurve { ^this }
	ramp { ^this }

	// binary ops
	+ { ^this }
	- { ^this }
	* { ^this }
	/ { ^this }
	mod { ^this }
	min { ^this }
	max { ^this }
	bitAnd { ^this }
	bitOr { ^this }
	bitXor { ^this }
	bitHammingDistance { ^this }
	hammingDistance { |that| ^this.asString.hammingDistance(that.asString) }
	lcm { ^this }
	gcd { ^this }
	round { ^this }
	roundUp { ^this }
	trunc { ^this }
	atan2 { ^this }
	hypot { ^this }
	hypotApx { ^this }
	pow { ^this }
	leftShift { ^this }
	rightShift { ^this }
	unsignedRightShift { ^this }
	rrand { ^this }
	exprand { ^this }

	< { arg aNumber; _LT; ^this }
	> { arg aNumber; _GT; ^this }
	<= { arg aNumber; _LE; ^this }
	>= { arg aNumber; _GE; ^this }

	degreeToKey { ^this }

	degrad { ^this }
	raddeg { ^this }

	doNumberOp { ^this }
	doComplexOp { ^this }
	doSignalOp { ^this }
	doListOp { arg aSelector, aList;
		aList.collect({ arg item;
			item.perform(aSelector, this)
		})
	}

	primitiveIndex {
		_Symbol_PrimitiveIndex
		^this.primitiveFailed
	}
	specialIndex {
		// used by BasicOpUGens to get an ID number for the operator
		_Symbol_SpecialIndex
		^this.primitiveFailed
	}

	printOn { arg stream;
		stream.putAll(this.asString);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}

	// code gen
	codegen_UGenCtorArg { arg stream;
		this.asString.codegen_UGenCtorArg(stream);
	}

	archiveAsCompileString { ^true }

	kr { | val, lag, fixedLag = false |
		^NamedControl.kr(this, val, lag, fixedLag)
	}

	ir { | val |
		^NamedControl.ir(this, val)
	}

	tr { | val |
		^NamedControl.tr(this, val)
	}

	ar { | val, lag |
		^NamedControl.ar(this, val, lag)
	}

	matchOSCAddressPattern { arg addressPattern;
		_Symbol_matchOSCPattern
		^this.primitiveFailed
	}

	help {
		this.asString.help
	}

}
