Poll : UGen {
	*ar { |trig, in, label, trigid = -1|
		this.multiNewList(['audio', trig, in, label, trigid]);
		^in
	}
	*kr { |trig, in, label, trigid = -1|
		this.multiNewList(['control', trig, in, label, trigid]);
		^in
	}
	*new { |trig, in, label, trigid = -1|
		var rate = in.asArray.collect(_.rate).unbubble;
		this.multiNewList([rate, trig, in, label, trigid]);
		^in
	}
	*new1 { |rate, trig, in, label, trigid|
		label = label ?? { "UGen(%)".format(in.class) };
		label = label.asString.collectAs(_.ascii, Array);
		if(rate === \scalar) { rate = \control };
		if(trig.isNumber) { trig = Impulse.multiNew(rate, trig, 0) };
		^super.new.rate_(rate).addToSynth.init([trig, in, trigid, label.size] ++ label)
	}

	checkInputs { ^this.checkSameRateAsFirstInput }

	init { |theInputs|
		// store the inputs as an array
		inputs = theInputs
	}
}
