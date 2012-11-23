AbstractFunction {

	// function compositions
	// override these in subclasses to perform different kinds of function compositions
	composeUnaryOp { arg aSelector;
		^UnaryOpFunction.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunction.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunction.new(aSelector, something, this, adverb);
	}
	composeNAryOp { arg aSelector, anArgList;
		^NAryOpFunction.new(aSelector, this, anArgList)
	}

	// double dispatch for mixed operations
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^this.reverseComposeBinaryOp(aSelector, aNumber, adverb)
	}
	performBinaryOpOnSignal { arg aSelector, aSignal, adverb;
		^this.reverseComposeBinaryOp(aSelector, aSignal, adverb)
	}
	performBinaryOpOnComplex { arg aSelector, aComplex, adverb;
		^this.reverseComposeBinaryOp(aSelector, aComplex, adverb)
	}
	performBinaryOpOnSeqColl { arg aSelector, aSeqColl, adverb;
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

	rho {  ^this.composeUnaryOp('rho') }
	theta {  ^this.composeUnaryOp('theta') }
	rotate { arg function; ^this.composeBinaryOp('rotate', function) }
	dist { arg function; ^this.composeBinaryOp('dist', function) }

	+ { arg function, adverb; ^this.composeBinaryOp('+', function, adverb) }
	- { arg function, adverb; ^this.composeBinaryOp('-', function, adverb) }
	* { arg function, adverb; ^this.composeBinaryOp('*', function, adverb) }
	/ { arg function, adverb; ^this.composeBinaryOp('/', function, adverb) }
	div { arg function, adverb; ^this.composeBinaryOp('div', function, adverb) }
	mod { arg function, adverb; ^this.composeBinaryOp('mod', function, adverb) }
	pow { arg function, adverb; ^this.composeBinaryOp('pow', function, adverb) }
	min { arg function, adverb; ^this.composeBinaryOp('min', function, adverb) }
	max { arg function=0, adverb; ^this.composeBinaryOp('max', function, adverb) }

	<  { arg function, adverb; ^this.composeBinaryOp('<',  function, adverb) }
	<= { arg function, adverb; ^this.composeBinaryOp('<=', function, adverb) }
	>  { arg function, adverb; ^this.composeBinaryOp('>',  function, adverb) }
	>= { arg function, adverb; ^this.composeBinaryOp('>=', function, adverb) }

	bitAnd { arg function, adverb; ^this.composeBinaryOp('bitAnd', function, adverb) }
	bitOr { arg function, adverb; ^this.composeBinaryOp('bitOr', function, adverb) }
	bitXor { arg function, adverb; ^this.composeBinaryOp('bitXor', function, adverb) }
	bitHammingDistance { arg function, adverb; ^this.composeBinaryOp('hammingDistance', function, adverb) }

	lcm { arg function, adverb; ^this.composeBinaryOp('lcm', function, adverb) }
	gcd { arg function, adverb; ^this.composeBinaryOp('gcd', function, adverb) }
	round { arg function=1, adverb; ^this.composeBinaryOp('round', function, adverb) }
	roundUp { arg function=1, adverb; ^this.composeBinaryOp('roundUp', function, adverb) }
	trunc { arg function=1, adverb; ^this.composeBinaryOp('trunc', function, adverb) }
	atan2 { arg function, adverb; ^this.composeBinaryOp('atan2', function, adverb) }
	hypot { arg function, adverb; ^this.composeBinaryOp('hypot', function, adverb) }
	hypotApx { arg function, adverb; ^this.composeBinaryOp('hypotApx', function, adverb) }
	leftShift { arg function, adverb; ^this.composeBinaryOp('leftShift', function, adverb) }
	rightShift { arg function, adverb; ^this.composeBinaryOp('rightShift', function, adverb) }
	unsignedRightShift { arg function, adverb; ^this.composeBinaryOp('unsignedRightShift', function, adverb) }
	ring1 { arg function, adverb; ^this.composeBinaryOp('ring1', function, adverb) }
	ring2 { arg function, adverb; ^this.composeBinaryOp('ring2', function, adverb) }
	ring3 { arg function, adverb; ^this.composeBinaryOp('ring3', function, adverb) }
	ring4 { arg function, adverb; ^this.composeBinaryOp('ring4', function, adverb) }
	difsqr { arg function, adverb; ^this.composeBinaryOp('difsqr', function, adverb) }
	sumsqr { arg function, adverb; ^this.composeBinaryOp('sumsqr', function, adverb) }
	sqrsum { arg function, adverb; ^this.composeBinaryOp('sqrsum', function, adverb) }
	sqrdif { arg function, adverb; ^this.composeBinaryOp('sqrdif', function, adverb) }
	absdif { arg function, adverb; ^this.composeBinaryOp('absdif', function, adverb) }
	thresh { arg function, adverb; ^this.composeBinaryOp('thresh', function, adverb) }
	amclip { arg function, adverb; ^this.composeBinaryOp('amclip', function, adverb) }
	scaleneg { arg function, adverb; ^this.composeBinaryOp('scaleneg', function, adverb) }
	clip2 { arg function=1, adverb; ^this.composeBinaryOp('clip2', function, adverb) }
	fold2 { arg function=1, adverb; ^this.composeBinaryOp('fold2', function, adverb) }
	wrap2 { arg function=1, adverb; ^this.composeBinaryOp('wrap2', function, adverb) }
	excess { arg function=1, adverb; ^this.composeBinaryOp('excess', function, adverb) }
	firstArg { arg function, adverb; ^this.composeBinaryOp('firstArg', function, adverb) }
	rrand { arg function, adverb; ^this.composeBinaryOp('rrand', function, adverb) }
	exprand { arg function, adverb; ^this.composeBinaryOp('exprand', function, adverb) }
	@ { arg function, adverb; ^this.composeBinaryOp('@', function, adverb) }

	// complex support
	real { ^this }
	imag { ^0.0 }


	|| { arg function, adverb; ^this.composeBinaryOp('||', function, adverb) }
	&& { arg function, adverb; ^this.composeBinaryOp('&&', function, adverb) }
	xor { arg function, adverb; ^this.composeBinaryOp('xor', function, adverb) }
	nand { arg function, adverb; ^this.composeBinaryOp('nand', function, adverb) }
	not { ^this.composeUnaryOp('not') }
	ref { ^this.composeUnaryOp('asRef') }

	// nary operators
	clip { arg lo, hi; ^this.composeNAryOp('clip', [lo,hi])  }
	wrap { arg lo, hi; ^this.composeNAryOp('wrap', [lo,hi])  }
	fold { arg lo, hi; ^this.composeNAryOp('fold', [lo,hi])  }
	blend { arg that, blendFrac = 0.5;
		^this.composeNAryOp('blend', [that, blendFrac])
	}
	linlin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('linlin', [inMin, inMax, outMin, outMax, clip])
	}
	linexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('linexp', [inMin, inMax, outMin, outMax, clip])
	}
	explin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('explin', [inMin, inMax, outMin, outMax, clip])
	}
	expexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('expexp', [inMin, inMax, outMin, outMax, clip])
	}
	lincurve { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		^this.composeNAryOp('lincurve', [inMin, inMax, outMin, outMax, curve, clip])
	}
	curvelin { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		^this.composeNAryOp('curvelin', [inMin, inMax, outMin, outMax, curve, clip])
	}
	bilin { arg inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('bilin', [inCenter, inMin, inMax, outCenter, outMin, outMax, clip])
	}
	biexp { arg inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax;
		^this.composeNAryOp('biexp', [inCenter, inMin, inMax, outCenter, outMin, outMax, clip])
	}

	degreeToKey { arg scale, stepsPerOctave=12;
		^this.composeNAryOp('degreeToKey', [scale, stepsPerOctave])
	}

	degrad { ^this.composeUnaryOp('degrad') }
	raddeg { ^this.composeUnaryOp('raddeg') }

	applyTo { arg ... args;
		^this.valueArray(args)
	}

	<> { arg that;
		// function composition
		^{|...args| this.value(that.value(*args)) }
	}

	sampled{ |n=80,from=0.0,to=1.0|
		var valueArray;
		valueArray = (from,(to-from)/(n-1) .. to).collect{|x| this.value(x) };
		^{ |x| valueArray.blendAt( ((x.clip(from,to)-from)/(to-from))*(n-1) ) }
	}

	// embed in ugen graph
	asUGenInput { arg for; ^this.value(for) }
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

	*new { arg selector, a;
		^super.newCopyArgs(selector, a)
	}
	value { arg ... args;
		^a.valueArray(args).perform(selector)
	}
	valueArray { arg args;
		^a.valueArray(args).perform(selector)
	}
	valueEnvir { arg ... args;
		^a.valueArrayEnvir(args).perform(selector)
	}
	valueArrayEnvir { arg ... args;
		^a.valueArrayEnvir(args).perform(selector)
	}
	functionPerformList { arg selector, arglist;
		^this.performList(selector, arglist)
	}
	storeOn { arg stream;
		stream <<< a << "." << selector;
	}


}

