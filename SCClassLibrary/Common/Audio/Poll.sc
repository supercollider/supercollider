Poll : UGen {
	*ar { arg trig, in, label, trigid = -1;
		this.multiNewList(['audio', trig, in, label, trigid]);
		^in;
	}
	*kr { arg trig, in, label, trigid = -1;
		this.multiNewList(['control', trig, in, label, trigid]);
		^in;
	}
	*new { arg trig, in, label, trigid = -1;
		var rate = in.asArray.collect(_.rate).unbubble;
		this.multiNewList([rate, trig, in, label, trigid]);
		^in;
	}
	*new1 { arg rate, trig, in, label, trigid;
		label = label ?? {  "UGen(%)".format(in.class) };
		label = label.asString.collectAs(_.ascii, Array);
		if(rate === \scalar) { rate = \control };
		if(trig.isNumber) { trig = Impulse.multiNew(rate, trig, 0) };
		^super.new.rate_(rate).addToSynth.init([trig, in, trigid, label.size] ++ label);
	}

	checkInputs { ^this.checkSameRateAsFirstInput }

	init { arg theInputs;
		// store the inputs as an array
		inputs = theInputs;
	}
}
