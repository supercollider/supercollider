//////////////////////////////////////////////////////////////////////////

(
// analog bubbles
{
	f = LFSaw.kr(0.4, 0, 24, LFSaw.kr([8,7.23], 0, 3, 80)).midicps; // glissando function
	CombN.ar(SinOsc.ar(f, 0, 0.04), 0.2, 0.2, 4) // echoing sine wave
}.play)

//////////////////////////////////////////////////////////////////////////

(
// LFO modulation of Pulse waves and resonant filters
play({
	CombL.ar(
		RLPF.ar(LFPulse.ar(FSinOsc.kr(0.05,0,80,160),0,0.4,0.05),
		   FSinOsc.kr([0.6,0.7],0,3600,4000), 0.2),
		0.3, [0.2,0.25], 2)
}))

//////////////////////////////////////////////////////////////////////////

(
// moto rev
{
	RLPF.ar(LFPulse.ar(SinOsc.kr(0.2, 0, 10, 21), 0.1), 100, 0.1).clip2(0.4)
}.play)

//////////////////////////////////////////////////////////////////////////

(
// scratchy
play({  RHPF.ar(BrownNoise.ar([0.5,0.5], -0.49).max(0) * 20, 5000, 1)  })
)

//////////////////////////////////////////////////////////////////////////

(
// sprinkler
play({
	BPZ2.ar(WhiteNoise.ar(LFPulse.kr(LFPulse.kr(0.09, 0, 0.16, 10, 7), 0, 0.25, 0.1)))
}))

(
play({
	BPZ2.ar(WhiteNoise.ar(LFPulse.kr(MouseX.kr(0.2,50), 0, 0.25, 0.1)))
}))

//////////////////////////////////////////////////////////////////////////

(
// harmonic swimming
play({
	var f, p, z, offset;
	f = 50;		// fundamental frequency
	p = 20;		// number of partials per channel
	z = 0.0;		// start of oscil daisy chain
	offset = Line.kr(0, -0.02, 60); // causes sound to separate and fade
	p.do({ arg i;
		z = FSinOsc.ar(
				f * (i+1), 		// freq of partial
				0,
				max(0, 			// clip negative amplitudes to zero
					LFNoise1.kr(
						6 + [4.0.rand2, 4.0.rand2],	// amplitude rate
						0.02, 					// amplitude scale
						offset					// amplitude offset
					)
				),
				z
		)
	});
	z
}))

//////////////////////////////////////////////////////////////////////////

(
// harmonic tumbling
play({
	var f, p, z, trig;
	f = 80;	// fundamental frequency
	p = 10;	// number of partials per channel
	z = 0.0;	// start of oscil daisy chain
	trig = XLine.kr([10,10], 0.1, 60);	// trigger probability decreases over time
	p.do({ arg i;
		z = FSinOsc.ar(
				f * (i+1), 		// freq of partial
				0,
				Decay2.kr(
					Dust.kr(
						trig,		// trigger rate
						0.02		// trigger amplitude
					),
					0.005, 	// grain attack time
					0.5.rand	// grain decay time
				),
				z
		)
	});
	z
}))

//////////////////////////////////////////////////////////////////////////

(
{
// Klank - bank of resonators excited by impulses
var n, p, z;
n = 5;	// number of simultaneous instruments
p = 15;	// number of partials per instrument
	z = `[	// filter bank specification :
				Array.fill(p, { 80 + 10000.0.linrand} ),	// frequencies
				Array.fill(p, { 1.0.rand2 }), 			// amplitudes
				Array.fill(p, { 0.2 + 8.0.rand } )		// ring times
			];
		Pan2.ar(
			Klank.ar(z, Dust.ar(0.7, 0.04)),
			1.0.rand2
		)
}.play;
)

//////////////////////////////////////////////////////////////////////////

(
{
// Klank - excited by noise bursts
var n, p;
var exciter, spec;
n = 5;	// number of simultaneous instruments
p = 8;	// number of partials per instrument
		exciter = Decay.ar(Dust.ar(0.6, 0.001), 3.1, WhiteNoise.ar);
		spec = Array.fill(2, {
			`[
				Array.fill(p, { 80 + 10000.0.linrand} ),
				nil,
				Array.fill(p, { 0.2 + 4.0.rand } )
			]
		});
		Klank.ar(spec, exciter)
}.play;
)

