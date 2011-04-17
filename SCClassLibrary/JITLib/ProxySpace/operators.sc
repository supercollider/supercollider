AbstractOpPlug : AbstractFunction {

	composeUnaryOp { arg aSelector;
		^UnaryOpPlug.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, this, something)
	}
	reverseComposeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, something, this)
	}
	composeNAryOp { arg aSelector, anArgList;
		^{ this.value(anArgList).performList(aSelector, anArgList) }
	}
	writeInputSpec {
		Error("use .ar or .kr to use within a synth.").throw;
	}


}

UnaryOpPlug : AbstractOpPlug {
	var >operator, >a;
	*new { arg operator, a;
		^super.newCopyArgs(operator, a)
	}

	isNeutral { ^a.isNeutral }

	rate { ^a.rate }

	numChannels { arg max;
		var n, res;
		max = max ? 0;
		n = a.numChannels(max);
		^if(n.isNil, { nil }, { max(n, max) });
	}

	value { arg proxy;
		var rate, numChannels;
		rate = this.rate;
		if(rate === 'stream') { rate = nil };  // cx defines rate of func as \stream
		numChannels = this.numChannels;
		if(rate.notNil and: { numChannels.notNil } and: { proxy.notNil }, {
			proxy.initBus(rate, numChannels)
		});
		a.initBus(rate, numChannels);
		^a.value(proxy).perform(operator)
	}

	initBus { arg rate, numChannels;
		^a.initBus(rate, numChannels)
	}
	wakeUp  {
		a.wakeUp;
	}

	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}

	asControlInput {
		"UnaryOpPlug: Cannot calculate this value. Use direct mapping only.".warn;
		^this
	}
}


BinaryOpPlug : AbstractOpPlug  {
	var >operator, <>a, <>b;
	*new { arg operator, a, b;
		^super.newCopyArgs(operator, a, b)
	}

	value { arg proxy;
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
	initBus { arg rate, numChannels;
		^a.initBus(rate, numChannels) and: { b.initBus(rate, numChannels) };
	}

	isNeutral { ^a.isNeutral && b.isNeutral }

	rate {
		if(a.isNeutral) { ^b.rate };
		if(b.isNeutral) { ^a.rate };
		^if(a.rate !== \control) { a.rate } { b.rate } // as function.rate is defined as \stream
	}

	numChannels { arg max;
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
