
// Séminaire Musical
// since 2007
// julian rohrhuber
// renate wieser

/*

This is an extended sound and music comprehension tool.
It is written to give a concisely written usable example,
which can be easily extended.

As usual, select all (cmd-a) and hit cmd-enter.
The synth server should start automatically,
if there is an error in the console, make sure
your soundcard in and out sample rates match.

In playing with this, you can try different combinations.
Some are harder, some are easier. There are no scores.

1. click next test and play ...
2. try to find the corresponding button in the row below
3. when you've found it, it will turn yellow.
Return to 1.

If "including low tones" is selected, frequencies that
require base speakers will not be excluded.

*/

(
// parameters
var mode=\western, allowBase=true, amp=0.5;

// resources:
var eurNotes, indNotes, chords, chordKeys;
var partchRatios, partchTonalities, partchPairsToRatio;
var indFreqs;

// states:
var solution_index;
var freqs, intervalle, rates, counts, freq0, note0, numChoices;

// internal states:
var referenceToneSynth;

// display states:
var synthDefName="sm_sine", nameList;

// views:
var dec, v_spiel, v_klang, v_neu, v_play, v_guess, v_display, v_context, v_referenceTone;

// functions
var f_setButtons, f_neu, f_play, f_guess, f_playReferenceTone, f_freeReferenceTone;
var f_freqToNoteName, f_makeDisplayString;

if(Server.default.serverRunning.not) { Server.default.boot };


chords = IdentityDictionary[
	'major' -> #[0, 2, 4],
	'minor' -> #[0, 2b, 4],
	'7th' -> #[0, 2, 4, 6b],
	'minor7' -> #[0, 2b, 4, 6b],
	'sixth' -> #[0, 2, 4, 5],
	'minor6' -> #[0, 2b, 4, 5],
	'aug' -> #[0, 2, 4s],
	'aug7' -> #[0, 2, 4s, 6b],
	'dim' -> #[0, 2b, 4b],
	'dim7' -> #[0, 2b, 4b, 6bb],
	'7th 5b' -> #[0, 2, 4b, 6b],
	'min7 5b' -> #[0, 2b, 4, 6b],
	'ninth' -> #[0, 2, 4, 6b, 8],
	'minor9' -> #[0, 2b, 4, 6b, 8],
	'major9' -> #[0, 2, 4, 6, 8],
	'11th' -> #[0, 2, 4, 6b, 8, 10],
	'dim9' -> #[0, 2, 5, 8],
	'added9' -> #[0, 2, 4, 8],
	'added4' -> #[0, 2, 4, 10],
	'sus' -> #[0, 3, 4],
	'sus9' -> #[0, 1, 4],
	'7 sus4' -> #[0, 3, 4, 6b],
	'7 sus9' -> #[0, 1, 4, 6b],
	'fifth intv' -> #[0, 4],
	'min9 intv' -> #[0, 13],
	'aug9 intv' -> #[0, 15]

];
chordKeys = chords.keys.asArray;


partchRatios =  #[
	[1, 1], [81, 80], [33, 32], [21, 20], [16, 15], [12, 11], [11, 10], [10, 9], [9, 8],
	[8, 7], [7, 6], [32, 27], [6, 5], [11, 9], [5, 4], [14, 11], [9, 7], [21, 16], [4, 3], [27, 20], [11, 8],
	[7, 5], [10, 7], [16, 11], [40, 27], [3, 2], [32, 21], [14, 9], [11, 7], [8, 5], [18, 11], [5, 3], [27, 16],
	[12, 7], [7, 4], [16, 9], [9, 5], [20, 11], [11, 6], [15, 8], [40, 21], [64, 33], [160, 81]
];


partchTonalities =  #[
	[0,8,14,20,25,34],
	[0,7,13,18,27,35],
	[0,6,12,21,29,36],
	[0,5,15,23,30,37],
	[0,10,18,25,31,38],
	[0,9,16,22,28,33],
	[0,9,18,23,29,35],
	[0,8,16,25,30,36],
	[0,7,14,22,31,37],
	[0,6,13,20,28,38],
	[0,5,12,18,25,33],
	[0,10,15,21,27,34]
];

partchPairsToRatio = { |pairs|
	pairs.collect { |x| x.reduce('/') };
};


// see: http://www.soundofindia.com/showarticle.asp?in_article_id=-446619640
indFreqs =  #[240, 256, 270, 288, 300, 320, 337.5, 360, 384, 400, 432, 450, 480];


eurNotes = #["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "Bb", "B", "C'"];
indNotes = #["Sa", "re", "Re", "ga", "Ga", "Ma", "ma", "Pa", "dha", "Dha", "ni", "Ni", "Sa'"];


