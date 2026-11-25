AbstractFunction {

	// function compositions
	// override these in subclasses to perform different kinds of function compositions
	composeUnaryOp { |aSelector|
		^UnaryOpFunction.new(aSelector, this)
	}
	composeBinaryOp { |aSelector, something, adverb|
		^BinaryOpFunction.new(aSelector, this, something, adverb)
	}
	reverseComposeBinaryOp { |aSelector, something, adverb|
		^BinaryOpFunction.new(aSelector, something, this, adverb)
	}
	composeNAryOp { |aSelector, anArgList|
		^NAryOpFunction.new(aSelector, this, anArgList)
	}

	// double dispatch for mixed operations
	performBinaryOpOnSimpleNumber { |aSelector, aNumber, adverb|
		^this.reverseComposeBinaryOp(aSelector, aNumber, adverb)
	}
	performBinaryOpOnSignal { |aSelector, aSignal, adverb|
		^this.reverseComposeBinaryOp(aSelector, aSignal, adverb)
	}
	performBinaryOpOnComplex { |aSelector, aComplex, adverb|
		^this.reverseComposeBinaryOp(aSelector, aComplex, adverb)
	}
	performBinaryOpOnSeqColl { |aSelector, aSeqColl, adverb|
		^this.reverseComposeBinaryOp(aSelector, aSeqColl, adverb)
	}

	// respond to math operators
	neg { ^this.composeUnaryOp('neg') }
	reciprocal { ^this.composeUnaryOp('reciprocal') }
	bitNot { ^this.composeUnaryOp('bitNot') }
	abs { ^this.composeUnaryOp('abs') }
	asFloat { ^this.composeUnaryOp('asFloat') }
	asInteger { ^this.composeUnaryOp('asInteger') }
	ceil { ^this.composeUnaryOp('ceil') }
	floor { ^this.composeUnaryOp('floor') }
	frac { ^this.composeUnaryOp('frac') }
	sign { ^this.composeUnaryOp('sign') }
	squared { ^this.composeUnaryOp('squared') }
	cubed { ^this.composeUnaryOp('cubed') }
	sqrt { ^this.composeUnaryOp('sqrt') }
	exp { ^this.composeUnaryOp('exp') }
	midicps { ^this.composeUnaryOp('midicps') }
	cpsmidi { ^this.composeUnaryOp('cpsmidi') }
	midiratio { ^this.composeUnaryOp('midiratio') }
	ratiomidi { ^this.composeUnaryOp('ratiomidi') }
	ampdb { ^this.composeUnaryOp('ampdb') }
	dbamp { ^this.composeUnaryOp('dbamp') }
	octcps { ^this.composeUnaryOp('octcps') }
	cpsoct { ^this.composeUnaryOp('cpsoct') }
	log { ^this.composeUnaryOp('log') }
	log2 { ^this.composeUnaryOp('log2') }
	log10 { ^this.composeUnaryOp('log10') }
	sin { ^this.composeUnaryOp('sin') }
	cos { ^this.composeUnaryOp('cos') }
	tan { ^this.composeUnaryOp('tan') }
	asin { ^this.composeUnaryOp('asin') }
	acos { ^this.composeUnaryOp('acos') }
	atan { ^this.composeUnaryOp('atan') }
	sinh { ^this.composeUnaryOp('sinh') }
	cosh { ^this.composeUnaryOp('cosh') }
	tanh { ^this.composeUnaryOp('tanh') }
	rand { ^this.composeUnaryOp('rand') }
	rand2 { ^this.composeUnaryOp('rand2') }
	linrand { ^this.composeUnaryOp('linrand') }
	bilinrand { ^this.composeUnaryOp('bilinrand') }
	sum3rand { ^this.composeUnaryOp('sum3rand') }

	distort { ^this.composeUnaryOp('distort') }
	softclip { ^this.composeUnaryOp('softclip') }
	coin { ^this.composeUnaryOp('coin') }
	even { ^this.composeUnaryOp('even') }
	odd { ^this.composeUnaryOp('odd') }

	rectWindow { ^this.composeUnaryOp('rectWindow') }
	hanWindow { ^this.composeUnaryOp('hanWindow') }
	welWindow { ^this.composeUnaryOp('welWindow') }
	triWindow { ^this.composeUnaryOp('triWindow') }

	scurve { ^this.composeUnaryOp('scurve') }
	ramp { ^this.composeUnaryOp('ramp') }

	isPositive { ^this.composeUnaryOp('isPositive') }
	isNegative { ^this.composeUnaryOp('isNegative') }
	isStrictlyPositive { ^this.composeUnaryOp('isStrictlyPositive') }
	binaryValue { ^this.composeUnaryOp('binaryValue') }

	rho { ^this.composeUnaryOp('rho') }
	theta { ^this.composeUnaryOp('theta') }
	rotate { |function| ^this.composeBinaryOp('rotate', function) }
	dist { |function| ^this.composeBinaryOp('dist', function) }

	+ { |function, adverb| ^this.composeBinaryOp('+', function, adverb) }
	- { |function, adverb| ^this.composeBinaryOp('-', function, adverb) }
	* { |function, adverb| ^this.composeBinaryOp('*', function, adverb) }
	/ { |function, adverb| ^this.composeBinaryOp('/', function, adverb) }
	div { |function, adverb| ^this.composeBinaryOp('div', function, adverb) }
	mod { |function, adverb| ^this.composeBinaryOp('mod', function, adverb) }
	modSeaside { |function, adverb| ^this.composeBinaryOp('modSeaside', function, adverb) }
	pow { |function, adverb| ^this.composeBinaryOp('pow', function, adverb) }
	min { |function, adverb| ^this.composeBinaryOp('min', function, adverb) }
	max { |function = 0, adverb| ^this.composeBinaryOp('max', function, adverb) }

	<  { |function, adverb| ^this.composeBinaryOp('<', function, adverb) }
	<= { |function, adverb| ^this.composeBinaryOp('<=', function, adverb) }
	>  { |function, adverb| ^this.composeBinaryOp('>', function, adverb) }
	>= { |function, adverb| ^this.composeBinaryOp('>=', function, adverb) }

	|==| { |that|
		^this.composeBinaryOp('|==|', that)
	}
	prReverseLazyEquals { |that|
		// commutative, so it's OK to flip the operands
		^this.composeBinaryOp('|==|', that)
	}

	bitAnd { |function, adverb| ^this.composeBinaryOp('bitAnd', function, adverb) }
	bitOr { |function, adverb| ^this.composeBinaryOp('bitOr', function, adverb) }
	bitXor { |function, adverb| ^this.composeBinaryOp('bitXor', function, adverb) }
	bitHammingDistance { |function, adverb| ^this.composeBinaryOp('hammingDistance', function, adverb) }

	lcm { |function, adverb| ^this.composeBinaryOp('lcm', function, adverb) }
	gcd { |function, adverb| ^this.composeBinaryOp('gcd', function, adverb) }
	round { |function = 1, adverb| ^this.composeBinaryOp('round', function, adverb) }
	roundUp { |function = 1, adverb| ^this.composeBinaryOp('roundUp', function, adverb) }
	trunc { |function = 1, adverb| ^this.composeBinaryOp('trunc', function, adverb) }
	atan2 { |function, adverb| ^this.composeBinaryOp('atan2', function, adverb) }
	hypot { |function, adverb| ^this.composeBinaryOp('hypot', function, adverb) }
	hypotApx { |function, adverb| ^this.composeBinaryOp('hypotApx', function, adverb) }
	leftShift { |function, adverb| ^this.composeBinaryOp('leftShift', function, adverb) }
	rightShift { |function, adverb| ^this.composeBinaryOp('rightShift', function, adverb) }
	unsignedRightShift { |function, adverb| ^this.composeBinaryOp('unsignedRightShift', function, adverb) }
	ring1 { |function, adverb| ^this.composeBinaryOp('ring1', function, adverb) }
	ring2 { |function, adverb| ^this.composeBinaryOp('ring2', function, adverb) }
	ring3 { |function, adverb| ^this.composeBinaryOp('ring3', function, adverb) }
	ring4 { |function, adverb| ^this.composeBinaryOp('ring4', function, adverb) }
	difsqr { |function, adverb| ^this.composeBinaryOp('difsqr', function, adverb) }
	sumsqr { |function, adverb| ^this.composeBinaryOp('sumsqr', function, adverb) }
	sqrsum { |function, adverb| ^this.composeBinaryOp('sqrsum', function, adverb) }
	sqrdif { |function, adverb| ^this.composeBinaryOp('sqrdif', function, adverb) }
	absdif { |function, adverb| ^this.composeBinaryOp('absdif', function, adverb) }
	thresh { |function, adverb| ^this.composeBinaryOp('thresh', function, adverb) }
	amclip { |function, adverb| ^this.composeBinaryOp('amclip', function, adverb) }
	scaleneg { |function, adverb| ^this.composeBinaryOp('scaleneg', function, adverb) }
	clip2 { |function = 1, adverb| ^this.composeBinaryOp('clip2', function, adverb) }
	fold2 { |function = 1, adverb| ^this.composeBinaryOp('fold2', function, adverb) }
	wrap2 { |function = 1, adverb| ^this.composeBinaryOp('wrap2', function, adverb) }
	excess { |function = 1, adverb| ^this.composeBinaryOp('excess', function, adverb) }
	firstArg { |function, adverb| ^this.composeBinaryOp('firstArg', function, adverb) }
	rrand { |function, adverb| ^this.composeBinaryOp('rrand', function, adverb) }
	exprand { |function, adverb| ^this.composeBinaryOp('exprand', function, adverb) }
	@ { |function, adverb| ^this.composeBinaryOp('@', function, adverb) }

	// complex support
	real { ^this }
	imag { ^0.0 }


	|| { |function, adverb| ^this.composeBinaryOp('||', function, adverb) }
	&& { |function, adverb| ^this.composeBinaryOp('&&', function, adverb) }
	xor { |function, adverb| ^this.composeBinaryOp('xor', function, adverb) }
	nand { |function, adverb| ^this.composeBinaryOp('nand', function, adverb) }
	not { ^this.composeUnaryOp('not') }
	ref { ^this.composeUnaryOp('asRef') }

	// nary operators
	clip { |lo, hi| ^this.composeNAryOp('clip', [lo, hi]) }
	wrap { |lo, hi| ^this.composeNAryOp('wrap', [lo, hi]) }
	fold { |lo, hi| ^this.composeNAryOp('fold', [lo, hi]) }
	blend { |that, blendFrac = 0.5|
		^this.composeNAryOp('blend', [that, blendFrac])
	}
	linlin { |inMin, inMax, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('linlin', [inMin, inMax, outMin, outMax, clip])
	}
	linexp { |inMin, inMax, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('linexp', [inMin, inMax, outMin, outMax, clip])
	}
	explin { |inMin, inMax, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('explin', [inMin, inMax, outMin, outMax, clip])
	}
	expexp { |inMin, inMax, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('expexp', [inMin, inMax, outMin, outMax, clip])
	}
	lincurve { |inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax|
		^this.composeNAryOp('lincurve', [inMin, inMax, outMin, outMax, curve, clip])
	}
	curvelin { |inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax|
		^this.composeNAryOp('curvelin', [inMin, inMax, outMin, outMax, curve, clip])
	}
	bilin { |inCenter, inMin, inMax, outCenter, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('bilin', [inCenter, inMin, inMax, outCenter, outMin, outMax, clip])
	}
	biexp { |inCenter, inMin, inMax, outCenter, outMin, outMax, clip = \minmax|
		^this.composeNAryOp('biexp', [inCenter, inMin, inMax, outCenter, outMin, outMax, clip])
	}
	moddif { |function = 0.0, mod = 1.0|
		^this.composeNAryOp('moddif', [function, mod])
	}

	degreeToKey { |scale, stepsPerOctave = 12|
		^this.composeNAryOp('degreeToKey', [scale, stepsPerOctave])
	}

	degrad { ^this.composeUnaryOp('degrad') }
	raddeg { ^this.composeUnaryOp('raddeg') }

	applyTo { |... args|
		^this.valueArray(args)
	}

	<> { |that|
		// function composition
		^{ |...args| this.value(that.value(*args)) }
	}

	sampled{ |n = 80, from = 0.0, to = 1.0|
		var valueArray;
		valueArray = (from, (to - from) / (n - 1) .. to).collect { |x| this.value(x) };
		^{ |x| valueArray.blendAt(((x.clip(from, to) - from) / (to - from)) * (n - 1)) }
	}

	// embed in ugen graph
	asUGenInput { |for| ^this.value(for) }
	asAudioRateInput { |for|
		var result = this.value(for);
		^if(result.rate != \audio) { K2A.ar(result) } { result }
	}
	// convert to control input
	asControlInput { ^this.value }
	isValidUGenInput { ^true }
}