//////////////////////////////////////////////////////////////////////////

(
// what was I thinking?
{
	z = RLPF.ar(
		Pulse.ar(
			max( SinOsc.kr(4, 0, 1, 80),
				Decay.ar(LFPulse.ar(0.1, 0, 0.05, Impulse.ar(8, 0, 500)), 2)
			),
			LFNoise1.kr(0.157, 0.4, 0.5),
			0.04),
		LFNoise1.kr(0.2, 2000, 2400),
		0.2);
	y = z * 0.6;
	z +  [
			  CombL.ar(y, 0.06, LFNoise1.kr(0.3.rand, 0.025, 0.035), 1)
			+ CombL.ar(y, 0.06, LFNoise1.kr(0.3.rand, 0.025, 0.035), 1)
		,
			  CombL.ar(y, 0.06, LFNoise1.kr(0.3.rand, 0.025, 0.035), 1)
			+ CombL.ar(y, 0.06, LFNoise1.kr(0.3.rand, 0.025, 0.035), 1)
		]
}.play;
)

//////////////////////////////////////////////////////////////////////////

(
// police state
var n;
n = 4;	// number of sirens
play({
	CombL.ar(
		Mix.arFill(n, {
			Pan2.ar(
				SinOsc.ar(
					SinOsc.kr(0.1.rand + 0.02, 2pi.rand, 600.rand, 1000 + 300.rand2),
					0,
					LFNoise2.ar(100 + 20.0.rand2, 0.1)
				),
				1.0.rand2
			)
		})
		+ LFNoise2.ar(LFNoise2.kr([0.4,0.4], 90, 620), LFNoise2.kr([0.3,0.3], 0.15, 0.18)),
		0.3, 0.3, 3)
}))

//////////////////////////////////////////////////////////////////////////

