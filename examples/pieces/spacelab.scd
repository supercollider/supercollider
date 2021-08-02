/*******************************************************************************
"Spacelab" -- Kraftwerk
old school exercise -- jy
*******************************************************************************/

s.boot;

// SynthDefs //
(
	SynthDef(\bd, { | out=0 |
		var osc, env;
		osc = FSinOsc.ar(40);
		env = EnvGen.kr(Env.perc(0, 0.05), doneAction: 2);
		Out.ar(out, Pan2.ar(osc, 0, env));
	}).add;

	SynthDef(\sd, { | out=0 |
		var osc1, osc2, env;
		osc1 = WhiteNoise.ar;
		osc2 = FSinOsc.ar(200);
		env = EnvGen.kr(Env.perc(0, 0.05), doneAction: 2);
		Out.ar(out, Pan2.ar(LPF.ar(Mix([osc1, osc2]), 12000), 0, env));
	}).add;

	SynthDef(\hat, { | out=0 |
		var osc1, env;
		osc1 = WhiteNoise.ar;
		env = EnvGen.kr(Env.perc(0, 0.01), doneAction: 2);
		Out.ar(out, Pan2.ar(osc1, 0, env));
	}).add;

	SynthDef(\res, { | out=0 gate=1 freq fltBus |
		var aEnv, osc, flt;
		aEnv = EnvGen.kr(Env.perc(0, 0.7), gate, doneAction: 2);
		osc = Mix([Saw.ar(freq), Pulse.ar(freq / 2, 0.5)]);
		flt = RLPF.ar(osc, In.kr(fltBus, 1), 0.1, aEnv);
		Out.ar(out, Pan2.ar(flt, 0));
	}).add;

	SynthDef(\resControl, { | fltBus |
		ReplaceOut.kr(fltBus, LFNoise1.kr(0.3, 1000, 1500));
	}).add;

	SynthDef(\poly, { | out=0 gate=1 freq |
		var aEnv, fEnv, osc1, osc2, flt;
		aEnv = EnvGen.kr(Env.asr(0.2, 1, 0.1), gate, doneAction: 2);
		fEnv = EnvGen.kr(Env.asr(7, 1, 0.2), levelScale: 12000);
		osc1 = Pulse.ar(freq * [1, 1.007], LFCub.kr(2, 0, 0.3, 0.5));
		osc2 = Pulse.ar(freq / 2, 0.3);
		flt = RLPF.ar(osc1 + osc2, fEnv, 0.9, aEnv);
		Out.ar(out, flt);
	}).add;

	SynthDef(\solo, { | out=0 gate freq |
		var aEnv, fEnv, osc, flt;
		aEnv = EnvGen.kr(Env.asr(0.2, 1, 2), gate);
		fEnv = EnvGen.kr(Env.adsr(0.4, 1.2, 0.7, 2), gate, 800);
		osc = Saw.ar(Lag.kr(freq, 0.1) * [1, 1.005]);
		flt = LPF.ar(osc, fEnv + 600, aEnv);
		Out.ar(out, flt);
	}).add;

	SynthDef(\soloControl, { | gateBus freqBus gate=1 freq |
		ReplaceOut.kr(gateBus, gate);
		ReplaceOut.kr(freqBus, freq);
		EnvGen.kr(Env.asr(0,1,0), gate, doneAction: 2);
	}).add;

	SynthDef(\string, { | out=0 gate=1 freq=1000 |
		var aEnv, osc, flt;
		aEnv = EnvGen.kr(Env.asr(0.2, 1, 0.5), gate, doneAction: 2);
		osc = Saw.ar([LFCub.kr(0.3, Rand(0, 1), freq * 0.003, freq), freq, LFCub.kr(0.7, Rand(0, 1), freq * 0.001, freq)]);
		flt = LPF.ar(osc, 1500, aEnv);
		Out.ar(out, flt);
	}).add;

	SynthDef(\bass, { | out=0 gate=1 freq |
		var aEnv, fEnv, osc, flt;
		aEnv = EnvGen.kr(Env.asr(0, 1, 1), gate, doneAction: 2);
		fEnv = EnvGen.kr(Env.perc(0, 3), levelScale: 6000);
		osc = Mix([Saw.ar(freq * [1, 1.005]), Pulse.ar(freq / 2, 0.5)]);
		flt = LPF.ar(osc, fEnv + 100, aEnv);
		Out.ar(out, flt);
	}).add;

	SynthDef(\intro, { | out=0 |
		var trig, div, freq, oct, reps, env, osc, flt;
		oct = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3];
		trig = Impulse.kr(EnvGen.kr(Env([1.75, 120], [30], 6.5)));
		div = PulseDivider.kr(trig, 16, 16);
		reps = PulseCount.kr(div);
		env = EnvGen.kr(Env.perc(0, 3), trig, 0.5);
		freq = (Stepper.kr(trig, div, 42, 74, 2, 42) + (12 * Select.kr(reps, oct))).midicps;
		osc = Saw.ar(freq * [1, 1.01], env);
		flt = RLPF.ar(osc, freq * 2, 0.7);
		Out.ar(out, flt);
	}).add;

	SynthDef(\delay, { | bus in mix |
		var sig, delay;
		sig = In.ar(bus, 2);
		delay = CombL.ar(sig, 0.25, [0.25, 0.24], 2);
		ReplaceOut.ar(bus, XFade2.ar(sig, delay, mix));
	}).add;

	SynthDef(\fader, { | out=0 in mute=1 amp=1 |
		Out.ar(out, In.ar(in, 2) * mute * amp)
	}).add;

	SynthDef(\line, { | bus val time |
		ReplaceOut.kr(bus, Line.kr(In.kr(bus), val, time, doneAction: 2))
	}).add;

	SynthDef(\sinenv, { | bus val time |
		ReplaceOut.kr(bus, EnvGen.kr(Env([In.kr(bus), val], [time], \sin), doneAction: 2))
	}).add;
)