UnaryOpFunction : AbstractFunction {
	var selector, a;

	*new { |selector, a|
		^super.newCopyArgs(selector, a)
	}
	value { |... args, kwargs|
		^a.valueArgs(args, kwargs).perform(selector)
	}
	valueArray { |args|
		^a.valueArray(args).perform(selector)
	}
	valueEnvir { |... args|
		^a.valueArrayEnvir(args).perform(selector)
	}
	valueArrayEnvir { |... args|
		^a.valueArrayEnvir(args).perform(selector)
	}
	functionPerformList { |selector, arglist|
		^this.performList(selector, arglist)
	}
	storeOn { |stream|
		stream <<< a << "." << selector
	}


}

BinaryOpFunction : AbstractFunction {
	var selector, a, b, adverb;

	*new { |selector, a, b, adverb|
		^super.newCopyArgs(selector, a, b, adverb)
	}
	value { |... args, kwargs|
		^a.valueArgs(args, kwargs).perform(selector, b.valueArgs(args, kwargs), adverb)
	}
	valueArray { |args|
		^a.valueArray(args).perform(selector, b.valueArray(args), adverb)
	}
	valueEnvir { |... args|
		^a.valueArrayEnvir(args).perform(selector, b.valueArrayEnvir(args), adverb)
	}
	valueArrayEnvir { |... args|
		^a.valueArrayEnvir(args).perform(selector, b.valueArrayEnvir(args), adverb)
	}
	functionPerformList { |selector, arglist|
		^this.performList(selector, arglist)
	}
	storeOn { |stream|
		stream << "(" <<< a << " " << selector.asBinOpString;
		if(adverb.notNil) { stream << "." << adverb };
		stream << " " <<< b << ")"

	}
}

