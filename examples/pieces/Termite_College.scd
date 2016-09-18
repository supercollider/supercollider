// Termite College
// (Tim Walters) (CC 2006)

// Features recursively generated phase modulation trees

s.serverRunning.not.if({ s.boot });

(
var pmSynthDefFactory;

pmSynthDefFactory = {
	|name, out = 0, flatness = 6, maxOperators = 32, flavor = 0, attack = 4.0, release = 4.0,
		sustain = 1.0, trace = false|
	var freqFuncs, freqModFunc, depthFunc, minNumModulatorsFunc;
	var maxNumModulatorsFunc, levelSpec, pmtree;

	// This does timbral shifts by modulating volume of individual operators
	depthFunc = { LinExp.kr(LFNoise1.kr(exprand(0.1, 2.0)), -1, 1, 0.1, 1.0) };

	// Three flavors
	freqFuncs = [
		{ exprand(0.125, 4000) },
		{ exprand(40, 4000) },
		{ exprand(80, 500) }
	];

	// Destabilize oscillators a bit.
	// Would need a LinExp for proper tuning, but here
	// we don't care.
	freqModFunc = { BrownNoise.kr.range(0.998, 0.998.reciprocal) };

	// This could be something more interesting
	minNumModulatorsFunc = {
		|level|
		1
	};

	// Tree thins out slowly with increasing depth
	maxNumModulatorsFunc = {
		|level|
		(level.reciprocal.sqrt * flatness).asInteger + 1;
	};

	// Generate tree of given size. If trace is true, dumps representation to post window.
	pmtree = {
		|level, size, trace = false|
		var numModulators, numSubOperators, modGroup, thisUnitDepth, freq, freqMod;
		var output, newSizes;

		// Keep root oscillator in normal note range
		freq = (level == 1).if(
			{ exprand(80, 500) },
			{ freqFuncs.wrapAt(flavor).value }
		);
		trace.if({
			(level - 1).do({ "-".post });
			freq.postln;
		});
		freqMod = freqModFunc.value;
		numSubOperators = size - 1;

		// Keep root oscillator at appropriate volume (don't modulate)
		thisUnitDepth = (level == 1).if(
			{ -18.dbamp * AmpComp.kr(freq) },
			{ depthFunc.value }
		);

		// Choose number of tree branches, based on current level
		numModulators = (
			minNumModulatorsFunc.value(level)..maxNumModulatorsFunc.value(level)
		).choose.min(numSubOperators);

		// Recursively generate subtrees, dividing up size among them.
		(size == 0).if({
			output = 0
		}, {
			(numModulators == 0).if({
				modGroup = 0
			}, {
				newSizes = 0 ! numModulators;
				numSubOperators.do({
					var index;
					index = rrand(0, numModulators - 1);
					newSizes[index] = newSizes[index] + 1;
				});
				modGroup = Mix.ar({
					|i|
					pmtree.value(level + 1, newSizes[i], trace)
				} ! numModulators);
			});

			// Return sine oscillator with subtrees summed as phase modulator
			output = SinOsc.ar(freq * freqMod, modGroup, thisUnitDepth);
		});
		output
	};

	// Generate synthdef from tree. Use audio rate panning for stereo,
	// with modulated width.
	SynthDef.new(name ? \PMTree, {
		|attack = 4.0, release = 4.0, sustain = 1.0, gate = 1.0|
		var env;
		env = EnvGen.kr(Env.asr(attack, sustain, release, curve: [4, -4]), gate, doneAction: 2);
		Out.ar(out, Pan2.ar(
			pmtree.value(1, maxOperators, trace),
			SinOsc.ar(freqFuncs.wrapAt(flavor).value, 0, depthFunc.value, rrand(-0.5, 0.5)),
			env
		));
	})
};

~synths = Set[];

~effectBus = Bus.audio(s, 2);

// Add some reverb.
~effect = {
	|gate = 1.0|
	var env;
	env = EnvGen.ar(Env.asr(0, 1.0, 16.0), gate, doneAction: 2);
	GVerb.ar(
		// Tame those pesky upper mids.
		BPeakEQ.ar(In.ar(~effectBus.index, 2), 4000.0, 0.5, -6),
		roomsize: 50,
		earlyreflevel: -9.dbamp,
		taillevel: -12.dbamp,
		mul: env
	)
}.play;

// Player generates and plays synths of random size and flavor.
~player = Task({
	var synthName, polyphony;
	polyphony = 4;
	inf.do({
		|i|
		var targetNumSynths;
		synthName = ("PMTree" ++ (i % 100).asString).asSymbol;
		targetNumSynths = rrand(1, polyphony - 1);
		{~synths.size > targetNumSynths}.while({
			~synths.remove(~synths.choose.release);
		});
		~synths.add(
			pmSynthDefFactory.value(
				synthName,
				~effectBus.index,
				exprand(2.0, 9.0).asInteger,
				exprand(4.0, 33.0).asInteger,
				[0, 0, 0, 0, 1, 2].choose,
				exprand(1.0, 8.0),
				exprand(4.0, 16.0),
				4.0
			).play
		);
		exprand(12.0, 36.0).wait;
	});
});

~player.start;
)

(
// Execute to finish.
~player.stop;
~synths.do({ |synth| ~synths.remove(synth.release) });
~effect.release;
)

(
// Execute after sound has completely died away.
~effectBus.free;
)
