
(
SynthDef(\demanding, { arg impulsefreq = 10;
	var freq, trig, reset, seq1, seq2, signal;
	trig = Impulse.kr(impulsefreq);
	seq1 = Drand([72, 75, 79, 82] - 12, inf).midicps; 
	seq2 = Dseq([72, 75, 79, Drand([82,84,86])], inf).midicps; 
	freq = Demand.kr(trig, 0, [seq1, seq2]);
	signal = (SinOsc.ar(freq + [0,0.7]) + Saw.ar(freq + [0,0.7], 0.3)).distort.log.distort.cubed * 0.1;
	
	Out.ar(0, signal);
}).add;
)


x = Synth.new(\demanding)

x.set(\impulsefreq, 6)
x.set(\impulsefreq, 4)
x.set(\impulsefreq, 2)
x.set(\impulsefreq, 1)