BinaryOpFunction : AbstractFunction {
	var selector, a, b, adverb;

	*new { arg selector, a, b, adverb;
		^super.newCopyArgs(selector, a, b, adverb)
	}
	value { arg ... args;
		^a.valueArray(args).perform(selector, b.valueArray(args), adverb)
	}
	valueArray { arg args;
		^a.valueArray(args).perform(selector, b.valueArray(args), adverb)
	}
	valueEnvir { arg ... args;
		^a.valueArrayEnvir(args).perform(selector, b.valueArrayEnvir(args), adverb)
	}
	valueArrayEnvir { arg ... args;
		^a.valueArrayEnvir(args).perform(selector, b.valueArrayEnvir(args), adverb)
	}
	functionPerformList { arg selector, arglist;
		^this.performList(selector, arglist)
	}
	storeOn { arg stream;
		stream << "(" <<< a << " " << selector.asBinOpString;
		if(adverb.notNil) { stream << "." << adverb };
		stream << " " <<< b << ")"

	}
}

NAryOpFunction : AbstractFunction {
	var selector, a, arglist;

	*new { arg selector, a, arglist;
		^super.newCopyArgs(selector, a, arglist)
	}
	value { arg ... args;
		^a.valueArray(args).performList(selector, arglist.collect(_.valueArray(args)))
	}
	valueArray { arg args;
		^a.valueArray(args).performList(selector, arglist.collect(_.valueArray(args)))
	}
	valueEnvir { arg ... args;
		^a.valueArrayEnvir(args).performList(selector, arglist.collect(_.valueArrayEnvir(args)))
	}
	valueArrayEnvir { arg ... args;
		^a.valueArrayEnvir(args).performList(selector, arglist.collect(_.valueArrayEnvir(args)))
	}
	functionPerformList { arg selector, arglist;
		^this.performList(selector, arglist)
	}
	storeOn { arg stream;
		stream <<< a << "." << selector << "(" <<<* arglist << ")"

	}

}

