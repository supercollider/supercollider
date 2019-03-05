
// James McCartney


s.boot;

(
// define a simple reverb.
SynthDef("reverb", {
	var in;
	in = In.ar(0, 2);
	5.do({ in = AllpassN.ar(in, 0.05, [0.05.rand, 0.05.rand], 2); });
	ReplaceOut.ar(0, in);
}).add;
)


(
// define 100 randomly generated percussive FM instruments.
// this will take a few seconds.
// this code can be modified to generate sustaining instruments.
var carrierF, middleF, modulatorF;
var maxAttack = 0.4;

carrierF = { arg freq=440, mod=0, mix=0, gate=1;
	var e, m;
	//e = Env.adsr(exprand(0.001,maxAttack), linrand(3.0), rrand(0.4,1.0).squared, rrand(0.001,0.2));
	e = Env.perc(exprand(0.001,maxAttack), exprand(0.1,2.0));
	m = linrand(10) + 1;
	e = EnvGen.kr(e, gate,  rrand(0.5,0.6).rand.squared);
	SinOsc.ar(freq * m, mod, e, mix);
};

middleF = { arg freq=440, mod=0, mix=0, gate=1;
	var e, m;
	//e = Env.adsr(exprand(0.001,maxAttack), linrand(3.0), 1.0.rand.squared, rrand(0.001,0.2));
	e = Env.perc(exprand(0.001,maxAttack), exprand(0.1,2.0));
	m = linrand(5) + 1;
	e = EnvGen.kr(e, gate,  3.0.rand.squared);
	SinOsc.ar(freq * m, mod, e, mix);
};

modulatorF = { arg freq=440, mix=0, gate=1;
	var e, m;
	//e = Env.adsr(exprand(0.001,maxAttack), linrand(3.0), 1.0.rand.squared, rrand(0.001,0.2));
	e = Env.perc(exprand(0.001,maxAttack), exprand(0.1,2.0));
	m = linrand(5) + 1;
	e = EnvGen.kr(e, gate,  3.0.rand.squared);
	SinOsc.ar(freq * m, 1.3.rand.cubed, e, mix);
};

100.do { |i|
	var name;
	name = "fmgen_a_" ++ i;
	SynthDef(name, { arg freq=440, amp=1, gate=1, pan=0;
		var c, m;
					
		c = 0;
		[
			{
				// sum of 3 modulator->carrier pairs
				[\A, i].postln;
				3.do {
					var f;
					f = freq + 1.8.rand2.squared;
					m = modulatorF.(f, 0, gate);
					c = carrierF.(f, m, c, gate);
				}
			},
			{
				// sum of 2 modulator->modulator->carrier chains
				[\B, i].postln;
				2.do {
					var f, m;
					f = freq + 1.8.rand2.squared;
					m = modulatorF.(f, 0, gate);
					m = middleF.(f, m, 0, gate);
					c = carrierF.(f, m, c, gate);
				}
			},
			{
				// sum of 2 modulator-+->carrier
				//                    |
				//                    +->carrier
				[\C, i].postln;
				2.do {
					var f;
					f = freq + 1.8.rand2.squared;
					m = modulatorF.(f, 0, gate);
					c = carrierF.(f, m, c, gate);
					c = carrierF.(f, m, c, gate);
				}
			},
		].choose.value;
		
		DetectSilence.ar(c, doneAction: 2);
		Out.ar(0, Pan2.ar(c, pan, amp));
	}).add;
};
)


(
// listen randomly to the random FM instruments.
Routine({
	var name;	
	s.sendMsg(\s_new, \reverb, 1000, 1, 0);
	name = "fmgen_a_" ++ 100.rand;
	name.postln;
	1000.do {	
		var freq, id;
		if (0.08.coin) { name = "fmgen_a_" ++ 100.rand; name.postln };
		[1,1,1,1,1,2,2].choose.do {
			freq = rrand(24,84).midicps;
			id = 1000000000.rand;
			s.sendMsg(\s_new, name, id, 0, 0, \freq, freq, \amp, 0.4, \pan, 1.0.rand2);
		};
		[0.1, 0.1, 0.1, 0.2, 0.2, 0.4, 0.8, 1.6].choose.wait;
	};
	s.sendMsg(\n_free, 1000);
}).play;
)