// GUI

w = Window("Séminaire musical", Rect(10, 250, 880, 420));

w.view.decorator = dec = FlowLayout(w.view.bounds);
dec.shift(20, 20);
v_spiel = ListView(w, Rect(0,0, 120, 150))
.items_(
	[
		"notes",
		"frequencies",
		"notes + octaves",
		"intervals",
		"times",
		"beatings",
		"chords",
		"intonation",
		"counts"
	]
);
dec.shift(10);
v_klang = ListView(w, Rect(0,0, 100, 130)).items_(["sinus", "noise", "pulse", "bell", "piano", "timbre"]);

dec.shift(10);

/*
StaticText(w, Rect(0,0, 100,20))
.string_("Basic settings:")
.align_(\left);
*/

v_context = ListView(w, Rect(0,0, 80, 130))
.items_(
	[
		"western",
		"indian",
		"partch",
		"numerical"
	]
);
v_context.action = { |v|
	mode = [\western, \indian, \partch, \numerical][v.value.asInteger];
	f_neu.value
};

dec.shift(20);

v_referenceTone = Button(w, Rect(0, 0, 180, 20))
.states_([["reference tone off", Color.black, Color.grey(0.8)], ["reference tone", Color.red, Color.grey(0.8)]]);

Button(w, Rect(0,0, 180, 20))
.states_([["including low tones", Color.black, Color.grey(0.8)],["excluding low tones", Color.black, Color.grey(0.8)]])
.action_({ |v|
	allowBase = v.value < 1;
});



//dec.shift(200, -80);

dec.nextLine;
dec.shift(20, 20);

v_neu = Button(w, Rect(0,0, 80, 35)).states_([["next test", Color.black, Color.grey(0.8)]]);
v_play = Button(w, Rect(0,0, 70, 35)).states_([["play ...", Color.black, Color.grey(0.8)]]);
Slider(w, Rect(0,0, 200, 35)).value_(0.5).action_({ |v| amp = v.value.linexp(0, 1, 0.01, 8) });


dec.shift(-342, 50);




dec.nextLine;
dec.shift(20, 30);

StaticText(w, Rect(0,0, 300,20))
.string_("... and try to find the corresponding value here:")
.align_(\left);

dec.nextLine;
dec.shift(20, 0);

v_guess = Array.fill(13, {
	Button(w, Rect(0,0, 60, 35))//.font_(Font("Monaco", 9));
});

dec.nextLine;
//dec.shift(20, 0);
v_display = StaticText(w, Rect(0,0, w.view.bounds.width, 20))
.align_(\left);

w.front;

// funktionen

