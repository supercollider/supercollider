Poll : UGen {
	*ar {arg trig, in, label = "Ugen", trigid = -1;
		label = label.asString;
		label = Array.fill(label.size, {arg i; label[i].ascii});
		this.multiNewList(['audio', trig, in, trigid] ++ `label);
		^in;
		}
		
	*kr {arg trig, in, label = "Ugen", trigid = -1;
		label = label.asString;
		label = Array.fill(label.size, {arg i; label[i].ascii});
		this.multiNewList(['control', trig, in, trigid] ++ `label);
		^in;
		}
	*new1 {arg rate, trig, in, trigid, label;
		var labelar;
		labelar = label.dereference.asArray;
		^super.new.rate_(rate).addToSynth.init([trig, in, trigid, labelar.size] ++ labelar);
		}
		
 	checkInputs { ^this.checkSameRateAsFirstInput }

 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
}
	
/*
s.boot;

{Poll.ar(Impulse.ar(5), Line.ar(0, 1, 1), \test2)}.play(s);
{SinOsc.ar(220, 0, 1).poll(Impulse.ar(15), "test")}.play(s);

o = OSCresponderNode(s.addr, '/tr', {arg time, resp, msg;
	msg.postln;
	}).add

{Poll.ar(Impulse.ar(5), Line.ar(0, 1, 1), \test2, 1234)}.play(s);
	
o.remove;
s.quit;
*/