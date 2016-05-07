//Sound recipes from:
//Mitchell Sigman (2011) Steal this Sound. Milwaukee, WI: Hal Leonard Books
//adapted for SuperCollider and elaborated by Nick Collins (http://www.sussex.ac.uk/Users/nc81/index.html)
//under GNU GPL 3 as per SuperCollider license

//if you see strange multipliers, like *0.3, these are compensation factors for mixing together many oscillators, or loss of amplitude from filters so the patches work out to roughly equal output



//start here
(
//make sure server is booted if you want to hear any sound!
s.boot;

//bpm 120 assumed in the following, impacting on some delay recipes.
TempoClock.default.tempo = 2;
)




//pp.2-3

(
SynthDef(\prophet5pwmstrings,{|out= 0 freq = 440 amp = 1.0 gate=1 lforate = 10 lfowidth= 0.5 cutoff= 12000 rq=0.5 pan = 0.0|

	var lfo, pulse, filter, env;

	lfo = LFTri.kr(lforate*[1,1.01],Rand(0,2.0)!2);

	pulse = Pulse.ar(freq*[1,1.01],lfo*lfowidth+0.5);

	filter = RLPF.ar(pulse,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.0,1.0,0.5),gate,doneAction:2);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp*0.5,pan));

}).add
)


(
Pbind(
	\instrument,\prophet5pwmstrings,
	\midinote,Pseq([0,3,8,7,5,8,7,3]+48,inf),
	\dur,Pseq((0.5!8) ++ (0.25!8) ++ (2.0!8),inf),
	\lfowidth,Pn(Pseries(0.0,0.025,7),inf),
	\lforate,Pn(Pseries(2,1,11),inf),
	\rq,Pn(Pseries(0.9,-0.1,5),inf)
).play
)



//pp. 8-9


//vibrato on oscillator
(
SynthDef(\singleoscillatorwobble,{|out= 0 freq = 440 amp = 1.0 gate=1 lforate = 10 lfowidth= 0.5 cutoff= 12000 rq=0.5 pan=0.0|

	var lfo, pulse, filter, env;

	lfo = LFTri.kr(lforate,Rand(0,2.0)!2);

	pulse = Pulse.ar(freq*(1.0+(lfowidth*lfo)),0.5);

	filter = RLPF.ar(pulse,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.0,1.0,0.5),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp*0.5,pan));

}).add;


SynthDef(\choruseffect, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var chorus;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	chorus= Mix.fill(7, {

		var maxdelaytime= rrand(0.005,0.02);

		DelayC.ar(source, maxdelaytime,LFNoise1.kr(Rand(4.5,10.5),0.25*maxdelaytime,0.75*maxdelaytime) )

	});

	XOut.ar(out,env, chorus);

}).add;

)



(
Pmono(
	\singleoscillatorwobble,
	\midinote,Pseq([0,3,8,7,5,8,7,3]+48,inf),
	\dur,Pseq((0.5!8) ++ (1.0!8) ++ (2.0!8),inf),
	\lfowidth,Pn(Pseries(0.0,0.01,7),inf),
	\lforate,Pn(Pseries(2,1,11),inf),
	\rq,Pn(Pseries(0.9,-0.1,5),inf)
).play
)


(
var p =
Pbind(
	\instrument, \singleoscillatorwobble,
	\tempo,2,
	\midinote,Pseq([0,3,8,7,5,8,7,3]+36,24),
	\dur,Pseq((0.5!8) ++ (1.0!8) ++ (2.0!8),1),
	\lfowidth,Pn(Pseries(0.0,0.01,7),4),
	\lforate,Pn(Pseries(2,1,11),3),
	\rq,Pn(Pseries(0.9,-0.1,5),6)
);


Pseq([p,Pfx(p,\choruseffect)],inf).play
)



//trying it on its own
a = Synth(\singleoscillatorwobble);
a.release






//pp. 10-11