f_setButtons = { arg werte;
	v_guess.do { |but, i|
		var val = werte[i];
		val = val ?? { "" };
		but.states_([[val.asString, Color.black, Color.grey(0.8)]]);
		but.refresh;
	};


};
f_setButtons.("" ! 13);
f_neu = {
	var referenceToneIsPlaying, minFreq, octaves, tonality, randomOctaves, ratios;

	referenceToneIsPlaying = v_referenceTone.value > 0;
	if(allowBase) {
		minFreq = 60;
		octaves = #[0.5, 1, 2, 4];
	} {
		minFreq = 200;
		octaves = #[1, 2, 4];
	};

	switch(v_spiel.value,

		// notes
		0, {
			switch(mode,

				\indian, {
					freqs = indFreqs;
					nameList = indNotes;
					freq0 = freqs[0];
				},

				\numerical, {
					freqs = ((0..12) + 60).midicps;
					freq0 = freqs[0];
					nameList = (0..12)
				},

				\partch, {
					tonality = partchTonalities.scramble.keep(3)
					.flat.as(IdentitySet).as(Array).keep(13);
					ratios = partchRatios.at(tonality);
					freq0 = 392; // Partch's reference G
					freqs = partchPairsToRatio.(ratios) * freq0;
					nameList = ratios.collect { |x| "%/%".format(*x) };
				},

				{
					freqs = ((0..12) + 60).midicps;
					freq0 = freqs[0];
					nameList = eurNotes;
				}
			);
			v_klang.enabled = true;
			solution_index = freqs.size.rand;
		},

		// frequencies
		1, {
			nameList = { exprand(minFreq, 15000).round(1) } ! 13;
			freqs = nameList;
			freq0 = 440;
			v_klang.enabled = true;
			solution_index = freqs.size.rand;
		},
		// notes + octaves
		2, {
			if(mode == \partch) {
				tonality = partchTonalities.scramble.keep(3)
				.flat.as(IdentitySet).as(Array).keep(13);
				ratios = partchRatios.at(tonality);
				freq0 = 392; // Partch's reference G
				freqs = partchPairsToRatio.(ratios) * freq0;
				nameList = ratios.collect { |x| "%/%".format(*x) };
				randomOctaves = { octaves.choose } ! 13;
				freqs = freqs * randomOctaves;
			} {
				if(mode == \indian) {
					freqs = #[240, 256, 270, 288, 300, 320, 337.5, 360, 384, 400, 432, 450, 480];
					nameList = indNotes;
				} {
					freqs = ((0..12) + 60).midicps;
					if(mode == \numerical) { nameList = (0..12) } { nameList = eurNotes };
				};
				freq0 = freqs[0];
				freqs = freqs * ({ octaves.choose } ! 12);
			};
			v_klang.enabled = true;
			solution_index = freqs.size.rand;
		},
		// intervals
		3, {
			if(mode == \western) {
				freq0 = ((0..12).choose + 60).midicps * octaves.choose;
				intervalle = (1..12).midiratio.insert(9, 7/4); // insert 7/4
				freqs = freq0 * intervalle;
				nameList =
				#["-sekund", "+sekund", "-terz", "+terz", "quart", "tritone", "quint",
					"-sext", "+sext", "7/4", "-sept", "sept", "oktav"
				];
			} {
				// TODO: use proper partch intervals instead of just the ones below
				freq0 =  if(mode == \partch) {
					392
				} {
					indFreqs.choose;
				};

				freq0 = freq0 * octaves.choose;
				intervalle = [16/15, 9/8, 6/5, 5/4, 4/3, 17/12, 3/2, 8/5, 5/3, 7/4, 16/9, 15/8, 2/1];
				freqs = freq0 * intervalle;
				nameList =
				#["16/15", "9/8", "6/5", "5/4", "4/3", "17/12", "3/2", "8/5", "5/3", "7/4",
					"16/9", "15/8", "2/1"];
			};
			solution_index = freqs.size.rand;
			v_klang.enabled = true;

		},
		// times
		4, {
			referenceToneIsPlaying = false;
			freq0 = nil; // no base tone
			freqs = rrand(1600, 8000)  ! 13;

			if(mode == \numerical) {
				rates = { exprand(0.6, 4) } ! 13;
				nameList = (rates * 60).round(0.1) // bpm
			} {
				rates = { exprand(0.9, 19) } ! 13;
				nameList = rates.round(0.01)
			};
			solution_index = freqs.size.rand;

			v_klang.enabled = false;


		},
		// beatings
		5, {
			referenceToneIsPlaying = false;
			freq0 = ((0..12).choose + 60).midicps * octaves.choose;
			rates = { exprand(0.9, 40) } ! 13;
			nameList = rates.round(0.01);
			solution_index = freqs.size.rand;

			v_klang.enabled = true;


		},
		// chords
		6, {
			note0 = (0..11).choose + #[-12, 0].choose;
			freq0 = (note0 + 60).midicps;
			freqs = freq0 ! 13;
			nameList = chordKeys[ (0..chordKeys.size-1).scramble.keep(13)].sort;
			solution_index = freqs.size.rand;

			v_klang.enabled = true;
		},
		// intonation
		7, {
			referenceToneIsPlaying = false;
			freq0 = switch(mode)
			{ \partch } { 392 * partchRatios.choose.reduce('/') }
			{ \indian } { indFreqs.choose }
			{ ((0..11).choose + 60).midicps };
			freq0 = freq0 * octaves.choose;
			intervalle = (1..6).collect { |x| x * 10 + 10.rand };
			intervalle = intervalle.reverse.neg ++ 0 ++ intervalle;
			freqs = freq0 * (intervalle / 100).midiratio;
			nameList = intervalle.collect { |x| x + "ct" };
			solution_index = freqs.size.rand;
		},
		// counts
		8, {
			referenceToneIsPlaying = false;
			freq0 = nil; // no base tone
			freqs = rrand(1600, 8000)  ! 13;
			rates = { exprand(4,16) } ! 13;
			counts = (1..13).scramble;
			nameList = counts;
			solution_index = freqs.size.rand;

			v_klang.enabled = false;


		},
	);
	v_klang.enabled = true;
	numChoices = freqs.size;
	f_setButtons.(nameList);
	f_freeReferenceTone.value;
	if(referenceToneIsPlaying) { f_playReferenceTone.value };
};

f_neu.value; // init

