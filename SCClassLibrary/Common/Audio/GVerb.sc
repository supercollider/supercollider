GVerb : MultiOutUGen {

	*ar { arg in, roomsize = 10, revtime = 3, damping = 0.5, inputbw =  0.5, spread = 15,
			drylevel = 1, earlyreflevel = 0.7, taillevel = 0.5, maxroomsize = 300, mul = 1,
			add = 0;
		^this.multiNew('audio', in, roomsize, revtime, damping, inputbw, spread, drylevel,
			earlyreflevel, taillevel, maxroomsize).madd(mul, add);
	}

	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}

	checkInputs { ^this.checkNInputs(1); }
}


/*
s.options.memSize_(32768 * 4);
s.boot;
SynthDef(\test, {
	Out.ar(0, GVerb.ar(In.ar(24), 50, 100, MouseX.kr(0, 1), 0.15, 0.4, 1, 0.4, 0.7, 500))}).load(s)

s.sendMsg(\s_new, \test, 1000, 0, 1);
s.sendMsg(\n_free, 1000);

s.boot;
{GVerb.ar(In.ar(24))}.play
s.quit;
*/