FunctionList : AbstractFunction {
	var <>array, <flopped=false;

	*new { arg functions;
		^super.newCopyArgs(functions)
	}
	addFunc { arg ... functions;
		if(flopped) { Error("cannot add a function to a flopped FunctionList").throw };
		array = array.addAll(functions)
	}
	removeFunc { arg function;
		array.remove(function);
		if(array.size < 2) { ^array[0] };
	}

	replaceFunc { arg find, replace;
		array = array.replace(find, replace);
	}

	value { arg ... args;
		var res = array.collect(_.valueArray(args));
		^if(flopped) { res.flop } { res }
	}
	valueArray { arg args;
		var res = array.collect(_.valueArray(args));
		^if(flopped) { res.flop } { res }
	}
	valueEnvir { arg ... args;
		var res = array.collect(_.valueArrayEnvir(args));
		^if(flopped) { res.flop } { res }
	}
	valueArrayEnvir { arg args;
		var res = array.collect(_.valueArrayEnvir(args));
		^if(flopped) { res.flop } { res }
	}
	do { arg function;
		array.do(function)
	}
	flop {
		if(flopped.not) {array = array.collect(_.flop) }; flopped = true;
	}
	envirFlop {
		if(flopped.not) {array = array.collect(_.envirFlop) }; flopped = true;
	}
	storeArgs { ^[array] }
}
