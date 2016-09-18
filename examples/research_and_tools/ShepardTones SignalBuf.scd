

// shepard tones
// alberto de campo
(
var ampTable, freqTable, b1, b2;

ampTable = Signal.hanningWindow(1024).squared;
freqTable = Signal.newClear(1024).waveFill({ arg i; 0.5 ** i * 20000 }, 0, 10);

b1 = Buffer.loadCollection(s, freqTable);
b2 = Buffer.loadCollection(s, ampTable);

{
	var freqs, amps, phasor, ratescale, rate, phases;
	ratescale = 1024 / 44100 / 10;
	rate = 0.1;	// once per 10 sec
	phasor = Phasor.ar(0, rate * ratescale, 0, 1024);
	phases = (0..9) * 0.1 * 1024 + phasor;

	freqs = BufRd.ar(1, b1.bufnum, phases);
	amps = BufRd.ar(1, b2.bufnum, phases);

	Mix.ar(SinOsc.ar(freqs) * amps) * 0.1;
}.play;

)