(
{
// cymbalism
var p;
		var z, f1, f2;
p = 15;	// number of partials per channel per 'cymbal'.
		f1 = 500 + 2000.0.rand;
		f2 = 8000.0.rand;
		z = Array.fill(2, {
			`[	// sine oscil bank specification :
				y = Array.fill(p, { f1 + f2.rand} ), // frequencies
				nil, 							// amplitudes default to 1.0
				Array.fill(p, { 1.0 + 4.0.rand })	// ring times
			]
		});
		Klank.ar(z, Decay.ar(Impulse.ar(3.0.rand + 0.5), 0.004, WhiteNoise.ar(0.03)));
}.play;
)

//////////////////////////////////////////////////////////////////////////

(
// synthetic piano
var n;
n = 6;	// number of keys playing
play({
	Mix.ar(Array.fill(n, {	// mix an array of notes
		var delayTime, pitch, detune, strike, hammerEnv, hammer;

		// calculate delay based on a random note
		pitch = (36 + 54.rand);
		strike = Impulse.ar(0.1+0.4.rand, 2pi.rand, 0.1); // random period for each key
		hammerEnv = Decay2.ar(strike, 0.008, 0.04); // excitation envelope
		Pan2.ar(
			// array of 3 strings per note
			Mix.ar(Array.fill(3, { arg i;
				// detune strings, calculate delay time :
				detune = #[-0.05, 0, 0.04].at(i);
				delayTime = 1 / (pitch + detune).midicps;
				// each string gets own exciter :
				hammer = LFNoise2.ar(3000, hammerEnv); // 3000 Hz was chosen by ear..
				CombL.ar(hammer,		// used as a string resonator
					delayTime, 		// max delay time
					delayTime,			// actual delay time
					6) 				// decay time of string
			})),
			(pitch - 36)/27 - 1 // pan position: lo notes left, hi notes right
		)
	}))
})
)

//////////////////////////////////////////////////////////////////////////

(
var a, c, d, s, z, y;
// reverberated sine percussion
d = 6; // number of percolators
c = 5; // number of comb delays
a = 4; // number of allpass delays

play({
		// sine percolation sound :
	s = Mix.ar(Array.fill(d, { Resonz.ar(Dust.ar(2/d, 50), 200 + 3000.0.rand, 0.003)}) );

		// reverb predelay time :
	z = DelayN.ar(s, 0.048);

		// 7 length modulated comb delays in parallel :
	y = Mix.ar(CombL.ar(z, 0.1, LFNoise1.kr(Array.fill(c,{0.1.rand}), 0.04, 0.05), 15));

		// chain of 4 allpass delays on each of two channels (8 total) :
	a.do({ y = AllpassN.ar(y, 0.050, [0.050.rand,0.050.rand], 1) });

		// add original sound to reverb and play it :
	s+(0.2*y)
}))

//////////////////////////////////////////////////////////////////////////

(
// reverberated noise bursts
var a, c, d, s, z, y;
play({
		// pink noise percussion sound :
	s = Decay.ar(Dust.ar(0.6, 0.2), 0.15, PinkNoise.ar);

		// reverb predelay time :
	z = DelayN.ar(s, 0.048);

		// 6 modulated comb delays in parallel :
	y = Mix.ar(CombL.ar(z, 0.1, LFNoise1.kr(Array.fill(6,{0.1.rand}), 0.04, 0.05), 15));

		// chain of 4 allpass delays on each of two channels (8 total) :
	4.do({ y = AllpassN.ar(y, 0.050, [0.050.rand,0.050.rand], 1) });

		// add original sound to reverb and play it :
	s+y
}))

//////////////////////////////////////////////////////////////////////////

(
	// sample and hold liquidities
	// mouse x controls clock rate, mouse y controls center frequency
{
	var clockRate, clockTime, clock, centerFreq, freq, panPos, patch;

	clockRate = MouseX.kr(1, 200, 1);
	clockTime = clockRate.reciprocal;
	clock = Impulse.kr(clockRate, 0.4);

	centerFreq = MouseY.kr(100, 8000, 1);
	freq = Latch.kr(WhiteNoise.kr(centerFreq * 0.5, centerFreq), clock);
	panPos = Latch.kr(WhiteNoise.kr, clock);
	patch = CombN.ar(
			Pan2.ar(
				SinOsc.ar(
					freq,
					0,
					Decay2.kr(clock, 0.1 * clockTime, 0.9 * clockTime)
				),
				panPos
			),
			0.3, 0.3, 2
		);
	patch
}.play;
)

//////////////////////////////////////////////////////////////////////////

(
// sweepy noise - mouse controls LFO
{
	var lfoDepth, lfoRate, freq, filtered;
	lfoDepth = MouseY.kr(200, 8000, 'exponential');
	lfoRate = MouseX.kr(4, 60, 'exponential');
	freq = LFSaw.kr(lfoRate, 0, lfoDepth, lfoDepth * 1.2);
	filtered = RLPF.ar(WhiteNoise.ar([0.03,0.03]), freq, 0.1);
	CombN.ar(filtered, 0.3, 0.3, 2, 1, filtered);
}.play
)

//////////////////////////////////////////////////////////////////////////

(
{
		var a, a0, a1, b, c, pan;
		a0 = 200.0.rand + 40;
		a1 = a0 + 1.0.rand2;
		a = [a0, a1];
		b = 2000.0.rand;
		c = [a0 + 1.0.rand2, a1 + 1.0.rand2];
		SinOsc.ar(SinOsc.ar(a, 0, 1.0.rand * b, b), 0, SinOsc.kr(c, 0, 0.05, 0.05))
}.play;
)

//////////////////////////////////////////////////////////////////////////

(

// aleatoric quartet
// mouse x controls density

play({
	var amp, density, dmul, dadd, signal;
	amp = 0.07;
	density = MouseX.kr(0.01,1); // mouse determines density of excitation

	// calculate multiply and add for excitation probability
	dmul = density.reciprocal * 0.5 * amp;
	dadd = dmul.neg + amp;

	signal = Mix.ar(	// mix an array of 4 instruments
		Array.fill(4, {
			var excitation, freq;

			excitation = PinkNoise.ar(
				// if amplitude is below zero it is clipped
				// density determines the probability of being above zero
				max(0, LFNoise1.kr(8, dmul, dadd))
			);

			freq = Lag.kr(			// lag the pitch so it glissandos between pitches
				LFNoise0.kr(				// use low freq step noise as a pitch control
					[1, 0.5, 0.25].choose, 	// choose a frequency of pitch change
					7, 					// +/- 7 semitones
					66 + 30.rand2			// random center note
				).round(1), 		// round to nearest semitone
				0.2				// gliss time
			).midicps;			// convert to hertz

			Pan2.ar(	// pan each intrument
				CombL.ar(excitation, 0.02, freq.reciprocal, 3), // comb delay simulates string
				1.0.rand2		// random pan position
			);
	}));

	// add some reverb via allpass delays
	5.do({ signal = AllpassN.ar(signal, 0.05, [0.05.rand,0.05.rand], 1) });
	LeakDC.ar( signal, 0.995);		// delays build up a lot of DC, so leak it out here.
})
)

////////////////////////////////////////////////////////////////////////////////////////////////

(
{
		var mix, out, n=8;
		r = LFNoise0.kr(rrand(0.2,1.0), 1,1).squared * MouseX.kr;
		mix = Klank.ar(
			`[
				Array.fill(n, { exprand(1.0,20.0) }),
				nil,
				Array.fill(n, { 0.2.rand })
			],
			Blip.ar(r, [rrand(2,5),rrand(2,5)], 0.1)
		).fold2(0.2).cubed * 12;
		mix = Mix.arFill(3, { CombL.ar(mix, 0.1, 0.03.linrand, 4.0.linrand) });
		out = mix.distort * 0.5;
		6.do({ out = AllpassN.ar(out, 0.05, [0.05.rand, 0.05.rand], 3) });
		out = LeakDC.ar(out);
}.play
)

////////////////////////////////////////////////////////////////////////////////////////////////

(
{
	var root, scale;

	// struck string
	var trig, p, exc, x, freq, s;
	root = rrand(3,6);
	scale = #[0,2,4,5,7,9,11];

	freq = (scale.choose + #[24,36,48,60,72,84].choose + root).midicps;
	exc = BrownNoise.ar(
		Decay2.kr(
			Impulse.kr(#[0.125,0.25,0.375,0.5,0.75,1,1.5,2,3,4].choose,0,[0.05,0.05]
		), 0.005, 0.05));
	s = (Klank.ar(`[
			Array.series(16, freq, freq),
			Array.geom(16,1,rrand(0.7,0.999)),
			Array.fill(16, {rrand(0.1,2.5)})
		], exc) * 0.1).softclip;
}.play
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{
	var root, scale, s;
			// wind chords
	var trig, p, n, exc, x, base, range;
		//root = rrand(3,6);
		root = 5;
		scale = #[0,2,4,5,7,9,11];
		n = 5;
		exc = BrownNoise.ar([0.007,0.007]) * max(0, LFNoise1.kr(exprand(0.125,0.5), 1.25, -0.25));
		s = (Klank.ar(`[Array.fill(n, {(scale.choose + #[36,48,60,72,84].choose + root).midicps}),nil,Array.fill(n, {rrand(0.1,2.0)})], exc) * 0.1).softclip;
}.play;
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{

			// wind metals
			var trig, s, p, n, exc, x, base, range;
			n = 6;
			base = exprand(60,4000);
			range = rrand(500.0,8000.0);
			exc = BrownNoise.ar([0.007,0.007]) * max(0, LFNoise1.kr(exprand(0.125,0.5), 0.75, 0.25));
			s = (Klank.ar(`[Array.fill(n, {linrand(range)+base}),nil,Array.fill(n, {rrand(0.1,2.0)})], exc) * 0.1).softclip;
}.play;
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{
var root, scale;
			// bowed string
		var trig, p, s, exc, x, freq;
		//root = rrand(3,6);
		root = 5;
		scale = #[0,2,4,5,7,9,11];
			freq = (scale.choose + #[24,36,48,60,72,84].choose + root).midicps;
			exc = BrownNoise.ar([0.007,0.007]) * max(0, LFNoise1.kr(exprand(0.125,0.5), 0.6, 0.4));
			s = (Klank.ar(`[
					Array.series(12, freq, freq),
					Array.geom(12,1,rrand(0.7,0.9)),
					Array.fill(12, {rrand(1.0,3.0)})
				], exc) * 0.1).softclip;
}.play;
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{
	var root, scale, s;
	// whistle
	var trig, p, exc, x, freq;
		root = 5;
		scale = #[0,2,4,5,7,9,11];
			freq = (scale.choose + #[84,96].choose + root).midicps;
			exc = BrownNoise.ar([0.05,0.05]) * max(0, SinOsc.kr(exprand(0.125,0.5), 2pi.rand));
			s = (Klank.ar(`[
					Array[freq],
					nil,
					Array[0.3]
				], exc) * 0.1).softclip;
}.play
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{

	// metallic
	var trig, p, exc, x, s;
	exc = BrownNoise.ar(Decay2.kr(Impulse.kr(#[0.125,0.25,0.375,0.5,0.75,1,1.5,2].choose,0,[0.04,0.04]), 0.005, 0.05));
	s = (Klank.ar(`[Array.fill(16, {linrand(8000.0)+60}),nil,Array.fill(16, {rrand(0.1,2.0)})], exc) * 0.1).softclip;
}.play
)

///////////////////////////////////////////////////////////////////////////////////////////////

(
{
	var x, r, f;
	x = Mix.fill(4, {
		LFSaw.ar((LFPulse.kr(0.06,0,0.5,2,34 + {0.1.rand2}.dup)).midicps, 0, 0.01)
	});
	r = MouseY.kr(0.1,0.7);
	f = LinExp.kr(SinOsc.kr(0.07),-1,1,300,5000);
	x = RLPF.ar(x, f, r).softclip;
	x = RLPF.ar(x, f, r).softclip;
}.play;
)

(
{
Pan2.ar(
	SinOsc.ar(rrand(24,108).midicps, 2pi.rand,

		Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.1)).max(0)
		* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.1)).abs
	), 1.0.rand2);

}.play;
)


(
{
		Pan2.ar(
	HPZ1.ar(LFTri.ar(rrand(24,128).midicps, 2pi.rand,

		Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.04)).max(0)
		* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs
	)), 1.0.rand2);
}.play;
)

(
{

		x = Pan2.ar(
		LPZ2.ar(LFPulse.ar(80 * rrand(1,32), 2pi.rand, 0.1,

			Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.04)).max(0)
			* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs

		)), 1.0.rand2);
		2.do({ x = AllpassN.ar(x, 0.05, [0.05.rand,0.05.rand], 3.0.rand, 2); });
		x

}.play
)


(
{
		x = Pan2.ar(
		LPZ2.ar(LFPulse.ar(80 * (1 + 32.linrand), 2pi.rand, 0.1,

			Mix.ar(SinOsc.ar(0.125 * [2**rrand(0,6),2**rrand(0,6)], [2pi.rand,2pi.rand], 0.04)).max(0)
			* Mix.ar(SinOsc.ar(4 * [rrand(1,6),rrand(1,6)], [2pi.rand,2pi.rand])).abs

		)), 1.0.rand2);
		2.do({ x = AllpassN.ar(x, 0.05, [0.05.rand,0.05.rand], 4.0.rand, 2); });
		x

	}.play;
)

(
{

		Pan2.ar(
	BPZ2.ar(LFPulse.ar(120 * 100.rand, 2pi.rand, 0.5,

		Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.04)).max(0)
		* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs
	)), 1.0.rand2);
}.play
)

