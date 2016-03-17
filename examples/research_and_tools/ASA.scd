/**
 * Two Auditory Scene Analysis (ASA) experiences
 * as proposed by Albert Bregman, Auditory Scene Analysis, MIT Press, 1990
 *
 * 1. Streaming: if rate of presentation and
 * frequency gap between the two series of sinusoidal tones are varied,
 * one or two stream(s) appear(s).
 *
 * 2. Miniature ASA Problem:
 * Three short sinusoidal segments.
 * Varying frequency distance of A and C with respect to B, and attack time of C, two groupings are possible:
 * (A-B)/C: a two note melody with a low tone (parallel)
 * A-(B+C): a simple tone and a complex tone (sequence)
 *
 * (andrea valle)
 */


// start server
s.boot;

(
// Streaming
var r;
var rateSlide, deltaSlide, volumeSlide;
var pauseButton1, pauseButton2, recButton;
var volume1 = 1.0, volume2 = 1.0;
var base = 500;
var durVar1 = [0.05, 0.3, 0.15, 0.5], durVar2 = [0.5, 0.05, 0.3, 0.15];
var levels1 = [0, 1, 1, 0, 0], levels2 = [0, 0, 1, 1, 0];
var synth1, synth2;
var ezFunc = { |ez| ez.() };
var rateSpec = ControlSpec(0.05, 2.0, \exp, 1e-3, 0.01);
var deltaSpec = ControlSpec(0, 1000, \lin, 1, 1);
var volumeSpec = ControlSpec(0.0, 1.0, \lin, 0.1, 0.1);


// a SynthDef
SynthDef(\asaSine1, { |out=0, freq=440, dur=1.0, levels=#[0, 1, 1, 0, 0], durVar=#[1, 1, 1, 1], mul=0.5|
	var env, snd;

	env = Env.new(levels, dur * durVar, 'welch');
	snd = SinOsc.ar(freq, mul: mul);

	Out.ar(out, snd * EnvGen.kr(env));
}).add;


// a Routine
r = Routine {
	inf.do { |i|
		var k;
		var arr1 = [0, 0, 1, 0, 2],
		arr2 = [1, 0, 3, 1, 2];

		// 1st Synth
		synth1 = Synth.new(\asaSine1, [\levels, levels1, \durVar, durVar1]);
		k = arr1[i.mod(5)] * 100 + base;
		synth1.set(\freq, k);
		synth1.set(\dur, rateSlide.value);
		synth1.set(\mul, volumeSlide.value * volume1);

		// 2nd Synth
		synth2 = Synth(\asaSine1, [\levels, levels2, \durVar, durVar2]);
		k = arr2[i.mod(5)] * 100 + base + deltaSlide.value;
		synth2.set(\freq, k);
		synth2.set(\dur, rateSlide.value);
		synth2.set(\mul, volumeSlide.value * volume2);

		// wait and free the Synths
		rateSlide.value.wait;
		[synth1, synth2].do(_.free);
	}
};


// GUI stuff
w = Window("ASA: Streaming", Rect(20, 400, 440, 150)).front;
w.view.decorator = FlowLayout(w.view.bounds);

rateSlide = EZSlider(w, 400@20, "DurEach", rateSpec, ezFunc, 0.1);
deltaSlide = EZSlider(w, 400@20, "Delta", deltaSpec, ezFunc, 500);
volumeSlide = EZSlider(w, 400@20, "Volume", volumeSpec, ezFunc, 0.5);

w.view.decorator.nextLine.shift(50, 20);

pauseButton1 = Button(w, 110@30);
pauseButton1.states = [
	["1 now is active", Color.black, Color.red],
	["1 now is muted", Color.white, Color.black],
];
pauseButton1.action = { |state|
	switch(state.value,
		1, { volume1 = 0; "muted 1".postln },
		0, { volume1 = 1; "activated 1".postln }
	)
};

w.view.decorator.shift(10, 0);

pauseButton2 = Button(w, 110@30);
pauseButton2.states = [
	["2 now is active", Color.black, Color.red],
	["2 now is muted", Color.white, Color.black],
];
pauseButton2.action = { |state|
	switch(state.value,
		1, { volume2 = 0; "muted 2".postln },
		0, { volume2 = 1; "activated 2".postln }
	)
};


w.onClose_({ r.stop; [synth1, synth2].do(_.free); });

SystemClock.play(r);
)




