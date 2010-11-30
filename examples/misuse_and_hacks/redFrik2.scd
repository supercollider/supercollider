
// /*RUN*/
(
s.waitForBoot{
	var synth;
	SystemClock.sched(0, {
		synth.free;
		synth= SynthDef("redFrik",{
			z=RHPF.ar(
				BrownNoise.ar(1.dup),
				Rand(0.3,3),
				FSinOsc.kr(FSinOsc.kr(Rand(0.3,5),0,Rand(0,0.5),Rand(0.49,0.56)),Rand(0.3,0.6),Rand(0.3,0.5))
			);
			Out.ar(0,z);
		}).play;
		10;
	});
}
)