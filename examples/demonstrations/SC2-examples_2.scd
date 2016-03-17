

//////////////////////////////////////////////////////////////////////////

(
// analog bubbles - with mouse control
play({
	var freq;
	freq = LFSaw.kr(
		MouseY.kr(0.1,10,'exponential'),	// lfo 1 rate
		24, 							// lfo 1 depth in semitones
		// lfo 2 in lfo 1's add input :
		LFSaw.kr(
			MouseX.kr(2,40,'exponential'),	// lfo 2 rate
			-3, 80					// lfo 2 depth & offset in semitones
		)
	).midicps; // convert to frequency
	CombN.ar(SinOsc.ar(freq, 0, 0.04), 0.2, 0.2, 2) // echoing sine wave
})
)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

(
	// sample and hold liquidities
	// mouse x controls clock rate, mouse y controls center frequency
{
	var clockRate, clockTime, clock, centerFreq, freq, panPos, patch;

	clockRate = MouseX.kr(1, 200, 'exponential');
	clockTime = clockRate.reciprocal;
	clock = Impulse.kr(clockRate, 0.4);

	centerFreq = MouseY.kr(100, 8000, 'exponential');
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
}.play
)

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// distort input
{
	var gain;
	gain = MouseX.kr(1,100,'exponential');	// mouse x controls gain into distortion
	AudioIn.ar([1,2], gain).distort * 0.4
}.play))

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// ring modulate input
{
	var input, modulator;
	input = AudioIn.ar([1,2]);
	modulator = SinOsc.ar(
				MouseX.kr(10,4000,'exponential'),	// mouse x controls ring mod freq
				[0,0.5pi]						// offset phase ofone osc by 90 degrees
			);
	input * modulator
}.play
)

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// ring modulate input using ring1
{
	var input, modulator;
	input = AudioIn.ar([1,2], 0.5);
	modulator = SinOsc.ar(
				MouseX.kr(10,4000,'exponential'),	// mouse x controls ring mod freq
				[0,0.5pi]						// offset phase ofone osc by 90 degrees
			);
	input ring1: modulator
}.play
)

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// filter the input
{
	var rQ;
	rQ = MouseY.kr(0.01, 1, 'exponential'); // bandwidth ratio = 1/Q
	RLPF.ar(
		AudioIn.ar([1,2], 0.4 * rQ.sqrt),		// attenuate to offset resonance
		MouseX.kr(100, 12000, 'exponential'), 	// mouse x controls cutoff freq
		rQ
	)
}.play
)

//////////////////////////////////////////////////////////////////////////



(
// input limiter
{
	CompanderD.ar(
		AudioIn.ar([1,2]),
		MouseX.kr(0.01, 0.5), 		// threshold
		1, 		// below threshold slope
		0.1		// above threshold slope
	)
}.play
)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

(
// input noise gate
{
	var input;
	input = AudioIn.ar([1,2]);
	Compander.ar(
		input, input,
		MouseX.kr(0.01, 0.5), 		// threshold
		10, 		// below threshold slope
		1		// above threshold slope
	)
}.play
)

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// echo input
{
	var in;
	in = AudioIn.ar([1,2], 0.1);
	CombL.ar(
		in,
		0.5, 				// max delay time
		MouseX.kr(0,0.5),	// mouse x controls delay time
		4,				// echo 60 dB decay time in seconds
		1, 				// scale by unity
		in				// mix with input
	)
}.play
)

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

(
// ring modulate & echo input
{
	var in;
	in = AudioIn.ar([1,2], 0.4) * SinOsc.ar(MouseX.kr(10,2000,'exponential'), [0,0.5pi]);
	CombL.ar(
		in,
		0.5,
		MouseY.kr(0,0.5),	// mouse y controls delay time
		4,				// echo 60 dB decay time in seconds
		1, 				// scale by unity
		in				// mix with input
	)
}.play
)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

(
// ring modulated and resonant filtered input
{
	var input, modulator;
	input = AudioIn.ar([1,2],0.2);
	modulator = SinOsc.ar(
				MouseX.kr(10,4000,'exponential'),	// mouse x controls ring mod freq
				[0,0.5pi]						// offset phase ofone osc by 90 degrees
			);
	RLPF.ar(
		input * modulator, 					// do ring modulation
		MouseY.kr(100, 12000, 'exponential'), 	// mouse y controls cutoff freq
		0.1)								// bandwidth ratio = 1/Q
}.play
)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

