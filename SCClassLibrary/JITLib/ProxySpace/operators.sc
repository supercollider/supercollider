AbstractOpPlug : AbstractFunction {

	composeUnaryOp { |aSelector|
		^UnaryOpPlug.new(aSelector, this)
	}
	composeBinaryOp { | aSelector, something |
		^BinaryOpPlug.new(aSelector, this, something)
	}
	reverseComposeBinaryOp { | aSelector, something |
		^BinaryOpPlug.new(aSelector, something, this)
	}
	composeNAryOp { | aSelector, anArgList |
		^{ this.value(anArgList).performList(aSelector, anArgList) }
	}
	writeInputSpec {
		Error("use .ar or .kr to use within a synth.").throw;
	}
	asUGenInput {
		^this.value(nil)
	}


}

UnaryOpPlug : AbstractOpPlug {
	var >operator, >a;

	*new { | operator, a |
		^super.newCopyArgs(operator, a)
	}

	isNeutral { ^a.isNeutral }

	rate { ^a.rate }

	numChannels { |max|
		var n;
		max = max ? 0;
		n = a.numChannels(max);
		^n !? { max(n, max) }
	}

	value { |obj|
		^a.value(obj).perform(operator)
	}

	initBus { |rate, numChannels|
		^a.initBus(rate, numChannels)
	}

	wakeUp  {
		a.wakeUp
	}

	prepareForProxySynthDef { |proxy|
		^{ this.value(proxy) }
	}

	asControlInput {
		"UnaryOpPlug: Cannot calculate this value. Use direct mapping only.".warn;
		^this
	}
}


BinaryOpPlug : AbstractOpPlug  {
	var >operator, <>a, <>b;

	*new { | operator, a, b |
		^super.newCopyArgs(operator, a, b)
	}

	value { |proxy|
		^a.value(proxy).perform(operator, b.value(proxy))
	}

	initBus { | rate, numChannels |
		^a.initBus(rate, numChannels) and: { b.initBus(rate, numChannels) };
	}

	isNeutral { ^a.isNeutral and: { b.isNeutral } }

	rate {
		// \audio < \control < \scalar
		// note that function.rate is defined as \stream
		var arate = \scalar, brate = \scalar;
		if(a.respondsTo(\rate)) { arate = a.rate };
		if(b.respondsTo(\rate)) { brate = b.rate };
		^if(arate < brate) { arate } { brate }
	}

	numChannels { |max|
		var n1, n2, res;
		max = max ? 0;
		n1 = a.numChannels(max);
		if(n1.notNil) { max = n1 };
		n2 = b.numChannels(max);
		res = if(n1.isNil) { n2 } { if(n2.isNil) { n1 } { max(n1, n2) } };
		^res !? { max(max, res) }
	}
	wakeUp  {
		a.wakeUp;
		b.wakeUp;
	}
	asControlInput {
		"BinaryOpPlug: Cannot calculate this value. Use direct mapping only.".warn;
		^this
	}


}
