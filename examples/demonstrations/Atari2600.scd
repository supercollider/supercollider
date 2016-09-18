
/*
// Fredrik Olofsson

A quick demo of Fredrik Olofsson's Atari 2600 plugin, which can be downloaded from:


www.fredrikolofsson.com/pages/code-sc.html

This lovely 8-bit tune is based on an example in the helpfile.

*/

// Simple synth definition using the Atari2600 UGen:
(
SynthDef(\atari2600, {|out= 0, gate= 1, tone0= 5,
	  tone1= 8, freq0= 10, freq1= 20, amp= 1, pan= 0|
	var e, z;
	e= EnvGen.kr(Env.asr(0.01, amp, 0.05), gate, doneAction:2);
	z= Atari2600.ar(tone0, tone1, freq0, freq1, 15, 15);
	Out.ar(out, Pan2.ar(z*e, pan));
}).add;
)

// And a pattern to play it:
(
Pbind(
	\instrument, \atari2600,
	\dur, Pseq([0.25, 0.25, 0.25, 0.45], inf),
	\amp, 0.8,
	\tone0, Pseq([Pseq([2, 5], 32), Pseq([3, 5], 32)], inf),
	\tone1, 14,
	\freq0, Pseq([Pbrown(28, 31, 1, 32),
						  Pbrown(23, 26, 3, 32)], inf),
	\freq1, Pseq([Pn(10, 16), Pn(11, 16)], inf)
).play
)
