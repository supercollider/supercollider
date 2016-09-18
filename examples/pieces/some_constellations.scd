// some small pieces
// Julian Rohrhuber, 2007


// stereo half-life
(
{
	var activity, nAtoms, tHalf, n;

	tHalf = 3.92; // Radon-219, discovered 1904 by Giesel and Debierne.
	nAtoms = 1e+5; // ca. 3.6e-14 mg

	n = max(0, nAtoms - PulseCount.ar(LocalIn.ar(2)));
	activity = Dust.ar( n * 2.log / tHalf);
	LocalOut.ar(activity);
	activity;
}.play;
)


// a bath
(
{
	var sources, u, uu, above, aside;
	above = MouseY.kr(0, 2).round(1);
	aside = MouseX.kr(1, 1.6);
	sources = [
		WhiteNoise.ar([0.3, 0.3]),
		PinkNoise.ar([1, 1]),
		LFDNoise3.ar(10000)
		* 0.1
	];
	u = SelectX.ar(
		LFDNoise1.kr(4).range(0, sources.size),
		sources
	) * 0.1;
	u = u + DelayN.ar(u, 0.1, [0.001, 0.0012], 0.1);

	uu = u.collect {|chan|
				RLPF.ar(
					u,
					{ rrand(100.0, 340) * aside }.dup(4),
					0.2
				).sum
			};
	uu = CombL.ar(uu.reverse, 0.05, 0.05, 0.3, 0.3) * LFNoise2.kr(0.2).max(0) + uu;
	SelectX.ar(above.lag(0.4), [u, uu]) * 2;
}.play;
)


// IBM thinkpad
(
SynthDef("ibm", {
		OffsetOut.ar(0, Pulse.ar(1000, 0.5, EnvGen.ar(Env.linen(0, 0.01, 0, 0.1), doneAction:2)))
}).add;

fork {
	var coin = 0.5;
	loop {
		if(coin.coin) { coin = rrand(0.1, 1) };
		(coin * (1 + 0.5.rand2) * 20).wait;
		(instrument: \ibm).play;
	}
}
);


// dial history
(
{
	var pat, trig, rate, which, mfv, numbers, both, dial, sig, n = 8;
	mfv = [[697, 770, 852, 941], [1209, 1336, 1477, 1633]];
	numbers = [[3, 1]] ++ {:[a,b], a<-(0..2), b<-(0..2)}.all;
	n = Dwhite(7, 12, inf);
	rate = Dseq([ Dwhite(2, 7, 1), Dbrown(0.1, 0.2, 0.01, n) ], inf);

	trig = Trig.kr(TDuty.kr(rate, 0, Dseq((1..10), inf)), 0.09);
	pat = Latch.kr(trig, trig);
	which = Hasher.kr(pat * MouseX.kr).range(0, numbers.size).trunc.poll(trig, "");
	both = Select.kr(which, numbers);
	dial = Select.kr(both, mfv.flop);
	sig = SinOsc.ar(dial) * 0.05 * trig;
	[0,
	DelayC.ar(
		sig,
		0.2,
		LFDNoise3.kr(0.5).range(0, 0.01)
	)
	+ GrayNoise.ar(0.01) + HPF.ar(GrayNoise.ar(0.02), 3000)]
}.play
)


// sturmian sequencer
(
var rules, axiom;
"MathLib".include; // needs MathLib

rules = [
	"0" -> "01",
	"1" -> "0"
];

axiom = "0";

{
	Splay.ar({ |i|
		var str = axiom.rewriteString(rules, i + 6);
		var dt = 2 ** i.neg * 10;
		var trig = TDuty.ar(dt, 0,
			Dseq(str.collectAs(_.digit, Array).postln)
		);
		var freq = ExpRand(200, i + 1 / 7 * 10100);
		Ringz.ar(trig, freq * [1, 1.2, 1.5], ExpRand(2 ** i.neg * 0.1, 1.101) ).sum.distort
	}.dup(7), 0.5) * 0.3
}.play;
)



