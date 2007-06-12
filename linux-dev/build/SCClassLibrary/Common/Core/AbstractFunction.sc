AbstractFunction {
	
	// function compositions
	// override these in subclasses to perform different kinds of function compositions
	composeUnaryOp { arg aSelector;
		^{ this.value.perform(aSelector) }
	}
	composeBinaryOp { arg aSelector, function;
		^{ this.value.perform(aSelector, function.value) }
	}
	reverseComposeBinaryOp { arg aSelector, something;
		^{ something.value.perform(aSelector, this.value) }
	}
	composeNAryOp { arg aSelector, anArgList;
		^{ this.value.performList(aSelector, anArgList) }
	}

	// double dispatch for mixed operations
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; 
		^this.reverseComposeBinaryOp(aSelector, aNumber) 
	}
	performBinaryOpOnSignal { arg aSelector, aSignal; ^this.reverseComposeBinaryOp(aSelector, aSignal) }
	performBinaryOpOnComplex { arg aSelector, aComplex; ^this.reverseComposeBinaryOp(aSelector, aComplex) }
	performBinaryOpOnSeqColl { arg aSelector, aSeqColl; ^this.reverseComposeBinaryOp(aSelector, aSeqColl) }
	
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
	nyqring { ^this.composeUnaryOp('nyqring') }
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

	
	+ { arg function; ^this.composeBinaryOp('+', function) }
	- { arg function; ^this.composeBinaryOp('-', function) }
	* { arg function; ^this.composeBinaryOp('*', function) }
	/ { arg function; ^this.composeBinaryOp('/', function) }
	div { arg function; ^this.composeBinaryOp('div', function) }
	mod { arg function; ^this.composeBinaryOp('mod', function) }
	pow { arg function; ^this.composeBinaryOp('pow', function) }
	min { arg function; ^this.composeBinaryOp('min', function) } 
	max { arg function; ^this.composeBinaryOp('max', function) }
	
	<  { arg function; ^this.composeBinaryOp('<',  function) }
	<= { arg function; ^this.composeBinaryOp('<=', function) }
	>  { arg function; ^this.composeBinaryOp('>',  function) }
	>= { arg function; ^this.composeBinaryOp('>=', function) }
	
	bitAnd { arg function; ^this.composeBinaryOp('bitAnd', function) }
	bitOr { arg function; ^this.composeBinaryOp('bitOr', function) }
	bitXor { arg function; ^this.composeBinaryOp('bitXor', function) }	
	lcm { arg function; ^this.composeBinaryOp('lcm', function) }
	gcd { arg function; ^this.composeBinaryOp('gcd', function) }
	round { arg function; ^this.composeBinaryOp('round', function) }
	trunc { arg function; ^this.composeBinaryOp('trunc', function) }
	atan2 { arg function; ^this.composeBinaryOp('atan2', function) }	
	hypot { arg function; ^this.composeBinaryOp('hypot', function) }
	hypotApx { arg function; ^this.composeBinaryOp('hypotApx', function) }
	leftShift { arg function; ^this.composeBinaryOp('leftShift', function) }
	rightShift { arg function; ^this.composeBinaryOp('rightShift', function) }
	unsignedRightShift { arg function; ^this.composeBinaryOp('unsignedRightShift', function) }
	ring1 { arg function; ^this.composeBinaryOp('ring1', function) }
	ring2 { arg function; ^this.composeBinaryOp('ring2', function) }
	ring3 { arg function; ^this.composeBinaryOp('ring3', function) }
	ring4 { arg function; ^this.composeBinaryOp('ring4', function) }
	difsqr { arg function; ^this.composeBinaryOp('difsqr', function) }
	sumsqr { arg function; ^this.composeBinaryOp('sumsqr', function) }
	sqrsum { arg function; ^this.composeBinaryOp('sqrsum', function) }
	sqrdif { arg function; ^this.composeBinaryOp('sqrdif', function) }
	absdif { arg function; ^this.composeBinaryOp('absdif', function) }
	thresh { arg function; ^this.composeBinaryOp('thresh', function) }
	amclip { arg function; ^this.composeBinaryOp('amclip', function) }
	scaleneg { arg function; ^this.composeBinaryOp('scaleneg', function) }
	clip2 { arg function; ^this.composeBinaryOp('clip2', function) }
	fold2 { arg function; ^this.composeBinaryOp('fold2', function) }
	wrap2 { arg function; ^this.composeBinaryOp('wrap2', function) }
	excess { arg function; ^this.composeBinaryOp('excess', function) }
	firstArg { arg function; ^this.composeBinaryOp('firstArg', function) }
	rrand { arg function; ^this.composeBinaryOp('rrand', function) }
	exprand { arg function; ^this.composeBinaryOp('exprand', function) }

	// complex support
	real { ^this }
	imag { ^0.0 }

	|| { arg function; ^this.composeBinaryOp('||', function) }
	&& { arg function; ^this.composeBinaryOp('&&', function) }
	xor { arg function; ^this.composeBinaryOp('xor', function) }
	not { ^this.composeUnaryOp('not') }
}