(
SynthDef(\trianglewavebells,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 10 lfowidth= 0.0 cutoff= 100 rq=0.5 pan=0.0|

	var osc1, osc2, vibrato, filter, env;

	vibrato = SinOsc.ar(lforate,Rand(0,2.0));

	osc1 = Saw.ar(freq*(1.0+(lfowidth*vibrato)),0.75);

	//Saw a bit rough, possibly slighter smoother:
	//osc1 = DPW4Saw.ar(freq*(1.0+(lfowidth*vibrato)),0.5);

	osc2 = Mix(LFTri.ar((freq.cpsmidi+[11.9,12.1]).midicps));

	//filter = (osc1+(osc2*0.5))*0.5; //no filter version
	filter = RHPF.ar((osc1+(osc2*0.5))*0.5,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.1,1.0,0.5),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;
)



(
Pbind(
	\instrument, \trianglewavebells,
	\sustain,0.6,
	\amp, 0.2,
	\midinote,Pseq([0,7,3,0, 8,7,8,5, 7,12,5,3, 12,7,15,-5]+60,inf),
	\dur,Pn(Pshuf([0.5,1.0,0.5,2.0,0.5,1.0,0.5,1.5],1),inf),
	\lfowidth,Pn(Pseries(0.0,0.005,16),inf),
	\lforate,Pn(Pseries(1,0.5,16),inf),
	\rq,Pn(Pseries(0.9,-0.1,8),inf),
	\cutoff,Pn(Pseries(60,10,9),inf)
).play
)






//pp.12-3

//essentially, Pulse waveforms in multiple octaves; I've refined the patch to add freq*[1,2,3] which gives octave and octave + fifth over fundamental
(
SynthDef(\organdonor,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 10 lfowidth= 0.0 cutoff= 100 rq=0.5 pan=0.0|

	var vibrato, pulse, filter, env;

	vibrato = SinOsc.ar(lforate,Rand(0,2.0));

	//up octave, detune by 4 cents
	//11.96.midiratio = 1.9953843530485
	//up octave and a half, detune up by 10 cents
	//19.10.midiratio = 3.0139733629359

	//Pulse version
	//pulse = Mix(Pulse.ar(([1,1.9953843530485,3.0139733629359]*freq)*(1.0+(lfowidth*vibrato)),Rand(0.4,0.6)!3,[1.0,0.7,0.3]))*0.5;

	//better alternative
	pulse = Mix(VarSaw.ar(([1,1.9953843530485,3.0139733629359]*freq)*(1.0+(lfowidth*vibrato)),Rand(0.0,1.0)!3,Rand(0.3,0.5)!3,[1.0,0.7,0.3]))*0.5;

	filter = RLPF.ar(pulse,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.5,1.0,0.5),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;
)





(
Pbind(
	\instrument, \organdonor,
	\sustain,0.9,
	\amp, 0.2,
	\midinote,Pn(Pshuf([[0,4,7],[-1,2,7],[-3,0,5],[-1,2,7],[2,5,9],[-5,-1,4]]+60),inf),
	\dur,Pn(Pshuf([2.0,2.0,4.0,2.0],1),inf),
	\lfowidth,Pn(Pseries(0.0,0.001,16),inf),
	\lforate,Pn(Pseries(1,0.25,16),inf),
	\rq,Pn(Pseries(0.3,-0.01,8),inf),
	\cutoff,Pn(Pseries(6000,200,9),inf)
).play
)






//pp. 14-5

(
SynthDef(\werkit,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff= 100 rq=0.1 pan=0.0|

	var source, filter, env;

	source = WhiteNoise.ar;

	filter = BLowPass4.ar(source,freq,rq)*0.3;

	//env = EnvGen.ar(Env.adsr(0.01,0.0,1.0,0.1),gate,doneAction:2);
	//no gate, fixed envelope size
	env = EnvGen.ar(Env([0,1,0.5,0.0],[0.02,0.1,0.1]),doneAction:2);

	Out.ar(out,Pan2.ar((0.7*filter+(0.3*filter.distort))*env*amp,pan));

}).add;



//via Comb filter for feedback
SynthDef(\delayeffect, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var delay;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	delay= CombC.ar(source,0.25,0.25,2.0);

	XOut.ar(out,env, delay);

}).add;

)




//p15 book claims 'musically correct' half steps; total nonsense. I've used their suggested steps here, but the original non-pitch quantised version could be explored.
(
Pfx(
	Pbind(
		\instrument, \werkit,
		\amp, 0.2,
		\midinote,Pseq(([-1,1,-1,1,3,6,9,11]+60)++([0,3,5,6,7,8,11,14]+72),inf),
		\dur,0.25,
		\rq,0.01
	),
	\delayeffect
).play
)







//their lasers on pp 16-17 seem just to be fast sweeps of frequency of one oscillator by a modulator oscillator, or an envelope.
(

//no use of gate, fixed length
SynthDef(\laserbeam,{|out= 0 freq = 440 amp = 0.1 attackTime= 0.04 gate=1 pan=0.0|

	var osc1, freqenv, ampenv;

	freqenv = EnvGen.ar(Env([4,0.5,1,1],[attackTime,0.01,1.0]));

	osc1 = LFTri.ar(freq*freqenv);

	//env = EnvGen.ar(Env.adsr(0.01,0.0,1.0,0.1),gate,doneAction:2);
	//no gate, fixed envelope size
	ampenv = EnvGen.ar(Env([0,1,0.5,0.0],[0.02,0.2,0.1]),doneAction:2);

	Out.ar(out,Pan2.ar(osc1*ampenv*amp,pan));

}).add;

)



(
Pfx(
	Pfx(
		Pbind(
			\instrument, \laserbeam,
			\amp, 0.2,
			\midinote,Pseq([36,48,60,72,84],inf),
			\dur,0.5,
			\attackTime,Pstutter(8,Pseq([Pshuf((0.01,0.02..0.1),1)],inf))
		),
		\delayeffect
	),
	\choruseffect
).play

)





//pp. 18-19

(

//no use of gate, fixed length
SynthDef(\moogbasstone,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff= 1000 gain=2.0 lagamount = 0.01 pan=0.0|

	var osc, filter, env, filterenv;

	osc = Mix(VarSaw.ar(freq.lag(lagamount)*[1.0,1.001,2.0],Rand(0.0,1.0)!3,Rand(0.5,0.75)!3,0.33));

	//alternative: richer source: see moogbasstone2 below
	//osc = Mix(Pulse.ar(freq.lag(lagamount)*[1.0,1.001,2.0],Rand(0.45,0.5)!3,0.33));

	filterenv = EnvGen.ar(Env.adsr(0.2,0.0,1.0,0.2),gate,doneAction:2);
	filter =  MoogFF.ar(osc,cutoff*(1.0+(0.5*filterenv)),gain);

	env = EnvGen.ar(Env.adsr(0.001,0.3,0.9,0.2),gate,doneAction:2);

	Out.ar(out,Pan2.ar((0.7*filter+(0.3*filter.distort))*env*amp*1.5,pan));

}).add;

)


(
Pfx(
	Pfx(
		Pmono(
			\moogbasstone,
			\amp, 0.5,
			\midinote,Pseq([24,36,48,36,35,36,43,48],inf),
			\dur,Pstutter(8,Pseq([0.5,0.25],inf)),
			\gain,Pn(Pseries(2,0.1,19),inf),
			\cutoff,Pn(Pseries(4000,400,18),inf)
		),
		\delayeffect
	),
	\choruseffect
).play

)