(
// distort, ring modulate & echo input - a real noise fest
{
	var in;
	in = AudioIn.ar([1,2], 20).distort.ring1(
			SinOsc.ar(MouseX.kr(10,2000,'exponential'), [0,0.5pi])
		) * 0.02;
	CombL.ar(
		in,
		0.5,
		MouseY.kr(0,0.5), // mouse y controls delay time
		4,
		1,
		in
	)
}.play
)

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

(
// sweep verb
{
	var s, z, y;

	s = AudioIn.ar([1,2], 0.01) ;

		// reverb predelay time :
	z = DelayN.ar(Mix.ar(s), 0.048);

		// 6 modulated comb delays in parallel :
	y = Mix.ar(CombL.ar(z, 0.1, LFNoise1.kr(Array.fill(6,{0.1.rand}), 0.04, 0.05), 15));

		// chain of 4 allpass delays on each of two channels (8 total) :
	4.do({ y = AllpassN.ar(y, 0.050, [0.050.rand,0.050.rand], 1) });

	// eliminate DC
	LeakDC.ar(y)
}.play
)


//////////////////////////////////////////////////////////////////////////

(
// monastic resonance
// mouse controls size and reverb time
{
	var s, z, y, decayTime, delayScale;

	decayTime = MouseX.kr(0,16);
	delayScale = MouseY.kr(0.01, 1);

	s = AudioIn.ar([1,2], 0.005) ;

		// reverb predelay time :
	z = DelayN.ar(Mix.ar(s), 0.048);

		// 8 comb delays in parallel :
	y = Mix.ar(CombL.ar(z, 0.1, Array.fill(8,{0.04.rand2 + 0.05}) * delayScale, decayTime));

		// chain of 5 allpass delays on each of two channels (10 total) :
	5.do({ y = AllpassN.ar(y, 0.050, [0.050.rand,0.050.rand], 1) });

	// eliminate DC
	LeakDC.ar(y)
}.play
)


//////////////////////////////////////////////////////////////////////////

(
// noise burst sweep
{
	var lfoRate, amp, cfreq, filt;

	lfoRate = MouseX.kr(10, 60, 'exponential');
	amp = max(0, LFSaw.kr(lfoRate, -1));
	cfreq = MouseY.kr(400, 8000, 'exponential');
	cfreq = SinOsc.kr(0.2, 0, cfreq, 1.05 * cfreq);
	Resonz.ar(WhiteNoise.ar(amp), cfreq, 0.1) ! 2;
}.play
)

//////////////////////////////////////////////////////////////////////////



(
// aleatoric quartet
// mouse x controls density
{
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
}.play;
)





//////////////////////////////////////////////////////////////////////

(
    // strummable guitar
    // use mouse to strum strings
{
	var pitch, mousex, out;
	// e a d g b e
	pitch = [ 52, 57, 62, 67, 71, 76 ];
	mousex = MouseX.kr;
	out = Mix.arFill(pitch.size, { arg i;
		var trigger, pluck, period, string;
		// place trigger points from 0.25 to 0.75
		trigger = HPZ1.kr(mousex > (0.25 + (i * 0.1))).abs;
		pluck = PinkNoise.ar(Decay.kr(trigger, 0.05));
		period = pitch.at(i).midicps.reciprocal;
		string = CombL.ar(pluck, period, period, 4);
		Pan2.ar(string, i * 0.2 - 0.5);
	});
	LPF.ar(out, 12000);
	LeakDC.ar(out);
}.play;
)




//////////////////////////////////////////////////////////////////////

(
    // strummable 12 string guitar
    // use mouse to strum strings
{
	var pitch, mousex, out;
	// e a d g b e
	pitch = [ 52, 57, 62, 67, 71, 76 ];
	mousex = MouseX.kr;
	out = Mix.arFill(pitch.size, { arg i;
		var pos, trigger1, trigger2, pluck1, pluck2, period, string1, string2;
		// place trigger points from 0.25 to 0.75
		pos = 0.25 + (i * 0.1);
		period = pitch.at(i).midicps.reciprocal;

		trigger1 = HPZ1.kr(mousex > pos).abs;
		pluck1 = PinkNoise.ar(Decay.kr(trigger1, 0.05));
		string1 = CombL.ar(pluck1, period, period, 4);

		trigger2 = HPZ1.kr(mousex > (pos + 0.015)).abs;
		pluck2 = PinkNoise.ar(Decay.kr(trigger2, 0.05));
		string2 = CombL.ar(pluck2, period/2, period/2, 4);

		Pan2.ar(string1 + string2, i * 0.2 - 0.5);
	});
	LPF.ar(out, 12000);
	LeakDC.ar(out);
}.play;
)

//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////