(
{

		Pan2.ar(
	Resonz.ar(LFPulse.ar(exprand(50,600), 2pi.rand, 0.5,

		Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.1)).max(0)
		* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs
	), exprand(100,2000), 0.2), 1.0.rand2);
}.play
)

(
{

		Pan2.ar(
	RLPF.ar(LFPulse.ar(exprand(50,600), 2pi.rand, 0.5,

		Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.1)).max(0)
		* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs
	), exprand(100,2000), 0.2), 1.0.rand2);
}.play
)

{  SinOsc.ar(exprand(100,6000), 0, LFNoise2.ar(exprand(4,24),[0.07,0.07])) }.play;

(
{

		Pan2.ar(
		RLPF.ar(LFPulse.ar(exprand(50,600), 2pi.rand, 0.5,

			Mix.ar(SinOsc.ar(exprand(0.3,8) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand], 0.1)).max(0)
			* Mix.ar(SinOsc.ar(exprand(6,24) * [rrand(0.7,1.3),1], [2pi.rand,2pi.rand])).abs
		), exprand(100,2000), 0.2), 1.0.rand2);
}.play;
)

/////////////////////////////////////////////////////////////////////////////////


(
{
		var freq, x;
		freq = LFPulse.kr(20.0.rand, 0, 1.0.rand,
			LFPulse.kr(4.0.rand, 0, 1.0.rand, 8000.rand, 2000.rand));
		freq = freq + LFPulse.kr(20.0.rand, 0, 1.0.rand,
			LFPulse.kr(4.0.rand, 0, 1.0.rand, 8000.rand, 2000.rand));
		x = LFPulse.ar(freq+[0.5,0.5].rand2, 0, 0.5, 0.15, -0.05);
		AllpassN.ar(x, 0.05, [0.05,0.05].rand, 0.3);
}.play
)