(

//no use of gate, fixed length
SynthDef(\moogbasstone2,{|out= 0 freq = 440 amp = 0.1 gate=1 attackTime= 0.2 fenvamount=0.5 cutoff= 1000 gain=2.0 pan=0.0|

	var osc, filter, env, filterenv;

	//alternative: richer source
	osc = Mix(Pulse.ar(freq.lag(0.05)*[1.0,1.001,2.0],Rand(0.45,0.5)!3,0.33));

	filterenv = EnvGen.ar(Env.adsr(attackTime,0.0,1.0,0.2),gate,doneAction:2);
	filter =  MoogFF.ar(osc,cutoff*(1.0+(fenvamount*filterenv)),gain);

	env = EnvGen.ar(Env.adsr(0.001,0.3,0.9,0.2),gate,doneAction:2);

	Out.ar(out,Pan2.ar((0.7*filter+(0.3*filter.distort))*env*amp,pan));

}).add;

)


(
Pfx(
	Pfx(
		Pmono(
			\moogbasstone2,
			\amp, 0.8,
			\midinote,Pseq([24,36,43,48, 43,48,36,36, 36,36,39,36, 31,31,31,31, 31,34,31,34],inf),
			\dur,0.25,
			\gain,Pn(Pseries(2,0.1,19),inf),
			\cutoff,Pstutter(3,Pn(Pseries(50,250,40),inf)),
			\attackTime,Pn(Pseries(0.0,0.01,30),inf),
			\fenvamount,Pstutter(4,Pn(Pseries(0.0,0.05,20),inf))
		),
		\delayeffect
	),
	\choruseffect
).play

)







//pp. 20-21  mr ostinato


(
SynthDef(\mrostinato,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 10 lfowidth= 0.5 pan = 0.0|

	var lfo, pulse, filter, env;

	lfo = LFTri.kr(lforate,Rand(0,2.0)!3);

	pulse = Pulse.ar(freq*[1,1.01,0.5],lfo*lfowidth+0.5);

	env = EnvGen.ar(Env.adsr(0.01,0.05,0.5,0.1),gate,doneAction:2);

	Out.ar(out,Pan2.ar(Mix(pulse)*env*amp,pan));

}).add
)


(
PmonoArtic(
	\mrostinato,
	\octave,Pseq([3,3,4,4],inf),
	\scale,[0,2,3,5,7,8,10],
	\degree,Pstutter(16,Pseq([0,2,5,3],inf)),
	\dur,0.25,
	\lfowidth,Pstutter(8,Pn(Pseries(0.0,0.05,7),inf)),
	\lforate,Pstutter(5,Pn(Pseries(0.5,0.2,11),inf)),
	\pan,Pstutter(2,Prand([-0.5,-0.3,0.3,0.5],inf))
).play
)


 //richer sequence with some heavier moments
(
Pbind(
	\instrument,\mrostinato,
	\sustain,Pstutter(64,Prand([0.1,0.5,0.7],inf)),
	\octave,Pseq([3,3,4,4,3,3,5,2],inf),
	\scale,[0,2,3,5,7,8,10],
	\degree,Pstutter(16,Prand([0,-1,1,2,-3,5,6,3,4,2],inf)),
	\dur,0.25,
	\lfowidth,Pstutter(8,Pn(Pseries(0.0,0.05,7),inf)),
	\lforate,Pstutter(5,Pn(Pseries(0.5,0.2,11),inf)),
	\pan,Pstutter(2,Prand([-0.5,-0.3,0.3,0.5],inf))
).play
)





//LinnDrum:
// p. 23 get the samples from http://machines.hyperreal.org/manufacturers/Linn/LinnDrum/
//The website is a nice resource; there are other sample kits there like Roland TR-909 and TR-808
 //I've just demoed a patch with three samples (kick, snare, hat): I've used Patterns below, though my personal preference would be explicit Busses, Groups, Synths and {}.fork for full control of routings


 //load samples (assumed mono)
(
var basepath = "/data/audio/percussion/linndrum/";

b = ["kick","sd","chh"].collect{|val| Buffer.read(s,basepath++val++".wav") };

//b[0].numChannels.postln; should be 1 for mono

)



(

SynthDef(\sampleplay,{|out= 0 bufnum = 0 amp = 0.1 gate=1 pan = 0.0|

	var playbuf, env;

	playbuf = PlayBuf.ar(1,bufnum);

	env = EnvGen.ar(Env.adsr(0.0,0.0,1.0,0.1),gate,doneAction:2);

	Out.ar(out,Pan2.ar(playbuf*env*amp,pan));

}).add;

SynthDef(\samplecompress,{|out =0 gain=2 reduction=8 ratio=3 gate= 1 attackTime=0.016 relaxTime=0.05|

	var source = In.ar(out,2);
	var compression;
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);

	compression= Compander.ar(2*source,source,(-8).dbamp,1.0,ratio.reciprocal,attackTime,relaxTime);

	XOut.ar(out,env, compression);

}).add;

SynthDef(\sampleeq1,{|out =0 gate= 1|

	var source = In.ar(out,2);
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);
	var eq;

	eq= BLowShelf.ar(source,100,1.0,3);
	eq= BPeakEQ.ar(eq,600,1.0,-3);

	XOut.ar(out,env,eq);

}).add;

SynthDef(\sampleeq2,{|out =0 gate= 1|

	var source = In.ar(out,2);
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);
	var eq;

	eq= BHiPass(150,0.3);

	XOut.ar(out,env,eq);

}).add;


SynthDef(\samplereverb,{|out =0 gate= 1|

	var source = In.ar(out,2);
	var env = Linen.kr(gate, 0.0, 0.3, 0.1, 2);
	var reverb;

	reverb= FreeVerb.ar(source,1.0,0.6,0.6);

	XOut.ar(out,env,reverb);

}).add;


)



