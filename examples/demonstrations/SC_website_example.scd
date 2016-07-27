/*
// the old sc website example
This example comes from supercollider.sourceforge.net as of 2003. It uses a loop to generate 40 different variations on a SynthDef, and then a process to play them.
*/
(
40.do({ arg i;
	SynthDef(
	"rperc" ++ i.asString,
	{ arg i_bus = 0, amp = 0.1, rate = 1;
		var n = 12;
		var exc, out;
		exc = // exitation signal
			  // .ar or .kr means it's a unit generator
			WhiteNoise.ar *
			Decay.kr(
			  Impulse.kr(0,0,amp*0.1),
			  rrand(0.2,1.0)
			);
	  out = Klank.ar(`[   // resonator
		{exprand(100.0, 10000.0)}.dup(n),
		{ rrand(0.1,1.0) }.dup(n),
		{exprand(0.05,1.0)}.dup(n)
	  ], exc, rate);
	  DetectSilence.ar(out, 0.0001, 0.1, 2);
	  Out.ar( // output to bus number i_bus
		i_bus,
		Pan2.ar( out, rrand(-1.0,1.0))
	  );
  }).add;
});
)

// a process to use them.

(
var s;
s = Server.local;
Task({
  var dur=0.2, inst = \rperc0, amp = 0.05;
  inf.do({	// do forever
	  // 30% chance of picking new synth
	if (0.3.coin, {
	  inst = "rperc" ++ 40.rand.asString;
	  amp = exprand(0.02,0.2) * 0.5;
	});
	// allocate new synth on server
	s.sendBundle(0.2, [
	  \s_new, inst, -1, 0, 0,
	  \amp, amp,
	  \rate, exprand(0.5,2)
	]);
	if (dur.coin, {
		dur = 0.1 + 1.9.rand
	});
	dur.wait;
  }); // inf.do
}).play(TempoClock.default);
)
