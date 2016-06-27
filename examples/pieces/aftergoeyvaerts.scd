//after goeyvaerts
//(C) Nick Collins 2007


//first send the SynthDefs; make sure the Server is on
(
SynthDef(\goeysynth, {
arg freq=440,amp=0.1, sustain=0.1, pan=0.0;
var source, env;

source= LPF.ar(Mix(LFPar.ar(freq*[0.999,1.001],0,amp)).distort,EnvGen.kr(Env([10000,2000,4000,1000],[0.005,Rand(0.009,0.05),0.005])));

env= EnvGen.kr(Env([0,1,0.4,0.7,0],[Rand(0.001,0.005),0.005,0.005,sustain]), doneAction:2);

Out.ar(0,Pan2.ar(source*env,pan))

}).add;

//preferred version if you have the FreeVerb UGen, commented out by default
//SynthDef(\goeyfx, {
//ReplaceOut.ar(0,FreeVerb.ar(In.ar(0,2),0.33,1.5))
//}).add;

//adapted from JmC reverb
SynthDef(\goeyfx, {
var a,c,z,y,in;
c = 2; // number of comb delays
a = 3; // number of allpass delays

in=In.ar(0,2);
// reverb predelay time :
z = DelayN.ar(in, 0.048,0.048);

//for delaytime if want modulation-	//LFNoise1.kr(0.1.rand, 0.04, 0.05)
y=Mix.arFill(c,{CombL.ar(z,0.1,rrand(0.01, 0.1),5)});

// chain of 4 allpass delays on each of two channels (8 total) :
a.do({ y = AllpassN.ar(y, 0.051, [rrand(0.01, 0.05),rrand(0.01, 0.05)], 1) });

// add original sound to reverb and play it :
Out.ar(0,(0.2*y));

}).add;

)



Synth(\goeysynth,[\freq,440]); //test



//now run the piece
(
var n, octaves, basenote, selections, probs;
var ioirow, ioimult, noterow, amprow, susrow, panrow;
var fxsynth;

n=12; //notes per octave

fxsynth= Synth(\goeyfx);

noterow=(0..(n-1)).scramble; //[4,0,5,14,13,15,21,19,20,11,6,10]%12;
amprow= Pseq((1,3..36).neg.dbamp.scramble,inf).asStream;
susrow= Pseq((((1..37)/37).exp/exp(1)).scramble,inf).asStream;
panrow= Pseq((((0..31)/31)*2-1).scramble,inf).asStream;

ioimult=0.1;

ioirow= n**(((1..n).scramble)/n); //exponential spacing 1 to n
ioirow= Pseq(ioirow,inf).asStream;

basenote=36;
octaves=4;
selections= Array.fill(n,{0});
probs=Array.fill(n,{1.0});

{

inf.do
{
	var notenow;

	if(ioimult.coin,{ioimult=[0.01,0.025,0.05,0.1,0.2].choose;});

	if(0.03.coin,{selections= Array.fill(n,{0});});
	if(0.02.coin,{octaves= rrand(2,5);});
	if(0.01.coin,{basenote=rrand(35,47);});

	//recovery rate
	probs.do{|val,i| if(val<0.9999,{probs[i]=probs[i]+0.1})};

	s.makeBundle(s.latency, {
		[1,2,3,4,5].wchoose([0.5,0.35,0.1,0.025,0.025]).do{
			notenow= noterow.wchoose(probs.normalizeSum);

			probs[notenow]=0.1;

			selections[notenow]=selections[notenow]+1;

			Synth.before(fxsynth, \goeysynth,[\freq, ((((selections[notenow]%octaves)*12)+ basenote) + ((notenow/n)*12)).midicps,\amp,(amprow.next)*0.2, \sustain, susrow.next, \pan, panrow.next]);

		};

	});

	((ioirow.next)*ioimult).wait;
};

}.fork;

)


//future possibilities:
//add different rhythmic sections, and also, use fold or wrap on array data
//up to four notes at once in selection; optional slight spread? really need independent voices too