(
var kick, snare,hat;

s.latency= 0.1;

kick = Pbind(
	\instrument, \sampleplay,
	\bufnum,b[0],
	\dur,1.0,
	\pan,0.0,
	\amp, 0.5
);


snare = Pbind(
	\instrument, \sampleplay,
	\bufnum,b[1],
	\dur,Pseq([1.25,0.75,2.0],inf),
	\bus,16,
	\amp,0.45,
	\pan,0.0
);

hat = Pbind(
	\instrument, \sampleplay,
	\bufnum,b[2],
	\dur,Pseq(0.5!8++(0.25!16),inf),
	\amp, 0.15,
	\pan,Pseq(0.3!8++((-0.3)!16),inf)
);


//Pfxb organises private busses for each sound
Ptpar([
	0.0,
	Pfxb(Pfx(kick,\samplecompress),\sampleeq1),
	1.0,
	Pfxb(Pfx(snare,\samplecompress,\gain,1,\reduction,10,\ratio,2,\attackTime,0.02),\samplereverb),
	0.0,
	Pfxb(hat,\sampleeq2)
]).play


)










//p.24-5 super (un)natural synths

//book claims 5.9KHz LFO rate: possible typo...
(
SynthDef(\plastickystrings,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 5900 lfowidth= 0.01 cutoff= 12000 rq=0.5 pan=0.0|

	var lfo, saw, filter, env;

	lfo = LFTri.ar(lforate,Rand(0,2.0));

	saw = Saw.ar(freq*(1.0+(lfowidth*lfo)),0.5);

	//filter = BBandPass.ar(saw,freq,4);
	filter = BHiPass.ar(saw,freq,rq); 	//or using cutoff

	env = EnvGen.ar(Env.adsr(0.4,0.0,1.0,1.0),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;


SynthDef(\choruseffect2, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var chorus;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	chorus= Mix.fill(17, {
		var maxdelaytime= rrand(0.005,0.02);
		DelayC.ar(source, maxdelaytime,LFNoise1.kr(Rand(4.5,10.5),0.25*maxdelaytime,0.75*maxdelaytime) )

	});

	XOut.ar(out,env,0.5*chorus);

}).add;

)


//goldfrapp number 1 riff
(
Pfx(
	Ppar([
		Pbind(
			\instrument, \plastickystrings,
			\amp, 0.2,
			[\midinote,\dur],Pseq([[72,1.5],[68,6.5],[72,1.5],[68,1.5],[61,5.0]],inf),
			\lfowidth,Pn(Pseries(0.0,0.001,16),inf),
			\lforate,5900, //5.9?
			\rq,Pn(Pseries(0.9,-0.05,8),inf)
		),
		Pbind(
			\instrument, \plastickystrings,
			\amp, 0.3,
			[\midinote,\dur],Pseq([[44,8.0],[37,8.0]],inf),
			\lfowidth,Pn(Pseries(0.0,0.001,16),inf),
			\lforate,5.9,
			\rq,0.9
		),
	]),
	\choruseffect2
).play
)






//pp. 26-7

(
SynthDef(\handclapsmash,{|out= 0 amp = 0.1 gate=1 cutoff= 12000 rq=0.5 releaseTime=0.5 pan=0.0|

	var noise, filter, filterenv, env;

	noise = WhiteNoise.ar;

	filterenv = EnvGen.ar(Env([0.0,1.0,0.3,0.0],[0.0,0.3,0.3]));
	//this involves a gate control, less use here
	//filterenv = EnvGen.ar(Env.adsr(0.0,0.3,0.3,0.3),gate);

	filter = BHiPass.ar(RLPF.ar(noise,cutoff*filterenv,rq),150);

	//body
	filter = BPeakEQ.ar(filter,440,1.0,8);
	//snap
	filter = BPeakEQ.ar(filter,1200,1.0,14);

	//no gate, fixed percussion sound
	env = EnvGen.ar(Env.perc(0.005,releaseTime),doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;


//with dirty attack
SynthDef(\handclapsmash2,{|out= 0 amp = 0.1 gate=1 cutoff= 12000 cutoff2=10000 rq=0.5 modamount=0.0 modrate=50 releaseTime=0.7 pan=0.0|

	var noise, filter, filterenv, modulation, env;

	noise = WhiteNoise.ar;

	filterenv = EnvGen.ar(Env([0.0,1.0,0.3,0.0],[0.0,0.3,0.3]));

	modulation = (LFSaw.ar(modrate).range(0.0,1.0))*EnvGen.ar(Env([1,1,0,0],[0.1,0.0,1.0]));

	//filter = BHiPass.ar(RLPF.ar(noise,cutoff*filterenv*(1.0+(modamount*modulation)),rq),150); //alternative
	filter = BHiPass.ar(RLPF.ar(noise,cutoff*filterenv+ (cutoff2*modamount*modulation),rq),150);

	//body
	filter = BPeakEQ.ar(filter,440,1.0,8);
	//snap
	filter = BPeakEQ.ar(filter,1200,1.0,14);

	//no gate, fixed percussion sound
	env = EnvGen.ar(Env.perc(0.005,releaseTime),doneAction:2);

	//amp modulation too not very effective ((1.0-modamount)+(modamount*modulation))
	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;


)




(
Pbind(
	\instrument,\handclapsmash,
	\dur,0.5,
	\rq,1.0
).play
)



(
Pbind(
	\instrument,\handclapsmash2,
	\dur,0.5,
	\cutoff,Pstutter(2,Pn(Pseries(12000,-1000,11),inf)),
	\cutoff2, Pstutter(3,Pn(Pseries(10000,-1000,9),inf)),
	\rq,1.0,
	\modrate,Pstutter(8,Pn(Pseries(25,5,7),inf)),
	\modamount,Pstutter(4,Pseq([0.1,0.3,0.5,0.6,0.7,0.9],inf)),
	\releaseTime,Pstutter(2,Pn(Pseries(0.8,-0.1,6),inf))
).play
)




//pp. 30-31 Synth bass layers

(
SynthDef(\bassfoundation,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff= 1000 rq=0.5 pan=0.0|

	var osc, filter, env, filterenv;

	osc = Saw.ar(freq);

	filterenv = EnvGen.ar(Env.adsr(0.0,0.5,0.2,0.2),gate,doneAction:2);
	filter =  RLPF.ar(osc,cutoff*filterenv+100,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.0,0.9,0.05),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp*2,pan));

}).add;


SynthDef(\basshighend,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff= 3000 rq=0.1 drive = 2.0 pan=0.0|

	var osc, filter, env, filterenv;
	var ab;

	//osc = Mix(VarSaw.ar(freq*[0.25,1,1.5],Rand(0.0,1.0)!3,0.9,[0.5,0.4,0.1]));
	osc = Mix(Saw.ar(freq*[0.25,1,1.5],[0.5,0.4,0.1]));
	//osc = Mix(DPW4Saw.ar(freq*[0.25,1,1.5],[0.5,0.4,0.1]));
	filterenv = EnvGen.ar(Env.adsr(0.0,0.5,0.2,0.2),gate,doneAction:2);
	filter =  RLPF.ar(osc,cutoff*filterenv+100,rq);

 	//distortion
 	//filter = filter.distort.softclip;

	ab = abs(filter);
 	filter = (filter*(ab + drive)/(filter ** 2 + (drive - 1) * ab + 1));

	//remove low end
	filter = BLowShelf.ar(filter,300,1.0,-12);
	//dip at 1600Hz
	filter = BPeakEQ.ar(filter,1600,1.0,-6);

	env = EnvGen.ar(Env.adsr(0.01,0.0,0.9,0.05),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp*2,pan));

}).add;

)




(
Pbind(
	\instrument,\bassfoundation,
	\midinote,36,
	\dur,0.5,
	\rq,1.0
).play
)


(
Pbind(
	\instrument,\basshighend,
	\midinote,36,
	\dur,0.5,
	\rq,1.0
).play
)


//combination
(
p = Pbind(
	\midinote,Pstutter(4,Pseq([36,43,39,31],inf)),
	\dur,0.5,
	\rq,Pstutter(4,Pn(Pseries(1.0,-0.1,8),inf)),
	\cutoff,Pstutter(4,Pn(Pseries(8000,-1000,7),inf))
);

Ppar([
	Pset(\instrument,Pseq([\bassfoundation],inf),p),
	Padd(\instrument,\basshighend,p)
]).play;
)









//pp. 32-33

(

SynthDef(\winwoodlead,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.8 lfowidth=0.01 lforate= 8 lagamount=0.01 pan=0.0|

	var pulse, filter, env, lfo;

	lfo = LFTri.kr(lforate,Rand(0,2.0)!2);

	pulse = Mix(Pulse.ar((freq.lag(lagamount))*[1,1.001]*(1.0+(lfowidth*lfo)),[0.2,0.19]))*0.5;

	filter =  RLPF.ar(pulse,cutoff,rq);

	//remove low end
	filter = BLowShelf.ar(filter,351,1.0,-9);

	env = EnvGen.ar(Env.adsr(0.01,0.0,0.9,0.05),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;

)



//Pmono allows use of the lag between setting frequency values
(
Pmono(
	\winwoodlead,
	\midinote,Pseq([Pshuf([60,76,77,76,74,72],2)],inf),
	\dur,Pseq([0.5,1.0,0.5,0.5,0.5,1.0],inf),
	\amp,Pseq([0,1,1,1,1,1],inf),
	\lagamount,0.1,
	\lfowidth,Pstutter(6,Pseq([0.0,0.01,0.02,0.03,0.05,0.1],inf)),
	\cutoff,Pstutter(6,Pn(Pgeom(2000,1.5,5),inf))
).play
)






//pp34-37

(

SynthDef(\spookystrings,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.2 lfowidth=0.01 lforate=3.8 pan=0.0|

	var pulse, filter, env, lfo;

	lfo = LFTri.kr(lforate,Rand(0,2.0)!2);

	//LFO only on second oscillator
	pulse = Mix(Saw.ar(freq*[1,1.001]*[1.0,1.0+(lfowidth*lfo)]))*0.5;

	filter = BHiPass.ar(pulse,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.1,0.0,1.0,3),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;

SynthDef(\spookyeffect, {|out =0 gate= 1 delaytime=0.25 phaserate=0.1|
	var source = In.ar(out,2);
	var delay,phaser;

	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);

	delay= DelayN.ar(source,0.8,delaytime);

	phaser = delay;
	//phaser = AllpassN.ar(delay,0.04,SinOsc.kr(phaserate,0,0.005,0.035)); //not working that well, too obvious, need to make more subtle

	Out.ar(out,phaser);

}).add;

SynthDef(\spookystrings2,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.2 pan=0.0|

	var saw, pulse, filter, env, lfo, pulsewidthlfo;

	lfo = LFTri.kr([3.5,5],Rand(0,2.0)!2);

	pulsewidthlfo = SinOsc.kr(4.5);

	saw = Saw.ar(freq*(1.0+(0.052*lfo[0])));

	pulse = Pulse.ar(freq*(1.0+(0.072*lfo[1])),pulsewidthlfo*0.25+0.5);

	//combine, equal mix
	pulse = 0.5*(saw+pulse);

	//filter specification not the clearest, so just going for slow attack to brightness
	filter = BLowPass.ar(pulse,XLine.kr(678,cutoff,0.378),rq);

	env = EnvGen.ar(Env.adsr(0.1,0.0,1.0,3),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;

)

(
Pfx(
	Pbind(
		\instrument, \spookystrings,
		\midinote,Pseq([69,67,62,59,55],inf),
		\dur,Pseq([9.5,1.5,1.0,1.5,2.5],inf),
		\amp,0.2,
		\lfowidth,0.04,
		\cutoff,1000,
		\rq,0.1
	),
	\spookyeffect
).play
)


(
Pfx(
	Pbind(
		\instrument,\spookystrings2,
		\midinote,Pseq([69,67,62,59,55],inf),
		\dur,Pseq([5.5,1.5,1.0,1.5,2.5],inf),
		\amp,0.2,
		\cutoff,10000,
		\rq,0.5
	),
	\spookyeffect
).play
)










//pp34-37

(

SynthDef(\synchronicity,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.2 pan=0.0|

	var pulse, filter, env, slavefreq;

	slavefreq = EnvGen.ar(Env([1,5,1],[0.04,0.9]))*freq;

	pulse = SyncSaw.ar(freq,slavefreq);

	filter = BLowPass.ar(pulse,cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.01,0.0,1.0,0.1),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;

)


(
Pbind(
	\instrument,\synchronicity,
	\midinote,Pseq([57,59,62,62,61,59,57],inf),
	\dur,Pseq([1.0,1.0,2.0,1.0,0.5,1.0,1.5],inf),
	\amp,0.2,
	\cutoff,7000,
	\rq,1.0
).play
)






//pp. 40-41

(

SynthDef(\situationsynth,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.8 lfowidth=0.001 lforate= 3.3 pan=(-0.1)|

	var pulse, filter, env, filterenv, lfo;

	lfo = LFTri.kr(lforate,Rand(0,2.0)!2);

	pulse = Mix(Pulse.ar((((freq.cpsmidi)+[0,0.14])+(lfo*lfowidth)).midicps,[0.5,0.51]+(lfowidth*lfo)))*0.5;

	filterenv = EnvGen.ar(Env([0.0,1.0,0.3,0.0],[0.005,0.57,0.1],-3));

	filter =  RLPF.ar(pulse,100+(filterenv*cutoff),rq);

	env = EnvGen.ar(Env.adsr(0.002,0.57,1.0,0.3),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;


SynthDef(\yazoodelayeffect, {|out =0 gate= 1 pan= 0.1|
	var source = In.ar(out,2);
	var delay;
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);

	delay= DelayC.ar(source[0].distort,0.25,0.25);

	Out.ar(out,Pan2.ar(delay*env,pan));

}).add;

)


//says original tempo 118, we'll keep 120 for note as it's so close.

//Pmono allows use of the lag between setting frequency values
(
Pfx(
	Pbind(
		\instrument,\situationsynth,
		\midinote,Pseq([1,1,13,1,-1,-1,-1,11,8,11,13,1,1,13,1,-1,-1,11,16,15,11,13]+60,inf),	\dur,Pseq([0.5,0.5,0.5,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.25,0.5,0.5,0.5,0.25,0.5,0.5,0.25,0.25,0.25,0.25,0.25],inf),
		\lfowidth,0.2,
		\cutoff,6000,
		\rq,0.6,
		\pan,-0.1,
		\amp,0.3
	),
	\yazoodelayeffect
).play
)









//pp. 42-3 not an exact attempt, but some resonance and distortion fun

(

SynthDef(\ressquares,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.8 pan=(-0.1)|

	var pulse, filter, env;

	//2 cents detune for second oscillator
	pulse = Mix(Pulse.ar( ((freq.cpsmidi)+[0,0.02]).midicps, 0.5))*0.5;

	filter =  BLowPass.ar(pulse,100+cutoff,rq);

	env = EnvGen.ar(Env.adsr(0.002,0.1,1.0,0.2),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;


SynthDef(\synthdistortion, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);
	var abs, excess,output;

	abs = source.abs;

	excess = (abs-0.1).max(0.0).min(0.9)/0.9;

	//original plus sinusoidal perturbation of amount based on absolute amplitude
	output = source+(excess*(sin(excess*2pi*5)*0.5-0.5));

	XOut.ar(out,env,output*env);

}).add;

)


//quite loud, be careful
(
Pfx(
	Pbind(
		\instrument,\ressquares,
		\midinote,Pseq([12,0,0,0,3,0,7,0]+36,inf),
		\dur,0.25,
		\amp,Pstutter(8,Pn(Pseries(0.2,0.08,8),inf)),
		\cutoff,Pstutter(8,Pn(Pseries(100,125,11),inf)),
		\rq,Pstutter(4,Pn(Pseries(0.2,-0.02,9),inf)),
		\pan,-0.1
	),
	\synthdistortion
).play
)







//pp.44-45

(

SynthDef(\whoorgan,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff=8000 rq=0.8 pan=(-0.1)|

	var osc, filter, env;
	var basefreq =  ((freq.cpsmidi)+[0,0.1]).midicps;

	//osc = Mix(Blip.ar(basefreq,16))*0.5;

  	osc = Mix.fill(10,{|i| SinOsc.ar(basefreq*(i+1),0.0,(0.25+(0.75*LFNoise0.kr(10.1+i,0.5,0.5)))/(1.0+(0.5*i)))})*0.2;

	filter =  BLowPass.ar(osc,2000+(amp*cutoff),rq);

	env = EnvGen.ar(Env.adsr(0.02,0.1,0.9,1.0),gate,doneAction:2);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp,pan));

}).add;


SynthDef(\filtereffect, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var env = Linen.kr(gate, 0.0, 1, 0.1, 2);
	var output;

	output = BLowPass.ar(BLowPass.ar(source,SinOsc.ar(0.25,0,5000,5500),0.4),(200+(5000*SinOsc.ar(4.01,0.5,0.5,0.5))),0.6);

	//output = BLowPass.ar(source,SinOsc.ar(0.275,0,5000,5500),0.1);

	XOut.ar(out,env,output*env);

}).add;

)


(
Pfx(
	Pbind(
		\instrument,\whoorgan,
		\midinote,Pseq([7,5,4,0]+60,inf),
		\dur,0.5,
		\amp,Pstutter(8,Pn(Pseries(0.3,0.05,8),inf)),
		\cutoff,8000
	),
	\filtereffect
).play
)




//pp. 46-7

(
SynthDef(\eightoeightkick,{|out= 0 freq = 440 amp = 0.1 ringTime=10.0 releaseTime=1.0 distortion = 0.1 pan=(-0.1)|

	var impulse, filter, env;

  	impulse = Impulse.ar(0);

	filter = Ringz.ar(impulse,XLine.ar(freq,60,0.1),ringTime);

	env = EnvGen.ar(Env.perc(0.001,releaseTime),doneAction:2);

	filter = (1.0-distortion)*filter + (distortion*(filter.distort));
	Out.ar(out,Pan2.ar(filter*env*amp,pan));

}).add;

)


(
Pbind(
	\instrument,\eightoeightkick,
	\dur,1.0,
	\amp,0.5,
	\releaseTime,Pstutter(4,Pn(Pseries(2.0,-0.3,6),inf)),
	\ringTime,Pstutter(6,Pn(Pseries(10.0,-1.0,9),inf)),
	\distortion,Pstutter(7,Pn(Pseries(0.1,0.2,4),inf))
).play
)






//pp. 48-9

(
SynthDef(\pwmbling,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 4.85 lfowidth= 0.5 cutoff= 12000 rq=0.25 pan = 0.0|

	var lfo, pulse, filter, env;
	var basefreq =  ((freq.cpsmidi)+[0,12.12]).midicps;

	lfo = LFTri.kr(lforate*[1,1.01],Rand(0,2.0)!2);

	pulse = Pulse.ar(basefreq,lfo*lfowidth+0.5);

	env = EnvGen.ar(Env.adsr(0.0,1.0,0.2,1.5),gate,doneAction:2);

	filter = RLPF.ar(pulse,(cutoff*(env.squared))+100,rq);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp,pan));

}).add
)


