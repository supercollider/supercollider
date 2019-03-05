
// (thor magnusson) (c 2007)

// A reinterpretation of the Poème symphonique was composed by György Ligeti (in 1962)
// http://www.youtube.com/watch?v=QCp7bL-AWvw

(
SynthDef(\ligetignome, {arg tempo=1, filterfreq=1000, rq=1.0;
var env, signal;
	var rho, theta, b1, b2;
	b1 = 2 * 0.996839 * cos(0.0931624);
	b2 = 0.996839.squared.neg;
	signal = SOS.ar(Impulse.ar(tempo), 1.0, 0.0, 0.0, b1, b2);
	signal = RHPF.ar(signal, filterfreq, rq);
	Out.ar(0, Pan2.ar(signal, 0));
}).add;
)

// and we create 10 different metronomes running in different tempi
// (try with 3 metros or 30 metros)
(
13.do({
	Synth(\ligetignome).set(
		\tempo, (rrand(0.5,1.5)).reciprocal,
		\filterfreq, rrand(2000,4000),
		\rq, rrand(0.3,0.9) )
});
)

// -----------

// and something totally else... ligeti's brother:

(
var trig;
var wait = 4;
Task({
	20.do({
		trig = [1, 1.5].wchoose([0.7, 0.3]);
		{Resonz.ar(Impulse.ar(trig, 0, 50*rrand(5,10)), Rand(200,2000), 0.003) ! 2}.play;
		(wait + rrand(0.1,1)).wait;
		wait = wait - rrand(0.01, 0.2);
	});
	"thank you! you can now stop this program at any point!".postln;
}).play
)
