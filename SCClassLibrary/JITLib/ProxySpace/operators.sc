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


}

UnaryOpPlug : AbstractOpPlug {
	var >operator, >a;

	*new { | operator, a |
		^super.newCopyArgs(operator, a)
	}

	isNeutral { ^a.isNeutral }

	rate { ^a.rate }

	numChannels { |max|
		var n, res;
		max = max ? 0;
		n = a.numChannels(max);
		^if(n.isNil, { nil }, { max(n, max) });
	}

	value { |proxy|
		var rate, numChannels;
		rate = this.rate;
		if(rate === 'stream') { rate = nil };  // crucial library defines rate of func as \stream
		numChannels = this.numChannels;
		if(rate.notNil and: { numChannels.notNil } and: { proxy.notNil }, {
			proxy.initBus(rate, numChannels)
		});
		a.initBus(rate, numChannels);
		^a.value(proxy).perform(operator)
	}

	initBus { |rate, numChannels|
		^a.initBus(rate, numChannels)
	}
	wakeUp  {
		a.wakeUp;
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
		var vala, valb, rate, numChannels;
		rate = this.rate;
		if(rate === 'stream') { rate = nil };  // cx defines rate of func as \stream
		numChannels = this.numChannels;
		if(rate.notNil and: { numChannels.notNil } and: { proxy.notNil }, {
			proxy.initBus(rate, numChannels)
		});
		this.initBus(rate, numChannels);
		vala = a.value(proxy);
		valb = b.value(proxy);
		^vala.perform(operator, valb)
	}
	initBus { | rate, numChannels |
		^a.initBus(rate, numChannels) and: { b.initBus(rate, numChannels) };
	}

	isNeutral { ^a.isNeutral && b.isNeutral }

	rate {
		if(a.isNeutral) { ^b.rate };
		if(b.isNeutral) { ^a.rate };
		^if(a.rate !== \control) { a.rate } { b.rate } // as function.rate is defined as \stream
	}

	numChannels { |max|
		var n1, n2, res;
		max = max ? 0;
		n1 = a.numChannels(max);
		if(n1.notNil, { max = n1 });
		n2 = b.numChannels(max);
		res = if(n1.isNil, { n2 }, { if(n2.isNil, { n1 }, { max(n1, n2) }) });
		^if(res.notNil, { max(max,res) }, { nil })
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