(
Pbind(
	\instrument,\pwmbling,
	\midinote,Prand([0,1,3,5,6,7,10]+60,inf),
	\dur,Pwrand([0.25,0.5],[0.92,0.08],inf),
	\lfowidth,Pn(Pseries(0.0,0.07,7),inf),
	\cutoff,Pstutter(8,Pn(Pseries(10000,-1000,9),inf))
).play
)










//tone wheel organ emulation via two oscillators pp. 50-51

(
SynthDef(\tonewheeltwo,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 4.85 lfowidth= 0.1 cutoff= 5000 rq=0.25 pan = 0.0|

	var lfo, pulse, filter, env;

	lfo = LFTri.kr(lforate*[1,1.01],Rand(0,2.0)!2);

	pulse = Pulse.ar( (((freq*[1,3]).cpsmidi) + (lfo*lfowidth)).midicps,[0.5,0.51],[0.4,0.6]);

	env = EnvGen.ar(Env.adsr(0.0,0.0,1.0,0.1),gate,doneAction:2);

	filter = BLowPass4.ar(pulse,cutoff,rq);

	filter= BPeakEQ.ar(filter,500,1.0,3);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp,pan));

}).add
)


(
Pbind(
	\instrument,\tonewheeltwo,
	\midinote,Prand([48,50,52,60,64,67,58,72],inf),
	\dur,0.5,
	\rq,Pstutter(6,Pn(Pseries(0.75,-0.1,6),inf)),
	\lfowidth,Pstutter(7,Pn(Pseries(0.0,0.05,10),inf)),
	\cutoff,Pstutter(8,Pn(Pseries(2000,1000,4),inf))
).play
)







