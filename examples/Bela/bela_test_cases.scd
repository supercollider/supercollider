Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

/// AnalogIn

// ak
(
SynthDef("AnalogIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(AnalogIn.ar(0).poll.exprange(200, 5000), 0, 0.1)
	)
}).add;
);

a = Synth("AnalogIn").postln;
a.free;

// ak
(
SynthDef("AnalogIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(AnalogIn.ar(
			Stepper.kr(Impulse.kr(1), 0, 0, 7, 1).poll
		).poll.exprange(200, 5000), 0, 0.1)
	)
}).add;
);

a = Synth("AnalogIn").postln;
a.free;

// kk
(
SynthDef("AnalogIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(AnalogIn.kr(
			0
		).poll.exprange(200, 5000), 0, 0.1)
	)
}).add;
);

a = Synth("AnalogIn").postln;
a.free;

// aa
(
SynthDef("AnalogIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(AnalogIn.ar(Stepper.ar(Impulse.ar(1), 0, 0, 7, 1).poll).poll.exprange(200, 5000), 0, 0.1)
	)
}).add;
);

a = Synth("AnalogIn").postln;
a.free;


/// AnalogOut

// aaa
(
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.ar(DC.ar(0), SinOsc.ar(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free

// aka
(
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.ar(0, SinOsc.ar(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free;

// aak
(
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.ar(DC.ar(0), SinOsc.kr(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free

// kk
(// diverted to kk - AnalogOut becomes .kr effectively
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.ar(0, SinOsc.kr(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free;

// kk
(
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.kr(0, SinOsc.kr(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free;

// kk + warning
(
SynthDef("AnalogOut", { arg out=0;
	AnalogOut.kr(0, SinOsc.ar(10, 0, 0.5, 0.5));
}).add;
);

a = Synth("AnalogOut").postln;
a.free;


// DigitalIn

// a
(
SynthDef("DigitalIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(500, 0, 0.1 * DigitalIn.ar(0))
	)
}).add;
);

a = Synth("DigitalIn").postln;
a.free;

// k
(
SynthDef("DigitalIn", { arg out=0;
	Out.ar(out,
		SinOsc.ar(500, 0, 0.1 * DigitalIn.kr(0))
	)
}).add;
);

a = Synth("DigitalIn").postln;
a.free;


// DigitalOut

// a
(
SynthDef("DigitalOut", { arg out=0;
	DigitalOut.ar(0, SinOsc.ar(10));
}).add;
);

a = Synth("DigitalOut").postln;
a.free;

// k + warning
(
SynthDef("DigitalOut", { arg out=0;
	DigitalOut.kr(0, SinOsc.ar(10));
}).add;
);

a = Synth("DigitalOut").postln;
a.free;

// k
(
SynthDef("DigitalOut", { arg out=0;
	DigitalOut.kr(0, SinOsc.kr(10));
}).add;
);

a = Synth("DigitalOut").postln;
a.free;

// k + warning
(
SynthDef("DigitalOut", { arg out=0;
	DigitalOut.ar(0, SinOsc.kr(10));
}).add;
);

a = Synth("DigitalOut").postln;
a.free;

// a-once
(
SynthDef("DigitalOut", { arg out=0;
	DigitalOut.ar(0, SinOsc.ar(10), 1);
}).add;
);

a = Synth("DigitalOut").postln;
a.free;


// DigitalIO

(// aaaa once - everything audio rate
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(DC.ar(0), SinOsc.ar(10), K2A.ar(LFPulse.kr(0.1))).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// aaak once - pinmode control rate
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(DC.ar(0), SinOsc.ar(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// aaka once - output value control rate
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(DC.ar(0), SinOsc.kr(10), K2A.ar(LFPulse.kr(0.1))).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// aakk once - output value control rate, pinmode control rate
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(DC.ar(0), SinOsc.kr(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// akaa once - pin changed at control rate, rest audio
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(0, SinOsc.ar(10), K2A.ar(LFPulse.kr(0.1))).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// akak once - pin changed at control rate, output value at audio rate, mode at control rate
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(0, SinOsc.ar(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// ak once - pin changed at control rate, output control rate, pinmode control
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(0, SinOsc.kr(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// akka once - pin changed at control rate, output control rate, pinmode control
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.ar(0, SinOsc.kr(10), K2A.ar(LFPulse.kr(0.1))).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// kk - warning
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.kr(0, SinOsc.kr(10), K2A.ar(LFPulse.kr(0.1))).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// kk - warning
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.kr(0, SinOsc.ar(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// kk - warning
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.kr(Stepper.ar(Impulse.ar(1), 0, 7, 1), SinOsc.kr(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

(// kk
SynthDef("DigitalIO", { arg out=0;
	DigitalIO.kr(0, SinOsc.kr(10), LFPulse.kr(0.1)).poll;
}).add;
);

a = Synth("DigitalIO").postln;
a.free;

//
s.freeAll;