(
{
	var out, lfoDepth, lfoRate, freq, filtered;
	lfoDepth = MouseY.kr(200, 8000, 1);
	lfoRate = MouseX.kr(4, 60, 1);
	freq = LFSaw.kr(lfoRate, 0, lfoDepth, lfoDepth * 1.2);
	filtered = RLPF.ar({WhiteNoise.ar(0.03)}.dup, freq, 0.1);
	out = CombN.ar(filtered, 0.3, 0.3, 2, 1, filtered);
	out
}.play;
)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// adding many things to the top level spawn.
(
{
	var exc, amp;
	amp = LFPulse.kr(4,0,0.1,0.002);
	exc = LPZ1.ar(GrayNoise.ar([amp,amp]));
	Klank.ar(`[FloatArray.fill(4, { rrand(80.0,400.0) }),
			nil,
			FloatArray[1, 1, 1, 1]], exc);
}.play;
)

(
{
	var exc, amp;
	amp = LFPulse.kr(8,0,0.1,0.002);
	exc = LPZ1.ar(GrayNoise.ar([amp,amp]));
	Klank.ar(`[FloatArray.fill(4, { rrand(80.0,400.0) }),
			nil,
			FloatArray[1, 1, 1, 1]], exc).abs;
}.play;
)

(
{
	CombN.ar(LPF.ar(LFNoise0.ar(MouseX.kr([300,2200])*[1,1.1],LFPulse.kr(1,0,0.3,0.1)), 800).abs, 0.2, [0.2,0.17], 5);
}.play;
)

(
{
	var amp, my;
	amp = LFPulse.kr(0.5,0,0.5);
	my = MouseY.kr(400,3200);
	Mix.arFill(8, {
		var x;
		x = Formlet.ar(Dust.ar(12,[0.05,0.05]) * amp, my * exprand(0.5,2.0), 0.005, 0.1);
		AllpassN.ar(x, 0.05,0.05.rand, 8);
	});
}.play;
)