(
// The miniature ASA problem
var r1, r2, r3;
var text;
var rateSlide, volumeSlide, freqASlide, freqBSlide, freqCSlide, shiftSlide;
var pauseButton1, pauseButton2, recButton;
var volume1 = 1.0, volume2 = 1.0, volume3 = 1.0;
var base = 500;
var durVar1 = [0.1, 0, 0.3, 0, 0.6], durVar2 = [0.6, 0, 0.3, 0, 0.1];
var durVar3 = durVar2;
var synth1, synth2, synth3;
var ezFunc = { |ez| ez.() };
var rateSpec = ControlSpec(0.2, 2.0, \exp, 1e-3, 0.01);
var freqASpec = ControlSpec(50, 2000, \exp, 1, 1);
var freqCSpec = ControlSpec(50, 500, \exp, 1, 1);
var shiftSpec = ControlSpec(-0.5, 0.0, \lin, 0.01, 0.01);
var volumeSpec = ControlSpec(0.0, 1.0, \lin, 0.1, 0.1);


// a SynthDef
SynthDef(\asaSine2, { |out=0, freq=1000, dur=1.0, durVar=#[1,1,1,1,1], mul=0.35, shift=0|
	var env, snd;

	env = Env.new([0, 0, 1, 1, 0, 0], dur * (durVar + (shift * [1, 0, 0, 0, 1])), 'welch');
	snd = SinOsc.ar(freq, 0, mul);

	Out.ar(out, snd * EnvGen.kr(env));
}).add;


// a Routine
r = Routine {
	inf.do { |i|

		// 1st Synth
		synth1 = Synth(\asaSine2, [\durVar, durVar1]);
		synth1.set(\freq, freqASlide.value);
		synth1.set(\dur, rateSlide.value);
		synth1.set(\mul, volumeSlide.value * volume1);
		// 2nd Synth
		synth2 = Synth(\asaSine2, [\freq, 500, \durVar, durVar2]);
		synth2.set(\dur, rateSlide.value);
		synth2.set(\mul, volumeSlide.value * volume2);
		// 3rd Synth
		synth3 = Synth(\asaSine2, [\durVar, durVar3]);
		synth3.set(\freq, freqCSlide.value);
		synth3.set(\dur, rateSlide.value);
		synth3.set(\mul, volumeSlide.value * volume3);
		synth3.set(\shift, shiftSlide.value);

		// wait and free the Synths
		rateSlide.value.wait;
		[synth1, synth2, synth3].do(_.free);
	}
};


// GUI stuff
w = Window("ASA: Miniature problem", Rect(20, 400, 440, 220)).front;
w.view.decorator = FlowLayout(w.view.bounds);

rateSlide = EZSlider(w, 400@20, "DurEach", rateSpec, ezFunc, 1.0);
freqASlide = EZSlider(w, 400@20, "Freq A", freqASpec, ezFunc, 1e3);

w.view.decorator.nextLine.shift(30);

text = TextField(w,Rect(0,0,200,30));
text.string = "Freq B = fixed at 500 Hz";
text.background_(Color.grey);
text.stringColor_(Color.white);

w.view.decorator.nextLine;

freqCSlide = EZSlider(w, 400@20, "Freq C", freqCSpec, ezFunc, 250);
shiftSlide = EZSlider(w, 400@20, "Shift C", shiftSpec, ezFunc, 0.0);
volumeSlide = EZSlider(w, 400@20, "Volume", volumeSpec, ezFunc, 0.5);

w.view.decorator.nextLine.shift(50, 20);

pauseButton1 = Button(w, 110@30);
pauseButton1.states = [
	["A now is active", Color.black, Color.red],
	["A now is muted", Color.white, Color.black],
];
pauseButton1.action = { |state|
	switch(state.value,
		1, { volume1 = 0; "muted A".postln },
		0, { volume1 = 1; "activated A".postln }
	)
};

w.view.decorator.shift(10, 0);

pauseButton2 = Button(w, 110@30);
pauseButton2.states = [
	["C now is active", Color.black, Color.red],
	["C now is muted", Color.white, Color.black],
];
pauseButton2.action = { |state|
	switch(state.value,
		1, { volume3 = 0; "muted C".postln },
		0, { volume3 = 1; "activated C".postln }
	)
};

w.onClose_({ r.stop; [synth1, synth2, synth3].do(_.free); });

SystemClock.play(r);
)
