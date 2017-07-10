

// LMFAP: Little Man From Another Place (the dwarf in the Red Room)
// (thor magnusson) (2006)

// better use headphones !!!
(
s = Server.local;
b = Buffer.alloc(s, 44100 * 6.0, 2);
)
(
SynthDef(\LMFAP, {|combdecay=1, allpassdecay=0.5, wet=0.1, bufnum, rate=1, end=264600|
	var in, signal;
	in = SoundIn.ar([0,1]);
	z = DelayN.ar(in, 0.1, 0.51);
	y = Mix.ar(Array.fill(3,{ CombL.ar(z, 0.05, rrand(0.03, 0.05), combdecay) }));
	6.do({ y = AllpassN.ar(y, 0.030, rrand(0.01, 0.03), allpassdecay) });
	BufWr.ar(in+(y*wet), bufnum, Phasor.ar(0, 1, 0, end));
	signal = BufRd.ar(2, bufnum, Phasor.ar(0, BufRateScale.kr(0)*rate, 0, end));
	y = Mix.ar(Array.fill(7,{ CombL.ar(signal, 0.05, rrand(0.03, 0.05), combdecay) }));
	6.do({ y = AllpassN.ar(y, 0.050, rrand(0.03, 0.05), allpassdecay) });
	Out.ar(0, signal+(y*wet));
}).add;
)

// wait a bit
c = Synth(\LMFAP, [\bufnum, b.bufnum, \rate, -1]);


c.set(\combdecay, 3)
c.set(\allpassdecay, 0.4)

c.set(\rate, -0.8)

c.set(\wet, 0.4) // more reverb

c.set(\end, 44100*3) // 3 sec loop

c.free;
b.free;