//pp. 52-3

(
SynthDef(\everythingrhodes,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 4.85 lfowidth= 0.5 cutoff= 2000 rq=0.5 pan = 0.0|

	var pulse, filter, env;

	pulse = Pulse.ar(freq*[1,33.5.midiratio],[0.2,0.1],[0.7,0.3]);

	env = EnvGen.ar(Env.adsr(0.0,1.0,0.8,3.0),gate,doneAction:2);

	//keyboard tracking filter cutoff
	filter = BLowPass4.ar(pulse,(cutoff*(env.squared))+200+freq,rq);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp,pan));

}).add;


//chorus and compression
SynthDef(\choruscompresseffect, {|out =0 gate= 1|
	var source = In.ar(out,2);
	var chorus;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	chorus= Splay.ar(Array.fill(4,{
		var maxdelaytime= rrand(0.005,0.02);

		DelayC.ar(source[0], maxdelaytime,LFNoise1.kr(Rand(0.1,0.6),0.25*maxdelaytime,0.75*maxdelaytime) )
	}));

	chorus = Compander.ar(4*(source + chorus),source,0.4,1,4);

	XOut.ar(out,env,chorus);

}).add;

)



(
Pfx(
	Pbind(
		\instrument,\everythingrhodes,
		\midinote,Pseq([12,8,7,0,[-12,4,12],[-12,4,12],[-12,4,12],[-11,5,12],[-11,5,12],[-9,7,12],7]+48,inf),
		\dur,Pseq([0.5,0.5,0.5,0.5,1.5,1.0,1.0,1.0,1.0,2.0,0.5],inf),
		\cutoff,Pstutter(11,Pn(Pseries(500,500,4),inf))
	),
	\choruscompresseffect
).play
)







