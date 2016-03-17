(
/*

RM octaver

inSignal is RingModulated by a sinusoidal tone with half frequency.
The resulting spectrum is given by all the components of inSignal with
half freqs.
This means that the new spectrum is a one 8ve below version of the input signal's one,
with only odd partials.
As a consequence, if inSignal is added again, even partials are
recovered.

See:
Miller Puckette, The Theory and Technique of Electronic Music, p. 126
http://crca.ucsd.edu/~msp/techniques/latest/book.pdf
http://crca.ucsd.edu/~msp/techniques/latest/book-html/node77.html#sect5.ringmod

andreavalle

*/

s = Server.local.waitForBoot({
	SynthDef.new(\RmOctaver, { var in, out = 0, freq, hasFreq ;
		in = SoundIn.ar(0) ;
		# freq, hasFreq = Pitch.kr(in) ;
		Out.ar(out, SinOsc.ar(freq: freq*0.5)*in+in);
	}).send(s) ;
})
)


Synth.new(\RmOctaver) ;