f_play = { |i|
	if(i < numChoices) {
		switch(v_spiel.value,
			3, { // intervall
				fork {
					Synth(synthDefName, [\freq, freq0, \amp, amp]);
					0.4.wait;
					Synth(synthDefName, [\freq, freq0 * intervalle[i], \amp, amp]);
				}
			},
			4, { // rates
				Synth("sm_click", [\freq, freqs[i], \rate, rates[i], \amp, amp])
			},
			5, { // beatings
				Synth(synthDefName, [\freq, freq0,  \amp, amp]);
				Synth(synthDefName, [\freq, freq0 + rates[i], \amp, amp])
			},
			6, { // akkorde
				fork {
					var strum = [0, 0.04].choose.rand;
					var notes = chords[nameList[i]].degreeToKey(#[0, 2, 4, 5, 7, 9, 11]) + note0;
					notes.do { |val|
						Synth(synthDefName, [\freq, (val + 60).midicps, \amp, amp * 0.5]);
						strum.wait;
					}
				}

			},
			8, { // counts
				Synth("sm_count", [\freq, freqs[i], \rate, rates[i], \count, counts[i], \amp, amp])
			},
			{
				Synth(synthDefName, [\freq, freqs[i], \amp, amp])
			}
		)
	}


};
f_playReferenceTone = { v_referenceTone.valueAction = 1 };
f_freeReferenceTone = { v_referenceTone.valueAction = 0 };

// funktionen zu buttons

v_neu.action = f_neu;
v_play.action = { f_play.value(solution_index) };
v_spiel.action = f_neu;

f_freqToNoteName = { |freq|
	var midinote, note, deviation;
	var f_deviationString = { |d|
		if(d > 0) { "+" ++ d } { if(d == 0) { "" } { d } }
	};

	if(mode == \western) {
		midinote = freq.cpsmidi;
		note = (midinote - 60).round;
		deviation = ((freq / midinote.round.midicps).ratiomidi * 100).round.asInteger;
		format("% %", eurNotes.at(note % 12), f_deviationString.(deviation))
	} {
		if(mode == \indian) {
			freq = freq.wrap(240.0, 480.0);
			note = (indFreqs.indexOfGreaterThan(freq) - 1).max(0);
			if(note.isNil) { nil } {
				deviation = ((freq / indFreqs[note]).ratiomidi * 100).round.asInteger;
				format("% %", indNotes.at(note), f_deviationString.(deviation))
			}
		}
	}
};

f_makeDisplayString = { |index|
	var freq = freqs[index];
	var note = freq.cpsmidi - 60;
	var noteName = f_freqToNoteName.(freq);
	var referenceName = if(v_referenceTone.value > 0 and: { freq0.notNil }) {
		"reference tone: " + f_freqToNoteName.(freq0)
	};
	var str = "      note number last played: %       %      %";
	format(str, note.round(0.1), noteName ? "", referenceName ? "")

};

v_guess.do {|but, i|
	but.action = {
		if(solution_index == i) {
			but.states = [[but.states[0][0], Color.red, Color.yellow]];
		};
		f_play.value(i);
		v_display.string = f_makeDisplayString.value(i);

	};
};
v_klang.action = {|view|
	synthDefName =
	["sm_sine", "sm_noise", "sm_pulse", "sm_bell", "sm_string", "sm_timbremix"][view.value.asInteger]
};
v_referenceTone.action = { |view|
	if(view.value.isNil or: { view.value == 0 }) {
		if(referenceToneSynth.isPlaying) { referenceToneSynth.release };
	} {
		referenceToneSynth = Synth(\sm_referenceTone, [\freq, freq0, \amp, amp * freq0.notNil.asInteger]);
		referenceToneSynth.register;
	}
};

w.view.keyDownAction = { arg view, char;
	f_play.value(char.ascii - 48 % 13);
};

w.onClose = { f_freeReferenceTone.value };

// synthdefs

SynthDef("sm_sine", { arg freq=440, amp=0.5;
	amp = AmpComp.kr(freq) * amp;
	Out.ar(0,
		Pan2.ar(
			SinOsc.ar(freq) * EnvGen.ar(Env.perc(0.03, Rand(0.3, 2), amp), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

SynthDef("sm_noise", { arg freq=440, amp=0.5;
	var u;
	amp = AmpComp.kr(freq) * amp;
	u = BPF.ar(PinkNoise.ar(20), freq, Rand(0.1, 0.005));
	Out.ar(0,
		Pan2.ar(
			u * EnvGen.ar(
				Env.linen(Rand(0.03, 0.2), Rand(0.3, 2), Rand(0.3, 0.8), amp),
				doneAction:2
			),
			Rand(-0.5, 0.5)
		)
	)

}).add;

SynthDef("sm_pulse", { arg freq=440, amp=0.5;
	var u;
	amp = AmpComp.kr(freq) * amp;
	u = Pulse.ar(freq, Rand(0.3, 0.5), 0.3);
	Out.ar(0,
		Pan2.ar(
			u * EnvGen.ar(Env.perc(0.03, Rand(0.3, 2), amp), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

SynthDef("sm_bell", { arg freq=440, amp=0.5;
	n = 5;
	amp = AmpComp.kr(freq) * amp;
	Out.ar(0,
		Pan2.ar(
			Klang.ar(`[
				[1] ++ ({ rrand(1.2, 5.8) } ! (n-1)),
				[1] ++ ({ rrand(0.1, 0.3) } ! (n-1)).sort.reverse,
				{ pi.rand } ! n
			], freq)
			* EnvGen.ar(Env.perc(0.03, Rand(0.3, 2), amp * 0.5), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

(
SynthDef("sm_string", { arg freq=440, amp=0.5;
	var u, exc, sustain, detuneStrings, attackPeriod;
	amp = AmpComp.kr(freq) * amp * 2;
	attackPeriod = freq.linlin(440, 15000, 0.006, 0.00001);
	exc = LFDNoise3.ar(min(freq*6, 20000), Decay2.ar(Impulse.ar(0, 0, 0.5), attackPeriod, attackPeriod * 3));
	sustain = ExpRand(1.3, 2.1);
	detuneStrings = [0, Rand(-0.1, 0.1)];
	u = LPF.ar(
		CombC.ar(exc, 0.04, (freq + detuneStrings).reciprocal, sustain).sum,
		min(20000, freq * Rand(1.3, 3))
	);

	//DetectSilence.ar(u, doneAction:2);
	amp = amp * EnvGen.ar(Env.linen(0, sustain, 0.1), doneAction:2);
	Out.ar(0,
		Pan2.ar(u * amp, freq.cpsmidi % 12 / 6 - 1)
	);
}).add;
);

SynthDef("sm_click", { arg freq=440, rate=1, amp=0.5;
	var u;
	amp = AmpComp.kr(freq) * amp;
	u = SinOsc.ar(freq) * Decay.kr(Impulse.kr(rate, 0, 40), 0.002, 0.04);
	Out.ar(0,
		Pan2.ar(
			u * EnvGen.ar(Env.linen(0, min(2, IRand(4, 8) / rate), 0.1, amp), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;


SynthDef("sm_count", { arg freq=440, rate=1, count=1, amp=0.5;
	var u, trigs;
	amp = AmpComp.kr(freq) * amp;
	trigs = TDuty.ar(1/rate, 0, Dseries(1, 0, count));
	u = SinOsc.ar(freq) * Decay.ar(trigs * 40, 0.002, 0.04);
	Out.ar(0,
		Pan2.ar(u * EnvGen.ar(Env.linen(0, count/rate + 0.5, 0), 1, amp, doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;


SynthDef("sm_beatings", { arg freq=440, rate=1, amp=0.5;
	var u;
	amp = AmpComp.kr(freq) * amp;
	u = SinOsc.ar([0, rate] + freq).sum * 0.3;
	Out.ar(0,
		Pan2.ar(
			u * EnvGen.ar(Env.linen(0.01, max(Rand(0.3, 2),  3 / rate), 0.5, amp), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

SynthDef("sm_timbremix", { arg freq=440, amp=0.5;
	var n = 10, u;
	var ratios = ([1.0] ++ { ExpRand(1.0, 6.0) }.dup(n));
	freq = freq * ratios;
	amp = amp * (Rand(1.2, 1.6) ** ratios.neg);
	amp[0] = 1;
	amp = amp * 0.4 * AmpCompA.ir(freq);
	Out.ar(0,
		Pan2.ar(
			u = sum(
				SinOsc.ar(freq, 0, amp)
			) * EnvGen.ar(Env.perc(0.03, Rand(0.5, 3))),

			Rand(-0.5, 0.5)
		)
	);
	DetectSilence.ar(u, doneAction:2);

}).add;

SynthDef("sm_referenceTone", { arg freq=440, amp=0.5, gate=1.0;
	var u;
	amp = AmpComp.kr(freq) * amp * 0.5;
	u = Pulse.ar(freq * [0.5, 1] +.t [Rand(0.2, 0.5), Rand(0.2, 0.5).neg],
		LFNoise1.kr(0.11, 0.05, 0.4), 0.15).sum;
	u = RLPF.ar(u, LFNoise2.kr(0.3, 0.2, 1) * 5000, 0.5);
	Out.ar(0,
		u * EnvGen.ar(Env.asr(1.0, amp, 1.0), gate, doneAction:2)
	)

}).add;



)
