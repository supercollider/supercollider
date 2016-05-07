// julian rohrhuber (c 2007)

(
SynthDef("deepsea", { arg out, pan=0, amp=0.1, variation=0.9;
	var dt, n, freq, mul, t, u;
	n = Rand(7, 46);
	dt = 25.0 + Rand(-1.7, 1.7);
	dt = dt + LFNoise2.kr(2, variation) * 0.001;
	freq = 901 + Rand(0, 65);
	t = Impulse.ar(dt.reciprocal, 0, 100);
	mul = PulseCount.ar(t) < n;
	u = BPF.ar(mul * t, freq, 0.1);
	u = BPF.ar(u, freq, 0.2);
	DetectSilence.ar(u, doneAction:2);
	Out.ar(out, Pan2.ar(u, pan, amp));
}).add;
)

(
SynthDef("deepsea", { arg out, pan=0, amp=0.1, variation=0.9;
	var dt, n, freq, mul, t, u, count;
	n = Rand(7, 46);
	dt = 25.0 + Rand(-1.7, 1.7);
	dt = dt + LFNoise2.kr(2, variation) * 0.001;
	freq = 901 + Rand(0, 65);
	t = Impulse.ar(dt.reciprocal, 0, 100);
	count = PulseCount.ar(t);
	mul = count < n;
	u = BPF.ar(mul * t, freq, 0.1);
	u = BPF.ar(u, freq * (count % LFNoise1.kr(1).range(2, 20) + 1), 0.2);
	DetectSilence.ar(u, doneAction:2);
	Out.ar(out, Pan2.ar(u, pan, amp * 10));
}).add;
)



s.sendMsg("/s_new", "deepsea", -1);


p = ProxySpace.push(s.boot);

~out.play;
~out.fadeTime = 10;

(
~fish = Pbind(
		\instrument, \deepsea,
		\dur, Pfunc #{ rrand(9.0, 2.0) },
		\pan, Pfunc #{ 1.0.rand2 }
		);
)

~out = ~fish;
~out[1] = \filter -> #{ arg in; BPF.ar(in * 5, 700, 0.1) };

(
~out[2] = \filter -> #{ arg in;
	CombL.ar(LPF.ar(in * LFNoise1.kr(0.1).max(0), 800), 0.5, 0.5) + in.reverse
};
)

(
~out[3] = \filter -> #{ arg in; var x; x = in;
						5.do { x = AllpassN.ar(x, 0.18, {0.06.rand} ! 2 + 0.06, 8) };
						LPF.ar(x + in, 400) + (LFNoise1.kr(0.2) * in)
					};

)



~out[1] = nil;
~out[2] = nil;


p.clear(10).pop;
