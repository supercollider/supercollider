
////////  the theremin ////////

// playing only one instance:
(
play(
	{
		var f;
		f = MouseY.kr(4000, 200, 'exponential', 0.8);
		SinOsc.ar(
			freq: f+ (f*SinOsc.ar(7,0,0.02)),
			mul: MouseX.kr(0, 0.9)
		)
	}
)
)


// building a synthdef and spawning separate synths

(
SynthDef(\theremin, { arg mod = 7, detune = 0;
	var f, a, z;
	f = MouseY.kr(4000, 200, 'exponential', 0.8) + detune;
	a = SinOsc.ar(f + (f * SinOsc.ar(mod,0,0.02)), mul: MouseX.kr(0, 0.9));
	z = Mix.ar(a);
	Out.ar(0, z) + Out.ar(1, z)
}).add
)

a = Synth(\theremin);
a.set(\mod, 12);

b = Synth(\theremin);
b.set(\mod, 5, \detune, 200);

a.free;
b.free;
