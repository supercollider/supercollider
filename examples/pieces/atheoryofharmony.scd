// a theory of harmony
//(C) Nick Collins 2007

//plaintive chords patch demonstrating scheduling attacks ahead of the beat and applying reverb
//the chord transitions are adapted from an idea by John Eacott



//first send the SynthDefs; make sure the Server is on
(
SynthDef(\slowatk,{arg note=60,atk=0.2,pan=0.0,det=0.1,detrate=9,wrate=16, wspan=0.4;
var slip,detune;

detune=SinOsc.kr(detrate,0,det,det);

slip= Mix.ar(Pulse.ar([note-detune,note+detune].midicps,Lag.kr(LFNoise0.kr(wrate,wspan,0.5)),0.1))*EnvGen.ar(Env([0,1,0],[atk,0.5]),doneAction:2);
Out.ar(0,Pan2.ar(slip,pan));
}).send(s);

SynthDef(\fastatk,{arg note=60,dcy=0.3,pan=0.0;
var osc;

osc= LPF.ar(Mix.ar(LFPar.ar(note.midicps,[0,1],0.12, PinkNoise.ar(XLine.kr(0.3,0.01,0.02)))), Line.kr(10000,200,0.1))
*EnvGen.ar(Env([0,1,0],[0.01,dcy]),doneAction:2);

Out.ar(0,Pan2.ar(osc,pan));
}).send(s);

SynthDef(\reverb,{
var a,c,z,y,in;
c = 7; // number of comb delays
a = 4; // number of allpass delays

in=In.ar(0,2);
// reverb predelay time :
z = DelayN.ar(in, 0.048,0.048);

//for delaytime if want modulation-	//LFNoise1.kr(0.1.rand, 0.04, 0.05)
y=Mix.arFill(c,{CombL.ar(z,0.1,rrand(0.01, 0.1),5)});

// chain of 4 allpass delays on each of two channels (8 total) :
a.do({ y = AllpassN.ar(y, 0.051, [rrand(0.01, 0.05),rrand(0.01, 0.05)], 1) });

// add original sound to reverb and play it :
ReplaceOut.ar(0,in+(0.2*y));
}).send(s)
)


//now run the piece
(
var sched;
var chordtype,diatonic,transitions, current, strength, type;
var revunit;
var next,hits,hitset;
var prevchord, ostinatostyle,which,base;
var chordbias;

prevchord=[60,64,67];
ostinatostyle=4.rand;
which=2.rand;

revunit= Synth(\reverb);

chordbias= 1.0; //0.7; //default 0.7

//a quick list of chord types, ignoring any theory of extensions
chordtype= [
[0,4,7],		//maj
[0,4,7,11],	//maj7
[0,3,7],		//min
[0,3,7,10],	//min7
[0,4,7,10],	//dom 7
[0,3,6,10],	//half dim
[0,3,6,9],	//dim
[0,5,7]		//sus4
];

diatonic=[0,2,4,5,7,9,11];

//perversion of John Eacott's theory of pop harmony, from strong to weak
transitions=[-3,-2,3,1,2,-1,0];

current=0;	//start on tonic
strength=0;

SystemClock.sched(0.0,{
	var chord,root, adjust, whichroot, pans;
	var chord3;

	if(0.2.coin,{strength= (strength+rrand(1,2))%7});

	current=(current+(transitions.at(strength)))%7;

	root=diatonic.at(current)+60;

	chord=chordtype.wchoose(Array.geom(8,1,0.7).normalizeSum)+root;

	adjust= (chord.size-1).rand;

	//changing octave of some elements
	adjust.do({arg i; chord.put(chord.size-1-i,chord.at(chord.size-1-i)-12)});

	//make root the bass eight times out of ten
	whichroot= 0;
	if(0.2.coin,{whichroot=chord.size-1});

	chord.put(whichroot,chord.at(whichroot)-24);

	chord.postln;

	pans= Array.fill(chord.size,{arg i; (2.0*i)/(chord.size-1)-1.0;});

	pans.scramble;

	//can do 3.do which is v.pleasant
	chord.size.do({arg i;
		var ahead;

		ahead= exprand(0.05,1.0);

		SystemClock.sched(1.0-ahead,{
		Synth.before(revunit,\slowatk,[\note, chord.at(i), \atk, ahead,\pan, pans.at(i),\detrate,8.0.rand, \det,0.12.rand,\wrate, [1,4,16].choose, \wspan, 0.45.rand]);
		nil
		});

	});

	if(0.05.coin,{ostinatostyle= 4.rand; which= 2.rand});

	base= [chord,prevchord].at(which);
	chord3= [base,base+12,base-12,(base%12).scramble+60].at(ostinatostyle);
	//jolly rhythm, can use prevchord or chord, can use plain chord instead of the modulo version
	//chord3= chord+12; //(chord %12).scramble+60; chord-12; chord+12; chord
	//chord3.postln;

	next=[1.0,1.5,2.0,3.25].wchoose([0.32,0.32,0.32,0.04]);

	hits= (next/0.25).asInteger;

	//hitset= Array.series(hits,0.0,0.5);
	hitset=List.new;

	hits.do({arg i;

		hitset.add(i*0.25);
		if(0.1.coin,{
		hitset.add(i*0.25+0.125)
		});

	});

	if(0.97.coin,{
		hitset.do({arg val;

		SystemClock.sched(val,{

		//could be 3.do or chord3.do
		chord3.size.do({arg i;
		Synth.after(revunit,\fastatk,[\note,chord3.at(i)]);
		});

		nil});
		});
	});

	prevchord=chord;

	next
});


)
