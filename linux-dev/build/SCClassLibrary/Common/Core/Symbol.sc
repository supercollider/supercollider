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
	//asCompileString { ^super.asString }
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

	openTextFile { arg selectionStart=0, selectionLength=0;
		^this.asString.openTextFile(selectionStart, selectionLength)
	}
	
	// Environment support
	// The compiler translates use of an Environment variable like ~myvar 
	// to a call to one of these methods, for example:
	// 			~myvar = 5;  translates to:  'myvar'.envirPut(5);
	// the implementations have been replaced by primitives
	envirGet { 
		_Symbol_envirGet
		//^currentEnvironment.at(this) 
	}
	envirPut { arg aValue;
		_Symbol_envirPut
		//^currentEnvironment.put(this, aValue) 
	}

	draw { 
		_Symbol_Draw
		^this.primitiveFailed
	}
	fontID {
		_Symbol_GetFontID
		^this.primitiveFailed
	}
	blend { // Envelopes may call this on the curves inst var.
		^this
	}

	// support for math on symbols

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
	nyqring { ^this }
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
	+ { arg x; ^this }		
	- { arg x; ^this } 
	* { arg x; ^this } 
	/ { arg x; ^this } 
	mod { arg x; ^this }
	min { arg x; ^this } 
	max { arg x; ^this }
	bitAnd { arg x; ^this }
	bitOr { arg x; ^this }
	bitXor { arg x; ^this }	
	lcm { arg x; ^this }
	gcd { arg x; ^this }
	round { arg x; ^this }
	trunc { arg x; ^this }
	atan2 { arg x; ^this }	
	hypot { arg x; ^this }
	hypotApx { arg x; ^this }
	pow { arg x; ^this }
	leftShift { arg x; ^this }
	rightShift { arg x; ^this }
	unsignedRightShift { arg x; ^this }
	rrand { arg x; ^this }
	exprand { arg x; ^this }
	
	degreeToKey { ^this }
	
	doNumberOp { arg x; ^this }
	doComplexOp { arg x; ^this }
	doSignalOp { arg x; ^this }
	doListOp { arg aSelector, aList; 
		aList.collect({ arg item; 
			item.perform(aSelector, this)
		}) 
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
}