(
    // bidirectional strummable guitar
    // use mouse to strum strings
{
	var pitch1, pitch2, mousex, out;
	// e a d g b e
	pitch1 = [ 52, 57, 62, 67, 71, 76 ];
	pitch2 = pitch1 + 7;
	mousex = MouseX.kr;
	out = Mix.arFill(pitch1.size, { arg i;
		var trigger, pluck1, pluck2, period1, period2, string1, string2;
		// place trigger points from 0.25 to 0.75
		trigger = HPZ1.kr(mousex > (0.25 + (i * 0.1)));

		pluck1 = PinkNoise.ar(Decay.kr(trigger.max(0), 0.05));
		period1 = pitch1.at(i).midicps.reciprocal;
		string1 = CombL.ar(pluck1, period1, period1, 4);

		pluck2 = BrownNoise.ar(Decay.kr(trigger.neg.max(0), 0.05));
		period2 = pitch2.at(i).midicps.reciprocal;
		string2 = CombL.ar(pluck2, period2, period2, -4);

		Pan2.ar(string1 + string2, i * 0.2 - 0.5);
	});
	LPF.ar(out, 12000);
	LeakDC.ar(out);
}.play;
)

//////////////////////////////////////////////////////////////////////

(
    // harmonic zither
    // use mouse to strum strings
{
	var pitch, mousex, mousey, out, triggerSpacing, panSpacing;
	// harmonic series
	pitch = [ 50, 53.86, 57.02, 59.69, 62, 64.04, 65.86, 67.51, 69.02, 71.69, 72.88, 74 ];
	mousex = MouseX.kr;
	mousey = MouseY.kr;
	triggerSpacing = 0.5 / (pitch.size - 1);
	panSpacing = 1.5 / (pitch.size - 1);
	out = Mix.arFill(pitch.size, { arg i;
		var trigger, pluck, period, string;
		// place trigger points from 0.25 to 0.75
		trigger = HPZ1.kr(mousex > (0.25 + (i * triggerSpacing))).abs;
		pluck = PinkNoise.ar(Decay.kr(trigger, 0.05));
		period = pitch.at(i).midicps.reciprocal;
		string = CombL.ar(pluck, period, period, 8);
		Pan2.ar(string, i * panSpacing - 0.75);
	});
	LPF.ar(out, 12000);
	LeakDC.ar(out);
}.play;
)

//////////////////////////////////////////////////////////////////////

(
    // strummable metals
    // use mouse to strum strings
{
	var mousex, out;
	mousex = MouseX.kr;
	out = Mix.arFill(8, { arg i;
		var trigger, pluck, period, metal, z, n=15;
		// place trigger points from 0.25 to 0.75
		trigger = HPZ1.kr(mousex > (0.25 + (i * 0.07))).abs;
		pluck = PinkNoise.ar(Decay.kr(trigger, 0.05, 0.04));

		z = `[	// filter bank specification :
				y = Array.fill(n, { (300 * i) + 8000.0.linrand} ), // frequencies
				nil, 							// amplitudes default to 1.0
				Array.fill(n, { 1.0 + 4.0.rand })	// ring times
			];
		metal = Klank.ar(z, pluck);
		//metal = SinOsc.ar(800, 0, 0.1);
		Pan2.ar(metal, i * 0.2 - 0.5);
	});
	LPF.ar(out, 12000);
	LeakDC.ar(out);
}.play;
)

//////////////////////////////////////////////////////////////////////

(
    // strummable pipes
{
	var mousex, out;
	mousex = MouseX.kr;
	out = Mix.arFill(8, { arg i;
		var trigger, pluck, period, metal, z, n=15, freq;
		// place trigger points from 0.25 to 0.75
		trigger = HPZ1.kr(mousex > (0.25 + (i * 0.07))).abs;
		pluck = PinkNoise.ar(Lag.kr(Trig.kr(trigger, 1), 0.2, 0.01));
		freq = (#[0, 3, 5, 7, 10, 12, 15, 17].at(i) + 60).midicps;
		z = `[	// filter bank specification :
				y = Array.fill(n, { arg j; (j+1) * freq } ), // frequencies
				nil, 							// amplitudes default to 1.0
				Array.fill(n, { 0.2.rand })	// ring times
			];
		metal = Klank.ar(z, pluck);
		//metal = SinOsc.ar(800, 0, 0.1);
		Pan2.ar(metal, i * 0.2 - 0.5);
	});
	LPF.ar(out, 12000);
	out = LeakDC.ar(out);
	6.do({ out = AllpassN.ar(out, 0.1, [0.05.rand,0.05.rand], 4) });
	out
}.play;
)

//////////////////////////////////////////////////////////////////////