NAryOpFunction : AbstractFunction {
	var selector, a, arglist, kwarglist;

	*new { |selector, a, arglist, kwarglist|
		^super.newCopyArgs(selector, a, arglist, kwarglist)
	}
	value { |... args, kwargs|
		^a.valueArgs(args, kwargs).performArgs(
			selector,
			arglist.collect(_.valueArgs(args, kwargs)),
			kwarglist.collect(_.valueArgs(args, kwargs))
		)
	}
	valueArray { |args|
		^a.valueArray(args).performArgs(
			selector,
			arglist.collect(_.valueArray(args)),
			kwarglist.collect(_.valueArray(args))
		)
	}
	valueEnvir { |... args|
		^a.valueArrayEnvir(args).performArgs(
			selector,
			arglist.collect(_.valueArrayEnvir(args)),
			kwarglist.collect(_.valueArrayEnvir(args))
		)
	}
	valueArrayEnvir { |args|
		^a.valueArrayEnvir(args).performArgs(
			selector,
			arglist.collect(_.valueArrayEnvir(args)),
			kwarglist.collect(_.valueArrayEnvir(args))
		)
	}
	functionPerformList { |selector, arglist|
		^this.performArgs(selector, arglist, kwarglist)
	}
	storeOn { |stream|
		stream <<< a << "." << selector << "(" <<<* arglist << ")"

	}

}

