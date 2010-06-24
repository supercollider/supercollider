/*
Two Auditory Scene Analysis experiences 
as proposed by Bregman, Auditory Scene Analysis, MIT Press, 1990

1. Streaming: if rate of presentation and 
frequency gap between the two series of sinusoidal tones are varied, 
 one or two stream(s) appear(s).

2. Miniature ASA Problem: varying frequency distance of A and C with respect to B, and attack time of C, two groupings are possible: 
(A+B)/C: a two note melody with a low tone (parallel)
A(B+C): a simple tone and a complex tone (sequence)

(andrea valle)
*/


(
// start server
s = Server.local.boot;
)


(

/*
Streaming
*/


var r1, r2;
var rateSlide, deltaSlide, volumeSlide;
var pauseButton1, pauseButton2, recButton; 
var volume1 = 1.0, volume2 = 1.0;
var base = 500;
var synth1, synth2;
var fileName, index = 1;





// Two analogous synthDefs


SynthDef("sine1",{ arg out=0, freq=440, dur=1.0, mul=0.5;
	var env;
	env = Env.new([0, 1,1,0,0],[dur*0.05, dur*0.3,dur*0.15,dur*0.5], 'welch');
	Out.ar(out,
		SinOsc.ar(
			freq, 
			0, mul
		) * EnvGen.kr(env)
	)
}).send(s);




SynthDef("sine2",{ arg out=0, freq=440, dur=1.0, mul=0.5;
	var env;
	env = Env.new([0,0,1,1,0],[dur*0.5, dur*0.05, dur*0.3,dur*0.15], 'welch');
	Out.ar(out,
		SinOsc.ar(
			freq, 
			0, mul
		) * EnvGen.kr(env)
	)
}).send(s);





// Two analogous routines

r1 = Routine.new({
	inf.do({ arg i; var k;
		var arr=[0,0, 1, 0, 2];
		synth1 = Synth.new("sine1");
		k = arr[i.mod(5)]*100+base;
		synth1.set("freq", k);
		synth1.set("dur", rateSlide.value);
		synth1.set("mul", volumeSlide.value*volume1);
		rateSlide.value.wait;
		synth1.free;
		});
});




r2 = Routine.new({
	inf.do({ arg i; var k;
		var arr=[1, 0, 3, 1, 2];
		synth2 = Synth.new("sine2");
		k = arr[i.mod(5)]*100+base+deltaSlide.value;
		synth2.set("freq", k);
		synth2.set("dur", rateSlide.value);
		synth2.set("mul", volumeSlide.value*volume2);
		rateSlide.value.wait;
		synth2.free;
	});
});




// GUI stuff

w = Window("ASA: Streaming", Rect(20, 400, 440, 150)).front;
w.view.decorator = FlowLayout(w.view.bounds);
rateSlide = EZSlider(w, 400 @ 20, "DurEach", ControlSpec(0.05, 2.0, \exp, 0.001, 0.01), 
	{arg ez; ez.value;}, 0.1);
deltaSlide = EZSlider(w, 400 @ 20, "Delta", ControlSpec(0, 1000, \lin, 1, 1), 
	{arg ez; ez.value;}, 500);
	
volumeSlide = EZSlider(w, 400 @ 20, "Volume", ControlSpec(0.0, 1.0, \lin, 0.1, 0.1), 
	{arg ez; ez.value;}, 0.5);



w.view.decorator.nextLine.shift(50, 20);

	pauseButton1 = Button(w, 110 @ 30);
	pauseButton1.states = [
			["1 now is active",Color.black,Color.red],
			["1 now is muted",Color.white,Color.black],
		];
		
	pauseButton1.action = { arg state; 
			if(state.value == 1, {volume1 = 0; "muted 1".postln});
			if(state.value == 0, {volume1 = 1.0; "activated 1".postln}); 
		};
		
w.view.decorator.shift(10, 0);
		

	pauseButton2 = Button(w, 110 @ 30);
	pauseButton2.states = [
			["2 now is active",Color.black,Color.red],
			["2 now is muted",Color.white,Color.black],
		];
		
	pauseButton2.action = { arg state; 
			if(state.value == 1, {volume2 = 0; "muted 2".postln});
			if(state.value == 0, {volume2 = 1.0; "activated 2".postln}); 
		};




w.onClose_({r1.stop; synth1.free; r2.stop; synth2.free;});

SystemClock.play(r1);
SystemClock.play(r2);


)