// sturmian sequencer II
(
var rules, axiom;
"MathLib".include; // needs MathLib

rules = [
	"0" -> "01",
	"1" -> "0"
];

axiom = "0";

{
	var n = 7, trig = 0;

	Splay.ar({ |i|
		var str = axiom.rewriteString(rules, i + 6);
		var dt = 2 ** (n - i).neg * 20;
		var trig = TDuty.ar(dt, 0,
			Dseq(str.collectAs(_.digit, Array), inf)
		);
		var freq = TExpRand.kr(200, (n - i) / n * 10100, trig);
		trig = BPF.ar(trig, LFNoise2.kr(0.1, 0.02, 1) * freq, 0.2);
		Ringz.ar(trig, freq * [1, 1.1, 1.2], ExpRand(2 ** i.neg * 0.1, 0.5)).sum.distort;

	}.dup(n), 0.5) * 0.3
}.play;
)

// sturmian sequencer III
(
var rules, axiom;
"MathLib".include; // needs MathLib

rules = [
	"0" -> "01",
	"1" -> "0"
];

axiom = "0";

{
	var n = 9;
	Splay.ar({ |i|
		var str = axiom.rewriteString(rules, i + 6);
		var dt = SampleDur.ir / (n - i + 2) * MouseX.kr(1, SampleRate.ir, 2);
		TDuty.ar(dt, 0,
			Dseq(str.collectAs(_.digit, Array) - 0.5, inf)
		);
	}.dup(n), 0.5) * 0.3
}.play;
)




// practise in the abstract
(
SynthDef(\strings, { arg out, freq=440, amp=0.1, gate=1, pan, freqLag=0.2;
					var env, in, delay, f1, f2;
					f1 = freq.lag(freqLag);
					f2 = freq.lag(freqLag * 0.5);
					delay = 0.25 / f2;
					env = Env.asr(0, 1, 0.3);
					in = WhiteNoise.ar(180);
					in = CombL.ar(in, delay, delay, 1);
					in = Resonz.ar(in, f1, 0.001).abs;
					in = in * EnvGen.kr(env, gate, doneAction:2);
					Out.ar(out, Pan2.ar(in, pan, amp));
}).add;

(
Pdef(\vi,
	Pbind(
        \instrument, \strings,
        \degree, Pseq([
        			Pn(\rest, 2),
        			Pshuf([0, 2, 3, 5], 10),
        			Pseq([\rest], { 9.rand })
        			], inf)
        			+ Prand([0, 0, 0, [0, 3], [0, 1]], inf),
        \dur, Pseq([1, 2, 0.3, 0.5, 0.5], inf) + (Prand([0.3, 0, 0.2], inf) * 0.1),
        \detune, Pseg(Pwhite(-2, 2, inf), 1.3)
       )
).play
);
)



// chainsaw
(
var f, g;

f = { | saw |
	var freq, u, amp, rate;

	rate = ExpRand(0.1, 2);
	freq = if(0.6.coin) {
		LFNoise1.kr(rate.dup).exprange(0.01, 10)
	} {
		LFNoise1.kr(rate.dup).exprange(10, 50)
	};
	u = LFSaw.kr(LinExp.kr(saw, -1, 1, freq, freq * LFNoise1.kr(rate.dup).exprange(2, 10)));
	u = if(0.5.coin) {
		u * [1 - saw, saw.reverse].choose
	} {
		u * LFSaw.kr(freq * 0.1, 0, 0.1, 1)
	};

	u.clip2(1.0)

};

g = { | func, n=5 |
	n.do { func = func <> func };
	func
};

play {
	var freq;
	freq = g.(f, 4).value(LFSaw.kr(0.2 * [1, 1.1])).exprange(6, 11000);
	BPF.ar(Saw.ar(freq).product, [70, 800, 9000, 5242], 0.2).sum.dup * 0.3
}
);
