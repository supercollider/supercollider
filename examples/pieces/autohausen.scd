// Autohausen
//(C) Nick Collins 2007


//first send the SynthDefs; make sure the Server is on
(
var numChannels=2;//4  //replace 2 by 4 if you want to try a quadrophonic setup

SynthDef(\impulsetrain1, {arg freq=440, amp=0.1,attack=0.01, sustain=0.1, decay=0.01,pan=0.0, bandwidth=100;
var source, filter, env;

env= EnvGen.ar(Env([0,1,1,0],[attack, sustain, decay]), doneAction:2);

source= Impulse.ar(freq);

filter= BPF.ar(source*env*amp*10,freq,bandwidth/freq);

Out.ar(0,PanAz.ar(numChannels,filter,pan))
}).add;


SynthDef(\sinarpegg1, {arg freq=440, amp=0.1,attack=0.01, sustain=0.1, decay=0.01,pan=0.0;
var source, filter, env;

source= SinOsc.ar(EnvGen.kr(Env([1,2,3,4,5,6]*freq,(0.2*attack).dup(5))));

filter= CombN.ar(source*EnvGen.kr(Env([1,1,0],[attack,0.01])),0.025,0.025,5);

env= EnvGen.kr(Env([0,0,1,1,0],[attack, 0.01,sustain, decay]), doneAction:2);

Out.ar(0,PanAz.ar(numChannels,filter*env*0.5*amp,pan))
}).add;


SynthDef(\pitchednoise, {arg freq=440, amp=0.1,attack=0.01, sustain=0.1, decay=0.01,pan=0.0, bandwidth=100;
var source, filter, env;

env= EnvGen.ar(Env([0,1,1,0],[attack, sustain, decay]), doneAction:2);

source= WhiteNoise.ar;

filter= BPF.ar(source*env*2*amp,freq,bandwidth/freq);

Out.ar(0,PanAz.ar(numChannels,filter,pan))
}).add;


//preferred version if you have the FreeVerb UGen, commented out by default
//SynthDef(\stockyfx, {arg shift=0.0, rate=1.0;
//var input, fx;
//var shifted;
//
//input=In.ar(0,2);
//
////simulate slow down
//shifted= PitchShift.ar(input, 0.01,rate);
//
//ReplaceOut.ar(0,FreeVerb.ar(((shift*shifted)+((1.0-shift)*input)),0.33,1.5))
//
//}).add;


SynthDef(\stockyfx, {arg shift=0.0, rate=1.0;
var input, fx;
var shifted;
var a,c,z,y,in;
c = 2; // number of comb delays
a = 3; // number of allpass delays

input=In.ar(0,2);

//simulate slow down
shifted= PitchShift.ar(input, 0.01,rate);

input= (shift*shifted)+((1.0-shift)*input);

// reverb predelay time :
z = DelayN.ar(input, 0.048,0.048);

//for delaytime if want modulation-	//LFNoise1.kr(0.1.rand, 0.04, 0.05)
y=Mix.arFill(c,{CombL.ar(z,0.1,rrand(0.01, 0.1),5)});

// chain of 4 allpass delays on each of two channels (8 total) :
a.do({ y = AllpassN.ar(y, 0.051, [rrand(0.01, 0.05),rrand(0.01, 0.05)], 1) });

ReplaceOut.ar(0,(0.33*y) + input);

}).add;

)



//tests
Synth(\pitchednoise,[\freq, 100, \bandwidth, 200])
Synth(\impulsetrain1,[\freq, 440])
Synth(\sinarpegg1,[\freq, 440])





//now run the piece
(
var series,seriesseries,series2,seriesseries2, tmp;
var ioiseries, durseries, dynamicseries, bandwidthseries, instrseries;
var attackseries, sustainseries, decayseries, freqseries, panseries, temposeries;
var numatonce;
var fxsynth;
var t=TempoClock(1);
var basefreq=110;
var sourceseries, moments;

fxsynth= Synth(\stockyfx);

sourceseries= Array.fill(rrand(1,12),{[(0..5).scramble,(0..11).scramble]});

moments= Array.fill(exprand(13,137).asInteger,{[sourceseries.choose[0], sourceseries.choose[1], exprand(12,143).asInteger]});

{

moments.do{|val,i|

("Struktur"+(i+1)).postln;

series=val[0];
series2=val[1];

tmp=series.copy;

seriesseries= Array.fill(5,{tmp= (tmp-1).max(0)});

tmp=series2.copy;

seriesseries2= Array.fill(12,{tmp= (tmp-1).max(0)});

//series of six except for pitches
ioiseries= Pseq(0.025+(6**((seriesseries2.copy.flatten.at((0..143)).rotate(144.rand)+1)/12)-1),inf).asStream;
dynamicseries= Pseq(((seriesseries2.copy.flatten.at((0..142)).rotate(144.rand))*2).neg.dbamp,inf).asStream;
bandwidthseries= Pseq((seriesseries2.copy.flatten.at((0..127)).rotate(144.rand)+1)*25,inf).asStream;
instrseries= Pseq([\pitchednoise,\impulsetrain1,\sinarpegg1].dup(6).flatten.scramble,inf).asStream;
attackseries= Pseq((seriesseries.copy.flatten.at((0..26)).rotate(36.rand)+1)*0.01,inf).asStream;
sustainseries=Pseq((seriesseries2.copy.flatten.at((0..125)).rotate(144.rand)+1)*0.1,inf).asStream;
decayseries=Pseq((seriesseries.copy.flatten.at((0..24)).rotate(36.rand)+1)*0.1,inf).asStream;
freqseries=Pseq(24**((0..216).scramble/216),inf).asStream;
panseries=Pseq((seriesseries.copy.flatten.at((0..23)).rotate(36.rand)+1)*(2/5)-1.0,inf).asStream;
numatonce=Pseq((seriesseries.copy.flatten.at((0..28)).rotate(36.rand)+1),inf).asStream;
temposeries=Pseq(((30,35..120)/60).scramble,inf).asStream;

fxsynth.set(\shift,[0.0,1.0,rrand(0.25,0.75)].choose,\rate,[1,0.5,0.25,0.125,4].choose);
basefreq=[27.5,55,110,220,330,660].choose;

val[2].do {

	if(0.1.coin,{
	t.tempo_(temposeries.next)});

	if(0.02.coin,{
	basefreq=[27.5,55,110,220,330,660].choose
	});

	s.makeBundle(s.latency, {
		var instrtype;

		instrtype=instrseries.next;

		numatonce.next.do{
			Synth.before(fxsynth,instrtype ,[\freq, basefreq*freqseries.next, \amp, (dynamicseries.next)*0.25, \pan, panseries.next, \attack, attackseries.next,  \decay, decayseries.next,  \sustain,  sustainseries.next, \bandwidth, bandwidthseries.next ]);
		};
	});

	ioiseries.next.wait;
};

}

}.fork(t)
)



//future: add ringmod and glissandi, plus distortion for old magnetic tape? differentiate moments more...