(

/*
The miniature ASA problem
*/


var r1, r2, r3;
var text;
var rateSlide, volumeSlide, freqASlide, freqBSlide, freqCSlide, shiftCSlide;
var pauseButton1, pauseButton2, recButton; 
var volume1 = 1.0, volume2 = 1.0, volume3 = 1.0;
var base = 500;
var synth1, synth2, synth3;
var fileName, index = 1;




// Three analogous synthDefs


SynthDef("sineA",{ arg out=0, freq=1000, dur=1.0, mul=0.35;
	var env;
	env = Env.new([0, 0,1,1, 0,0],dur*[0.1, 0.0, 0.3, 0, 0.6], 'welch');
	Out.ar(out,
		SinOsc.ar(
			freq, 
			0, mul
		) * EnvGen.kr(env)
	)
}).send(s);




SynthDef("sineB",{ arg out=0, freq=500, dur=1.0, mul=0.35;
	var env;
	env = Env.new([0, 0,1,1, 0,0],dur*[0.6, 0.0, 0.3, 0, 0.1], 'welch');
	Out.ar(out,
		SinOsc.ar(
			freq, 
			0, mul
		) * EnvGen.kr(env)
	)
}).send(s);




SynthDef("sineC",{ arg out=0, freq=250, dur=1.0, mul=0.35, shift=0;
	var env;
	env = Env.new([0, 0,1,1, 0,0], dur*[0.6+shift, 0.0, 0.3, 0, 0.1+shift], 'welch');
	Out.ar(out,
		SinOsc.ar(
			freq, 
			0, mul
		) * EnvGen.kr(env)
	)
}).send(s);

// Three analogous routines

r1 = Routine.new({
	inf.do({ arg i;
		synth1 = Synth.new("sineA");
		synth1.set("freq", freqASlide.value);
		synth1.set("dur", rateSlide.value);
		synth1.set("mul", volumeSlide.value*volume1);
		rateSlide.value.wait;
		synth1.free;
	})
});




r2 = Routine.new({
	inf.do({ arg i; 
		synth2 = Synth.new("sineB");
		synth2.set("dur", rateSlide.value);
		synth2.set("mul", volumeSlide.value*volume2);
		rateSlide.value.wait;
		synth2.free;
	})
});



r3 = Routine.new({
	inf.do({ arg i;
		synth3 = Synth.new("sineC");
		synth3.set("freq", freqCSlide.value);
		synth3.set("dur", rateSlide.value);
		synth3.set("mul", volumeSlide.value*volume3);
		synth3.set("shift", shiftCSlide.value);
		rateSlide.value.wait;
		synth3.free;
	})
});



// GUI stuff

w = Window("ASA: Miniature problem", Rect(20, 400, 440, 220));
w.front; // make window visible and front window.
w.view.decorator = FlowLayout(w.view.bounds);
rateSlide = EZSlider(w, 400 @ 20, "DurEach", ControlSpec(0.2, 2.0, \exp, 0.001, 0.01), 
	{arg ez; ez.value;}, 1.0);
freqASlide = EZSlider(w, 400 @ 20, "Freq A", ControlSpec(50, 2000, \exp, 1, 1), 
	{arg ez; ez.value;}, 1000);
w.view.decorator.nextLine.shift(30);
text = 	TextField(w,Rect(0,0,150,30));
text.string = "Freq B = fixed at 500 Hz";
text.boxColor_(Color.grey);
text.stringColor_(Color.white);
w.view.decorator.nextLine;
freqCSlide = EZSlider(w, 400 @ 20, "Freq C", ControlSpec(50, 500, \exp, 1, 1), 
	{arg ez; ez.value;}, 250);	
shiftCSlide = EZSlider(w, 400 @ 20, "Shift C", ControlSpec(-0.5, 0.0, \lin, 0.01, 0.01), 
	{arg ez; ez.value;}, 0.0);		
volumeSlide = EZSlider(w, 400 @ 20, "Volume", ControlSpec(0.0, 1.0, \lin, 0.1, 0.1), 
	{arg ez; ez.value;}, 0.5);



w.view.decorator.nextLine.shift(50, 20);

	pauseButton1 = Button(w, 110 @ 30);
	pauseButton1.states = [
			["A now is active",Color.black,Color.red],
			["A now is muted",Color.white,Color.black],
		];
		
	pauseButton1.action = { arg state; 
			if(state.value == 1, {volume1 = 0; "muted A".postln});
			if(state.value == 0, {volume1 = 1.0; "activated A".postln}); 
		};
		
w.view.decorator.shift(10, 0);
		

	pauseButton2 = Button(w, 110 @ 30);
	pauseButton2.states = [
			["C now is active",Color.black,Color.red],
			["C now is muted",Color.white,Color.black],
		];
		
	pauseButton2.action = { arg state; 
			if(state.value == 1, {volume3 = 0; "muted C".postln});
			if(state.value == 0, {volume3 = 1.0; "activated C".postln}); 
		};


w.view.decorator.shift(20, 0);


w.onClose_({r1.stop; synth1.free; r2.stop; synth2.free;  r3.stop; synth3.free;});

SystemClock.play(r1);
SystemClock.play(r2);
SystemClock.play(r3);


)