// Busses //
(
	~numChannels = 9;

	~main = Bus.audio(s, 2);
	~mainAmp = Bus.control(s, 1);

	~numChannels.do { | i |
		("ch" ++ i).asSymbol.envirPut(Bus.audio(s, 2));
		("chAmp" ++ i).asSymbol.envirPut(Bus.control(s, 1));
	};

	~resFlt = Bus.control(s, 1);
	~soloGate = Bus.control(s, 1);
	~soloFreq = Bus.control(s, 1);
)


// Nodes //
(
	~mixGroup = Group.new;
	~fxGroup = Group.new;
	~synthGroup = Group.new;

	~mainFader = Synth(\fader, [\out, 0, \in, ~main], ~mixGroup);
	~numChannels.do { | i |
		("fader" ++ i).asSymbol.envirPut(Synth(\fader, [\out, ~main, \in, ("ch" ++ i).asSymbol.envirGet], ~mixGroup));
	};

	~ch3ins01 = Synth(\delay, [\bus, ~ch3, \mix, -0.8], ~fxGroup);
	~ch4ins01 = Synth(\delay, [\bus, ~ch4, \mix, -0.9], ~fxGroup);
	~ch5ins01 = Synth(\delay, [\bus, ~ch5, \mix, -0.6], ~fxGroup);
	~ch6ins01 = Synth(\delay, [\bus, ~ch6, \mix,  0.0], ~fxGroup);
	~ch7ins01 = Synth(\delay, [\bus, ~ch7, \mix, -0.8], ~fxGroup);
	~ch8ins01 = Synth(\delay, [\bus, ~ch8, \mix, -0.5], ~fxGroup);

	~mainFader.map(\amp, ~mainAmp);
	~numChannels.do { | i | ("fader" ++ i).asSymbol.envirGet.map(\amp, ("chAmp" ++ i).asSymbol.envirGet) };

	~mainAmp.value = 1;
	~chAmp0.value = 0.7;
	~chAmp1.value = 0.3;
	~chAmp2.value = 0.15;
	~chAmp3.value = 0.04;
	~chAmp4.value = 0.02;
	~chAmp5.value = 0.1;
	~chAmp6.value = 0.07;
	~chAmp7.value = 0.03;
	~chAmp8.value = 0.075;

	~soloGate.value = 0;
	~soloFreq.value = 1000;
	~soloSynth = Synth(\solo, [\out, ~ch5], ~synthGroup);
	~soloSynth.map(\gate, ~soloGate, \freq, ~soloFreq);
	~resControlSynth = Synth(\resControl, [\fltBus, ~resFlt], ~synthGroup);
)


