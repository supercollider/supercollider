(
//a study for recreating key (sound) aspects of data.microhelix by Ryoji Ikeda
//Batuhan Bozkurt 2009 http://www.earslap.com
{
	//rhythm logic: very crude, times are 1/8's mixed with 1/4's (equal probability) plus 1/16.
	//when 1/16 is chosen, we make sure they come in doubles so it sounds rhytmically nicer.
	//we create 2 instances of these trigs so glitching sound will be independent between speakers.
	//we use TDuty with Demand UGens to create this kind of sophisticated triggers (easier and more
	//flexible dealing with pure noise generators).
	var ctrigs =
	{
		TDuty.ar
		(
			Dxrand(((1/8!8) ++ (1/4!8) ++ [Dseq([1/16, 1/16], Drand([1, 2], inf))]) * 1.25, inf), //see above logic
			0,
			Dwhite(0.5, 1, inf) //trigger values are between 0.5, 1
		)
	}!2;

	//the glitch/clicking noises: basically enveloped PinkNoise through bandpass filter with high rq (low q)
	//here I'm modulating the decay time of the noise envelope with a LFNoise1.
	//there are many diferent ways of approaching this, needs experimentation,
	//dynamic center freq, rq etc. can be employed for example. This sounded nice to me, however.

	var clicks =
		BPF.ar
		(
			PinkNoise.ar(Decay.ar(ctrigs, 0.001 * LFNoise1.ar(4).abs)), //decay time is modulated
			15000,
			0.9,
			25 * LFNoise1.ar(8).range(0, 1) //extreme amplification of glitches.
		).fold(-1, 1); //folding them back into [-1, 1] range (foldback distortion). I think it sounds nice.

	//i don't really know what to call this sound, so its names snd1. 2 sine oscillators tuned by ear, with filtered
	//background noise. nothing fancy. we will random-pan and envelope this later on.
	var snd1 =
		LPF.ar
		(
			SinOsc.ar(44.midicps, 0, 0.5) + SinOsc.ar(90.midicps, 0, 0.6),
			32.midicps * 2
		) + HPF.ar(LPF.ar(WhiteNoise.ar(0.008), 12000), 2400);

	//the shaker-like sound in the background. in the original piece, the rhytmical logic for this sound
	//is a lot more different, I'm using the same ctrigs here, for the sake of simplicity (and I'm a bit lazy).
	//its basically an enveloped WhiteNoise through a bandpass filter with a random freq
	var hiNoise =
		BPF.ar
		(
			WhiteNoise.ar(Decay2.ar(ctrigs * LFNoise1.ar(8, 0.5, 0.5), 0.02, 0.1) * 0.05),
			TRand.ar(12000, 15000, ctrigs),
			0.9
		);

	//the bass/kicky sound needs a lot of tuning (sound and envelope, some best be left to post processing maybe)
	//2 sine oscillators, one has a low freq nad the other is an upper harmonic (quieter)
	//the original piece uses a completely different (and very better) rhytmical logic for this sound.
	//I'm using our ctrigs impulse generator here (first channel) for the sake of clarity.
	//but since I don't want every trigger to activate this, I'm multiplying the trigger with
	//a noise generator (rounded, so its -1, 0, or 1). so not all trigs pass through.

	//the sine oscillator uses a trick I've learned from Josh Parmenter. the freq arguments of the
	//sine oscillators are set to zero, and they are driven by the phase argument with a sweep.
	//this makes the oscillator phase resettable which is very useful for having uniform attacks
	//for low frequency kick-like sounds. or else, the envelope sounding the oscillators can kick
	//in at any phase and the attack of the kicks will vary (in a bad way). here I reset the phase
	//of the sine at each trigger. the speed of the phase driver ([52.8, 740]) determines the
	//frequency of the oscillator here. I'm adding a phase of pi/3 to it so the phase resets to
	//pi/3 instead of zero which produces an additional glitch. this is all about glitch after all right?

	var trigMod = LFNoise0.ar(8).round;

	var bass =
		SinOsc.ar(0, (Sweep.ar(ctrigs[0] * trigMod, 2pi * [52.8, 740]) + (pi/3)).wrap(-pi, pi), [2, 0.05]).mean.tanh *
		EnvGen.ar(Env([0, 0.5, 0.4, 0], [0, 0.2, 0.01], -5), (ctrigs[0] * trigMod).abs)!2;

	//panning and enveloping of snd1. I don't want to use every trigger for the envelope of this so
	//i again multiply it with a random generator. not all trigs pass through. and some random panning.
	snd1 =
		Pan2.ar
		(
			snd1 * EnvGen.ar(Env([0, 1, 0.6, 0], [0.0001, 0.4, 0.01]), ctrigs * LFNoise0.ar(8)),
			TRand.ar(-1, 1, ctrigs)
		);

	//i'm using limiter here because i'm lazy. one needs to get the
	//gain scheduling right for a better sound. this especially kills the bass
	//sometimes. I'm also boosting the freqs around 15000Hz.
	//delete some sounds from the input of MidEQ to hear the sounds
	//(glitches, bass etc) in isolation.
	Limiter.ar(MidEQ.ar(clicks + snd1 + hiNoise + bass, 14000, 0.7, 8));
}.play;
)
