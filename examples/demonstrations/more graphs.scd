s.boot;

(

SynthDef("bubbles", {
	var f, zout;
	f = LFSaw.kr(0.4, 0, 24, LFSaw.kr([8,7.23], 0, 3, 80)).midicps; // glissando function
	zout = CombN.ar(SinOsc.ar(f, 0, 0.04), 0.2, 0.2, 4); // echoing sine wave
	Out.ar(0, zout.postln);
}).add;

SynthDef("moto-rev", {
	var x;
	x = RLPF.ar(LFPulse.ar(SinOsc.kr(0.2, 0, 10, 21), [0,0.1], 0.1), 100, 0.1).clip2(0.4);
	Out.ar(0, x);
}).add;

SynthDef("thru", { arg out, gate=1;
	var zin, zout, env;
	env = Linen.kr(gate, 2, 1, 2);
	XOut.ar(out, env, ClipNoise.ar([0.1,0.1]));
	FreeSelfWhenDone.kr(env);
}).add;

SynthDef("wah", { arg out, rate = 1.5, cfreq = 1400, mfreq = 1200, rq=0.1, wet=1, gate=1;
	var zin, zout, env;
	zin = In.ar(out, 2);
	cfreq = Lag3.kr(cfreq, 0.1);
	mfreq = Lag3.kr(mfreq, 0.1);
	env = Linen.kr(gate, 1, 1, 1);
	wet  = Ramp.kr(wet, 0.1);
	rq   = Ramp.kr(rq, 0.1);
	zout = RLPF.ar(zin, LFNoise1.kr(rate, mfreq, cfreq), rq, 10).distort * 0.15;
	XOut.ar(out, wet * env, zout);
	FreeSelfWhenDone.kr(env);
}).add;

SynthDef("combdist", { arg out, freq = 400, decay=2, wet=1, gate=1;
	var zin, zout, env;
	zin = In.ar(out, 2);
	freq = Lag3.kr(freq, 0.1);
	env = Linen.kr(gate, 1, 1, 1);
	wet  = Ramp.kr(wet, 0.1);
	zout = CombN.ar(zin, 24.midicps.reciprocal, freq.reciprocal, decay).distort.reverse;
	XOut.ar(out, wet * env, zout);
	FreeSelfWhenDone.kr(env);
}).add;

SynthDef("echo", { arg out, maxdelay = 0.5, delay = 0.4, decay=3, wet=1, gate=1;
	var zin, zout, env;
	zin = In.ar(out, 2);
	delay = Lag3.kr(delay, 0.1);
	env = Linen.kr(gate, 1, 1, 1);
	wet  = Ramp.kr(wet, 0.1);
	zout = CombN.ar(zin * env, maxdelay, delay, decay).reverse;
	XOut.ar(out, wet * env, zout);
	FreeSelfWhenDone.kr(env);
}).load(s);

SynthDef("reverb", { arg out, decay=6, wet=1, gate=1;
	var zin, zout, env;
	zout = In.ar(out, 2);
	env = Linen.kr(gate, 0.1, 1, 0.1);
	wet = Ramp.kr(wet, 0.1);
	decay = Ramp.kr(decay, 0.1);
	8.do({
		zout = AllpassN.ar(zout, 0.04, {Rand(0.001,0.04)}.dup, decay)
	});
	XOut.ar(out, wet * env, zout);
	FreeSelfWhenDone.kr(env);
}).load(s);

SynthDef("ring-mod", { arg out, freq=800, wet=1, gate=1;
	var zin, zout, env;
	freq = Ramp.kr(freq, 0.1);
	env = Linen.kr(gate, 1, 1, 1);
	wet  = Ramp.kr(wet, 0.1);
	zin = In.ar(out, 2);
	zout = zin * SinOsc.ar(freq, [0,0.5pi]);
	XOut.ar(out, wet * env, zout);
	FreeSelfWhenDone.kr(env);
}).add;


/*

a = sqrt(1 - b.squared);
XFadeOut.ar(chan, xfade, outs)

fade in an effect, fade out an effect.

simple mix architecture:
8 stereo inputs with gain & balance.
4 insert fx per input with 4 params each.
4*4*8

groups:
	control group
	voice group
	fx group
	mixer

*/

SynthDef("mixer3", {
	var channels, faders, main;
	channels = In.ar(4, 16).clump(2);
	main = Ramp.kr(In.kr(0, 1), 0.05);
	faders = Ramp.kr(In.kr(1, 8), 0.05);
	Out.ar(0, Mix(channels * faders) * main);
}).add;

SynthDef("line", { arg out, start=1, end=0, dur=3;
	var line;
	line = Line.kr(start, end, dur);
	ReplaceOut.kr(out, line);
	FreeSelfWhenDone.kr(line);
}).add;

SynthDef("xline", { arg out, start=1, end=0.0001, dur=3;
	var line;
	line = XLine.kr(start, end, dur);
	Out.kr(out, line);
	FreeSelfWhenDone.kr(line);
}).add;


)



//patch a control to read from a bus.

//gradually change a parameter over some period.


(
	var server;
	server = Server.local;

	d = 0.04;
	r = Routine({

		3.do({

		0.5.postln.wait;

		server.sendBundle(d, ["/s_new", "moto-rev", 1000, 0]);

		3.wait;

		//server.sendBundle(d, ["nod.free", 1000]);


		server.sendMsg("/n_free", 1000);
		1.wait;
		});

	});
	SystemClock.play(r);
)

(
	d = 0.05;
	r = Routine({
		var server;
		server = Server.local;

		1.do({

			0.5.postln.wait;

			server.sendBundle(d, ["/s_new", "bubbles", 1000, 0].postln);

			// add effects, each one (processing order) after the previous one
			4.wait;
			server.sendBundle(d, ["/s_new", "combdist", 1004, 3,1000, \freq, rrand(24,60).midicps].postln);
			4.wait;
			server.sendBundle(d, ["/s_new", "wah", 1001, 3,1004].postln);

			4.wait;
			server.sendBundle(d, ["/s_new", "echo", 1002, 3, 1001,\delay, 0.2, \decay, 4].postln);

			4.wait;
			server.sendBundle(d, ["/s_new", "ring-mod", 1003, 3,1002, \delay, 0.2, \decay, 4].postln);

			12.wait;

			// release the gates by setting them to 0, allowing synths to die
			server.sendBundle(d, ["/n_set", 1004, "gate", 0].postln);

			4.wait;
			server.sendBundle(d, ["/n_set", 1001, "gate", 0].postln);

			4.wait;
			server.sendBundle(d, ["/n_set", 1002, "gate", 0].postln);

			4.wait;
			server.sendBundle(d, ["/n_set", 1003, "gate", 0].postln);

			4.wait;
			server.sendBundle(d, ["/n_free", 1000].postln);
			1.wait;

			server.sendBundle(d, ["/g_freeAll", 0].postln);
		});

		1.wait;

	});
	SystemClock.play(r);
)

// default node def not written
//f = { arg o; Pbind(\note, Pxrand([0,2,3,5,7,9,10,12],inf), \octave, o, \dur, 0.25) };
//Ppar([f.value(7),f.value(6), f.value(5)]).play(Event.protoEvent)

Server.default.sendMsg("/notify", 1);
(
SynthDef("default", { arg freq=440, gate=1, amp=0.2;
	var e, s;
	e = EnvGen.kr(Env.linen(0.004,amp,0.5), gate);
	FreeSelfWhenDone.kr(e);
	s = SinOsc.ar(freq,0, e);
	Out.ar(0, s);
}).add;
)