// Patterns //
(
	~bd_intro = Pbind(
		\instrument, \bd,
		\out, ~ch0,
		\dur, Pseq([1], 16),
		\group, ~synthGroup.nodeID
	);

	~bd = Pbind(
		\instrument, \bd,
		\out, ~ch0,
		\dur, Pseq([1], 32),
		\group, ~synthGroup.nodeID
	);

	~sd_intro = Pbind(
		\instrument, \sd,
		\out, ~ch1,
		[\freq, \dur], Pseq([
			Pseq([[\, 1], [1, 1]], 6),
			Pseq([[\, 1], [1, 1], [\, 1], [1, 0.5], [1, 0.25], [1, 0.25]])
		]),
		\group, ~synthGroup.nodeID
	);

	~sd = Pbind(
		\instrument, \sd,
		\out, ~ch1,
		[\freq, \dur], Pseq([
			Pseq([[\, 1], [0, 1]], 15),
			Prand([
				Pseq([[\, 1], [0, 0.5], [0, 0.25], [0, 0.25]]),
				Pseq([[\, 0.5], [0, 0.25], [0, 0.25], [0, 0.5], [0, 0.25], [0, 0.25]]),
				Pseq([[\, 0.5], [0, 0.5], [0, 0.5], [0, 0.5]]),
				Pseq([[0, 0.25], [0, 0.25], [0, 0.5], [0, 0.25], [0, 0.25], [0, 0.5]])
			])
		]),
		\group, ~synthGroup.nodeID
	);

	~hat_intro = Pbind(
		\instrument, \hat,
		\out, ~ch2,
		\dur, Pseq([0.25], 64),
		\group, ~synthGroup.nodeID
	);

	~hat = Pbind(
		\instrument, \hat,
		\out, ~ch2,
		\dur, Pseq([0.25], 128),
		\group, ~synthGroup.nodeID
	);

	~res_intro = Pbind(
		\instrument, \res,
		\out, ~ch3,
		\fltBus, ~resFlt,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Pseq([2], 16), Pseq([0], 16)], 2),
		\octave, Pseq([4, 5], inf),
		\dur, 0.25,
		\group, ~synthGroup.nodeID
	);

	~res_chorus = Pbind(
		\instrument, \res,
		\out, ~ch3,
		\fltBus, ~resFlt,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Pseq([2], 16), Pseq([0], 16)], 4),
		\octave, Pseq([4, 5], inf),
		\dur, 0.25,
		\group, ~synthGroup.nodeID
	);

	~res_verse = Pbind(
		\instrument, \res,
		\out, ~ch3,
		\fltBus, ~resFlt,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Pseq([0], 32), Pseq([3], 14), Pseq([6], 2), Pseq([0], 16)], 2),
		\octave, Pseq([4, 5], inf),
		\dur, 0.25,
		\group, ~synthGroup.nodeID
	);

	~res_break = Pbind(
		\instrument, \res,
		\out, ~ch3,
		\fltBus, ~resFlt,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Pseq([2], 32), Pseq([0], 32)]),
		\octave, Pseq([4, 5], inf),
		\dur, 0.25,
		\group, ~synthGroup.nodeID
	);

	~poly_intro = Pbind(
		\instrument, \poly,
		\out, ~ch4,
		\scale, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
		\degree, Pseq([[3, 8, 12, 15], [0, 5, 9, 12], [8, 12, 15], [5, 9, 12]]),
		\octave, 5,
		\dur, 4,
		\legato, 0.5,
		\group, ~synthGroup.nodeID
	);

	~poly_chorus = Pbind(
		\instrument, \poly,
		\out, ~ch4,
		\scale, [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
		\degree, Pseq([[8, 12, 15], [5, 9, 12]], 4),
		\octave, 5,
		\dur, 4,
		\legato, 0.5,
		\group, ~synthGroup.nodeID
	);

	~solo_intro = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([\, 2, 3, 0]),
		\octave, 6,
		\dur, Pseq([8, 3.5, 0.5, 4]),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_chorus = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([\, 2, 3, 0, \, 2, 3, 7]),
		\octave, 6,
		\dur, Pseq([8, 3.5, 0.5, 4], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_verse = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([
			5, 6, 7, 6, 5, 4, 5, 6, 3, 4, 5, 4, 3, 2, 3, 4,
			5, 6, 7, 6, 5, 4, 5, 6, 3, 4, 5, 4, 3, 2, -1, 0
		]),
		\octave, 6,
		\dur, Pseq([1, 0.5, 1.5, 0.5, 0.5, 1, 0.5, 2.5], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_break1 = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([2, 3, 0]),
		\octave, 6,
		\dur, Pseq([7, 1, 8], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_break2 = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([2, 3, 7]),
		\octave, 6,
		\dur, Pseq([7, 1, 8], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_vamp1 = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([
			5, 6, 7, 8, 9, 4, 5, 6, 3, 4, 5, 4, 3, 2, 3, 4
		], 2),
		\octave, 5,
		\dur, Pseq([1, 0.5, 1.5, 0.5, 0.5, 1, 0.5, 2.5], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~solo_vamp2 = Pbind(
		\instrument, \soloControl,
		\gateBus, ~soloGate,
		\freqBus, ~soloFreq,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([
			5, 6, 7, 8, 9, 4, 5, 6, 3, 4, 5, 4, 3, 2, 3, 4
		], 2),
		\octave, 6,
		\dur, Pseq([1, 0.5, 1.5, 0.5, 0.5, 1, 0.5, 2.5], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_intro = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([7]),
		\octave, 7,
		\dur, 16,
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_chorus = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([7]),
		\octave, 7,
		\dur, 32,
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_verse = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([7, 6, 5, 4], 2),
		\octave, Prand([Pseq([7], 4), Pseq([6], 4)], inf),
		\dur, 4,
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_break = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([7], 16),
		\octave, Pseq([6, 7], inf),
		\dur, 1,
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_vamp1 = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Prand([0, 7]), 2, 3, 0, 2, 3, 7], 2),
		\octave, 5,
		\dur, Pseq([4, 1, 0.5, 6.5, 1, 0.5, 2.5], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~string_vamp2 = Pbind(
		\instrument, \string,
		\out, ~ch6,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([Prand([0, 7]), 2, 3, 0, 2, 3, 7], 2),
		\octave, 6,
		\dur, Pseq([4, 1, 0.5, 6.5, 1, 0.5, 2.5], inf),
		\legato, 1,
		\group, ~synthGroup.nodeID
	);

	~bass_verse = Pbind(
		\instrument, \bass,
		\out, ~ch7,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([0, 0, 3, 6, 7], 2),
		\octave, 3,
		\dur, Pseq([7.5, 0.5, 3.5, 0.5, 4], inf),
		\group, ~synthGroup.nodeID
	);

	~bass_break = Pbind(
		\instrument, \bass,
		\out, ~ch7,
		\scale, [0, 2, 3, 5, 7, 8, 10],
		\degree, Pseq([2, 0]),
		\octave, 3,
		\dur, 8,
		\group, ~synthGroup.nodeID
	);

	// Song sections //
	~intro1 = Ppar([~poly_intro, ~solo_intro]);
	~intro2 = Ppar([~hat_intro, ~res_intro]);
	~intro3 = Ppar([~bd_intro, ~sd_intro, ~hat_intro, ~res_intro, ~string_intro ]);
	~chorus = Ppar([~bd, ~sd, ~hat, ~res_chorus,  ~poly_chorus, ~solo_chorus, ~string_chorus]);
	~verse = Ppar([~bd, ~sd, ~hat, ~res_verse, ~solo_verse, ~string_verse, ~bass_verse]);
	~break1 = Ppar([~string_break, ~bass_break]);
	~break2 = Ppar([~solo_break1, ~string_break, ~bass_break]);
	~break3 = Ppar([~hat_intro, ~res_break, ~string_break]);
	~break4 = Ppar([~bd_intro, ~sd_intro, ~hat_intro, ~res_break, ~solo_break2, ~string_break]);
	~vamp1 = Ppar([~bd, ~sd, ~hat, ~res_verse, ~solo_vamp1, ~string_vamp1]);
	~vamp2 = Ppar([~bd, ~sd, ~hat, ~res_verse, ~solo_vamp2, ~string_vamp2, ~bass_verse]);

	// Mixer patterns //
	~fader8auto = Pbind(
		\instrument, \sinenv,
		\bus, ~chAmp8,
		[\freq, \dur, \val, \time], Pseq([[\, 12, 0, 0], [0, 530, 0, 16], [0, 1, 0.05, 8]]),
		\group, ~mixGroup.nodeID
	);

	~mainFaderAuto = Pbind(
		\instrument, \sinenv,
		\bus, ~mainAmp,
		[\freq, \dur, \val, \time], Pseq([[\, 560, 0, 0], [0, 1, 0, 16]]),
		\group, ~mixGroup.nodeID
	);
)


// Play it! //
(
	~tc = TempoClock.default;
	~introSynth = Synth(\intro, [\out, ~ch8], ~synthGroup);
	SystemClock.sched(30, {
		~tc.tempo = 98 / 60;
		//~beat = 0; ~tc.schedAbs(~tc.beats.ceil, { ~beat = ~beat + 1; ~beat.postln; 1 });
		~tc.sched(16, { ~tc.tempo = 118 / 60 });
		Ppar([
			Pseq([
				~intro1,
				~intro2,
				~intro3,
				~chorus,
				~chorus,
				~verse,
				~chorus,
				~verse,
				~break1,
				~break2,
				~break3,
				~break4,
				~verse,
				~chorus,
				~verse,
				~chorus,
				~vamp1,
				~vamp1,
				~vamp2,
				~vamp2,
				~vamp2,
				~vamp2
			]),
			Ppar([~fader8auto, ~mainFaderAuto])
		]).play;
	});
)

~mainFader.set(\mute, 0);
~mainFader.set(\mute, 1);


// Clean up //
s.freeAll;
