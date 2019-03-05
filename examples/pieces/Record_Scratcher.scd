// Josh Parmenter (2007)
// record scratcher

SynthDef(\scratch, {arg gate = 1, buffer;
	var buf, speed, env;
	env = EnvGen.kr(
		Env([0,1,0], [0.1, 0.1], \sin, 1),
		gate, doneAction: 20);
	speed = MouseX.kr(-10, 10);
	speed = speed - DelayN.kr(speed, 0.1, 0.1);
	speed = MouseButton.kr(1, 0, 0.3) + speed ;
	buf = PlayBuf.ar(1, buffer, speed * BufRateScale.kr(buffer), loop: 1);
	Out.ar(0, (buf * env).dup );
}).add;

s.boot;

// path to a mono soundfile here
a = Buffer.read(s, "sounds/a11wlk01.wav");

b = Synth(\scratch, [\buffer, a]);

// move mouse to scrub the record.
// press mouse button to 'stop the record', you can scrub while it is stopped.

// stop the synth
b.set(\gate, 0);
// free the Buffer
a.free;