//pp. 54-5

(

//added some modulation to ring frequency to roughen sound a bit
SynthDef(\justwannahavefun,{|out= 0 freq = 440 amp = 0.1 ringTime=10.0 pan=(-0.1)|

	var impulse, filter, env;

  	impulse = Impulse.ar(0);

	filter = Ringz.ar(impulse,(freq.cpsmidi + (Rand(0.2,1.2)*SinOsc.kr(Rand(10,50)))).midicps,ringTime);

	env = EnvGen.ar(Env([0.0,1.0,1.0,0.0],[0.001,0.04,0.04]),doneAction:2);

	Out.ar(out,Pan2.ar((filter.softclip(0.9))*env*amp,pan));

}).add;


SynthDef(\gatedreverb,{|out =0 gate= 1 threshold= 0.1 amp=0.1|

	var a,c,z,y,in;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	c = 5; // number of comb delays
	a = 4; // number of allpass delays

	in=In.ar(out,2);

	gate = if((in.mean)>threshold,in.mean,DC.ar(0)!2);

	// reverb predelay time :
	z = gate; //DelayN.ar(gate, 0.048,0.048);

	//for delaytime if want modulation-	//LFNoise1.kr(0.1.rand, 0.04, 0.05)
	y=Mix.arFill(c,{CombL.ar(z,0.1,rrand(0.01, 0.1),rrand(1.5,4))});

	// chain of 4 allpass delays on each of two channels (8 total) :
	a.do({ y = AllpassN.ar(y, 0.051, [rrand(0.01, 0.05),rrand(0.01, 0.05)], 1) });

	Out.ar(out,Pan2.ar(y,0)*env*amp);

}).add;


)

