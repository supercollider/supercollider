
// Séminaire Musical Final 2007
// julian rohrhuber
// renate wieser

(
var dec, v_spiel, v_klang, v_neu, v_play, v_guess, v_display, v_sargam;
var f_setButtons, f_neu, f_play, f_guess, f_playSargam, f_freeSargam;
var spiel="sm_sine", freqs, index;
var intervalle, freq0, note0, rates, nameList, amp=0.5, sargamSynth;

var mode=\european, allowBase=true;
var eurNotes, indNotes, chords, chordKeys;

if(Server.default.serverRunning.not) { Server.default.boot };

eurNotes = #["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "Bb", "B", "C'"];
indNotes = #["Sa", "re", "Re", "ga", "Ga", "Ma", "ma", "Pa", "dha", "Dha", "ni", "Ni", "Sa'"];


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
	'fifth' -> #[0, 4]
];
chordKeys = chords.keys.asArray;

// GUI

w = Window("Séminaire musical", Rect(10, 250, 880, 420));

w.view.decorator = dec = FlowLayout(w.view.bounds);
dec.shift(20, 20);
v_spiel = ListView(w, Rect(0,0, 120, 130))
/*.items_(
["noten", "frequenzen", "noten + oktave","intervalle","times","beatings", "akkorde"]
);*/
.items_(
	["notes", "frequencies", "notes + octaves","intervals","times","beatings", "chords"]
);
dec.shift(5);
v_klang = ListView(w, Rect(0,0, 100, 130)).items_(["sinus", "noise", "pulse", "bell", "piano", "timbre"]);

dec.shift(20);

StaticText(w, Rect(0,0, 100,20))
.string_("Basic settings:")
.align_(\left);

Button(w, Rect(0,0, 80,20))
.states_([["europe","india","numerical"], Color.black, Color.grey(0.8)].flop)
.action_({ |v| mode = [\european, \indian, \numerical][v.value.asInteger]; f_neu.value; });

v_sargam = Button(w, Rect(0,0, 60, 20))
.states_([["sargam", Color.black, Color.grey(0.8)], ["sargam", Color.red, Color.grey(0.8)]]);

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
dec.shift(20);
v_display = StaticText(w, Rect(0,0,200,35));

w.front;

// funktionen

f_setButtons = { arg werte;
	v_guess.do { |but, i|
		but.states_([[werte[i].asString, Color.black, Color.grey(0.8)]]);
		but.refresh;
	};


};
f_setButtons.("" ! 13);
f_neu = {
	var sargamIsPlaying, minFreq, octaves;
	sargamIsPlaying = v_sargam.value > 0;
	if(allowBase) {
		minFreq = 60;
		octaves = #[0.5, 1, 2, 4];
	} {
		minFreq = 200;
		octaves = #[1, 2, 4];
	};

	index = 13.rand;
	switch(v_spiel.value,
		0, {
			if(mode == \indian) {

				freqs = #[240, 256, 270, 288, 300, 320, 337.5, 360, 384, 400, 432, 450, 480];
				// see: http://www.soundofindia.com/showarticle.asp?in_article_id=-446619640
				nameList = indNotes;
			} {
				freqs = ((0..12) + 60).midicps;
				if(mode == \numerical) { nameList = (0..12) } { nameList = eurNotes };
			};
			freq0 = freqs[0];

		},
		1, {
			nameList = { exprand(minFreq, 15000).round(1) } ! 13;
			freqs = nameList;
			freq0 = 440;
		},
		2, {
			if(mode == \indian) {
				freqs = #[240, 256, 270, 288, 300, 320, 337.5, 360, 384, 400, 432, 450, 480];
				nameList = indNotes;
			} {
				freqs = ((0..12) + 60).midicps;
				if(mode == \numerical) { nameList = (0..12) } { nameList = eurNotes };
			};
			freq0 = freqs[0];
			freqs = freqs * ({ octaves.choose } ! 12);


		},
		3, {
			if(mode == \european) {
				freq0 = ((0..12).choose + 60).midicps * octaves.choose;
				intervalle = (1..12).midiratio.insert(9, 7/4); // insert 7/4
				freqs = freq0 * intervalle;
				nameList =
				#["-sekund", "+sekund", "-terz", "+terz", "quart", "tritone", "quint",
					"-sext", "+sext", "7/4", "-sept", "sept", "oktav"
				];
			} {
				freq0 =  #[240, 256, 270, 288, 300, 320, 337.5, 360, 384, 400, 432, 450].choose;
				freq0 = freq0 * octaves.choose;
				intervalle = [16/15, 9/8, 6/5, 5/4, 4/3, 17/12, 3/2, 8/5, 5/3, 7/4, 16/9, 15/8, 2/1];
				freqs = freq0 * intervalle;
				nameList =
				#["16/15", "9/8", "6/5", "5/4", "4/3", "17/12", "3/2", "8/5", "5/3", "7/4",
					"16/9", "15/8", "2/1"];
			};
		},
		4, {
			freq0 = rrand(1600, 8000);
			freqs = freq0 ! 13;
			rates = { exprand(0.9, 19) } ! 13;
			nameList = rates.round(0.01);

		},
		5, {
			freq0 = ((0..11).choose + 60).midicps * octaves.choose;
			freqs = freq0 ! 13;
			rates = { exprand(0.9, 40) } ! 13;
			nameList = rates.round(0.01);

		},
		6, {
			note0 = (0..11).choose + #[-12, 0].choose;
			freq0 = (note0 + 60).midicps;
			freqs = freq0 ! 13;
			nameList = chordKeys[ (0..chordKeys.size-1).scramble.keep(13)].sort;

	});
	f_setButtons.(nameList);
	f_freeSargam.value;
	if(sargamIsPlaying) { f_playSargam.value };
};