FunctionList : AbstractFunction {
	var <>array, <flopped = false;

	*new { |functions|
		^super.newCopyArgs(functions ?? { Array.new })
	}
	addFunc { |... functions|
		if(flopped) { Error("cannot add a function to a flopped FunctionList").throw };
		array = array.addAll(functions)
	}
	removeFunc { |function|
		array.remove(function);
		if(array.size < 2) { ^array[0] }
	}

	replaceFunc { |find, replace|
		array = array.replace(find, replace)
	}

	value { |... args, kwargs|
		var res = array.collectCopy(_.valueArgs(args, kwargs));
		^if(flopped) { res.flop } { res }
	}
	valueArray { |args|
		var res = array.collectCopy(_.valueArray(args));
		^if(flopped) { res.flop } { res }
	}
	valueEnvir { |... args|
		var res = array.collectCopy(_.valueArrayEnvir(args));
		^if(flopped) { res.flop } { res }
	}
	valueArrayEnvir { |args|
		var res = array.collectCopy(_.valueArrayEnvir(args));
		^if(flopped) { res.flop } { res }
	}
	do { |function|
		array.do(function)
	}
	flop {
		if(flopped.not) { array = array.collect(_.flop) }; flopped = true
	}
	envirFlop {
		if(flopped.not) { array = array.collect(_.envirFlop) }; flopped = true
	}
	storeArgs { ^[array] }
	copy { ^super.copy.array_(array.copy) }
}