//reverb a bit excessive and resonant, but isn't that fun in itself, Cyndi?
(
Pfx(
	Pbind(
		\instrument,\justwannahavefun,
		\amp,0.5,
		\pan,Pfunc({1.0.rand}),
		\midinote,Pseq(((6!14)++(3!14))+72,inf),
		\dur,Pseq([0.5,0.25,0.5,0.5,0.25,0.5,1.5],inf),
		\ringTime,Pstutter(7,Pn(Pseries(0.1,0.3,8),inf))
	),
	\gatedreverb
).play
)










//pp. 56-7

(

SynthDef(\spacetheremin,{|out= 0 freq = 440 amp = 0.1 gate=1 lforate = 6 lfowidth= 0.5 cutoff= 4000 rq=0.25 lagTime=0.1 pan=0.0|

	var lfo, osc, filter, env;

	lfo = LFTri.kr(lforate+(LFNoise1.kr(5,0.3,0.3)),Rand(0,2.0));

	osc = Saw.ar((freq.lag(lagTime).cpsmidi+(lfo*lfowidth)).midicps,0.5);

	filter = BLowPass4.ar(osc,cutoff.lag(lagTime*4),rq);

	env = EnvGen.ar(Env.adsr(0.6,0.0,1.0,0.05),gate,doneAction:2);

	Out.ar(out,Pan2.ar(filter*env*amp.lag(lagTime*4),pan));

}).add;


SynthDef(\spacereverb,{|out =0 gate= 1 threshold= 0.1 amp=0.1|

	var a,c,z,y,in;
	var env = Linen.kr(gate, 0.1, 1, 0.1, 2);

	c = 6; // number of comb delays
	a = 4; // number of allpass delays

	in=In.ar(out,2);

	// reverb predelay:
	z = DelayN.ar(in, 0.048,0.048);

	y=Mix.arFill(c,{CombL.ar(z,0.1,rrand(0.01, 0.1),5)});

	// chain of 4 allpass delays on each of two channels (8 total) :
	a.do({ y = AllpassN.ar(y, 0.051, [rrand(0.01, 0.05),rrand(0.01, 0.05)], 1) });

	Out.ar(out,Pan2.ar(y,0)*env*amp);

}).add;

)



(
Pfx(
	Pmono(
		\spacetheremin,
		\amp,Prand([0.4,0.45,0.5,0.55,0.5],inf),
		\midinote,Prand([0,2,3,5,6,8,9,11]+60,inf), //octotonic
		\dur,Prand([1.0,1.5,0.5,0.75,1.25,2.0,0.57],inf),
		\lagTime,Pstutter(7,Pn(Pseries(0.05,0.05,8),inf)),
		\lfoRate,Pstutter(9,Pn(Pseries(6,0.5,5),inf)),
		\cutoff,Pstutter(11,Prand([1000,1500,2000,3000],inf))
	),
	\spacereverb
).play
)







//pp. 58-9 velocity-controlled bass

(
SynthDef(\fatvelocitybass,{|out= 0 freq = 440 amp = 0.1 gate=1 cutoff= 12000 rq=0.25 lagTime=0.01 pan = 0.0|

	var lfo, osc, filter, env;

	var basefreq =  ((freq.lag(lagTime).cpsmidi)+[0,11.95,31.03]).midicps;

	osc = Saw.ar(basefreq,[0.5,0.4,0.1]); //+PinkNoise.ar(Line.kr(1.0,0,0.03));

	env = EnvGen.ar(Env.adsr(0.01,1.0,1.0,0.25),gate,doneAction:2);

	filter = BLowPass4.ar(osc,100+((amp.squared)*(freq+cutoff)),rq);

	Out.ar(out,Pan2.ar(Mix(filter)*env*amp*0.8,pan));

}).add
)


(
PmonoArtic(
	\fatvelocitybass,
	\midinote,Pseq([13,18,3,1,3,1,3,1,4,1,3]+36,inf),
	\dur,Pseq([0.5,0.5,0.5,0.5,1.0,0.5,1.5,0.5,1.0,0.5,1.0],inf),
	\amp,Pseq([0.7,1.0,0.5,0.6,0.4,0.7,0.8,0.3,0.6,0.4,0.7],inf),
	\cutoff,3000,
	\rq,0.1
).play
)