f_neu.value; // init

f_play = { |i|
	switch(v_spiel.value,
		3, { // intervall
			fork {
				Synth(spiel, [\freq, freq0, \amp, amp]);
				0.4.wait;
				Synth(spiel, [\freq, freq0 * intervalle[i], \amp, amp]);
			}
		},
		4, { // rates
			Synth("sm_click", [\freq, freq0, \rate, rates[i], \amp, amp])
		},
		5, { // beatings
			if(spiel.postln == "sm_string") {
				Synth("sm_string", [\freq, freq0,  \amp, amp]);
				Synth("sm_string", [\freq, freq0 + rates[i], \amp, amp])
			} {
				Synth("sm_beatings", [\freq, freq0, \rate, rates[i], \amp, amp])
			}
		},
		6, { // akkorde
			fork {
				var strum = [0, 0.04].choose.rand;
				var notes = chords[nameList[i]].degreeToKey(#[0, 2, 4, 5, 7, 9, 11]) + note0;
				notes.do { |val|
					Synth(spiel, [\freq, (val + 60).midicps, \amp, amp * 0.5]);
					strum.wait;
				}
			}

		},
		{
			Synth(spiel, [\freq, freqs[i], \amp, amp])
		}
	)


};
f_playSargam = { v_sargam.valueAction = 1 };
f_freeSargam = { v_sargam.valueAction = 0 };

// funktionen zu buttons
v_neu.action = f_neu;
v_play.action = { f_play.value(index) };
v_spiel.action = f_neu;
v_guess.do {|but, i|
	but.action = {
		if(index == i) {
			but.states = [[but.states[0][0], Color.red, Color.yellow]];
		};
		f_play.value(i);
		v_display.string = "note number last played:" + (freqs[i].cpsmidi - 60).round(0.1);
	};
};
v_klang.action = {|view|
	spiel =
	["sm_sine", "sm_noise", "sm_pulse", "sm_bell", "sm_string", "sm_timbremix"][view.value.asInteger]
};
v_sargam.action = { |view|
	view.value.postln;
	if(view.value.isNil or: { view.value == 0 })
	{ sargamSynth.release } { sargamSynth = Synth(\sm_sargam, [\freq, freq0, \amp, amp])
	};
};

w.view.keyDownAction = { arg view, char;
	f_play.value(char.ascii - 48 % 13);
};

w.onClose = { f_freeSargam.value };

// synthdefs

SynthDef("sm_sine", { arg freq=440, amp=0.5;
	amp = AmpComp.kr(freq) * amp;
	Out.ar(0,
		Pan2.ar(
			SinOsc.ar(freq) * EnvGen.kr(Env.perc(0.03, Rand(0.3, 2), amp), doneAction:2),
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
			u * EnvGen.kr(
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
			u * EnvGen.kr(Env.perc(0.03, Rand(0.3, 2), amp), doneAction:2),
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
			* EnvGen.kr(Env.perc(0.03, Rand(0.3, 2), amp * 0.5), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

(
SynthDef("sm_string", { arg freq=440, amp=0.5;
	var u, exc, sustain;
	amp = AmpComp.kr(freq) * amp;
	exc = LFNoise2.ar(3000, Decay2.ar(Impulse.ar(0.0001, 0, 0.5), 0.006, 0.02));
	sustain = Rand(1, 1.5);
	u = LPF.ar(
		CombN.ar(exc, 0.04, (freq * (1+[0, 0.001])).reciprocal, sustain).sum,
		min(20000, freq * Rand(3, 2.3) ! 2)
	).sum;

	//DetectSilence.ar(u, doneAction:2);
	amp = amp * EnvGen.kr(Env.linen(0, sustain, 0.1), doneAction:2);
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
			u * EnvGen.kr(Env.linen(0, min(2, IRand(4, 8) / rate), 0.1, amp), doneAction:2),
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
			u * EnvGen.kr(Env.linen(0.01, max(Rand(0.3, 2),  3 / rate), 0.5, amp), doneAction:2),
			Rand(-0.5, 0.5)
		)
	)

}).add;

SynthDef("sm_sargam", { arg freq=440, amp=0.5, gate=1.0;
	var u;
	amp = AmpComp.kr(freq) * amp * 0.5;
	u = Pulse.ar(freq * [0.5, 1] +.t [Rand(0.2, 0.5), Rand(0.2, 0.5).neg],
		LFNoise1.kr(0.11, 0.05, 0.4), 0.15).sum;
	u = RLPF.ar(u, LFNoise2.kr(0.3, 0.2, 1) * 5000, 0.5);
	Out.ar(0,
		u * EnvGen.kr(Env.asr(1.0, amp, 1.0), gate, doneAction:2)
	)

}).add;

SynthDef("sm_timbremix", { arg freq=440, amp=0.5;
	var n = 10, u;
	freq = freq * ([1.0] ++ { ExpRand(1.0, 3.0) }.dup(18));
	amp = amp * 0.4 * (AmpComp.ir(freq) * ([1.0] ++ (1..n).reciprocal));
	Out.ar(0,
		Pan2.ar(
			u = sum(
				SinOsc.ar(freq, 0, amp)
			) * EnvGen.kr(Env.perc(0.03, Rand(0.3, 2))),

			Rand(-0.5, 0.5)
		)
	);
	DetectSilence.ar(u, doneAction:2);

}).add;

)
