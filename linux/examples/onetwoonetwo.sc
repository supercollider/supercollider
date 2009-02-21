s = Server.local;

"SC_JACK_DEFAULT_OUTPUTS".setenv("system:playback_1,system:playback_2");

s.waitForBoot({
	SynthDef("onetwoonetwo", {
		var freqs, cycle, freq, rez, cut, sig;
		freqs = #[30, 40, 42, 38, 60, 40, 68, 50];
		cycle = 0.5 * freqs.size;
		freq = Select.kr(LFSaw.kr(0.6,0,cycle,cycle),freqs);
		rez = SinOsc.kr(0.1,0,0.8,1);
		cut = SinOsc.kr(0.1,0.5,400,800);
		sig = Mix(CombN.ar(RLPF.ar(Saw.ar([freq.midicps, (freq+7.001).midicps],mul:0.5),cut,rez),1.2,1.2,3));
		sig = Pan2.ar(sig, SinOsc.kr(0.6,mul:0.2));
		Out.ar(0, sig);
	}).send(s, ["/s_new", "onetwoonetwo", -1, 1, s.asTarget.nodeID]);
});

// EOF