
// from SC2

// Fibonacci and Overtone scale (see e.g. Jay Kappraff, "Connections - the
// Geometric Bridge between Art and Science"). I keep returning to this for
// some reason. Maybe I should stop listening to Bela B? Anyway it is about
// Fibonacci numbers, ie
// 		f(0) = 1, f(1) = 1, f(n) = f(n-1)+f(n-2)
// 		which gives the series 1,1,2,3,5,8,13, 21...
// It approximates the Golden Ratio if You divide f(n)/f(n-1)
// Bela Bartok used this and constructed a (fibonacci) scale from this,
// with C as fundamental (0), skipping 1's and going upto 13:
//		C, D, Eb, F, Ab, C#.
// He also used the overtone scale, built on fundamental plus harmonics and
// made into a scale with frequency ratios 1:1, 2:1, 3:1 etc. This gives fundamental,
// octave, fifth+octave etc. Starting on C, we get (a well-tempered approximation) :
// 		C, D, E, F#, G, A, Bb, B, C.
// These scales decomposes the chromatic scale into to two complementary in terms
// of emotion and this duality can be used to create great contrast, ie Fibonacci
// "inferno" and overtone "paradisio".
// Staffan Liljegren, 990712, staffan@medialab.ericsson.se

// First both in contrasting sequence:

(
var ot, fib, p;
fib = #[60, 62, 63, 65, 68, 73];
ot =  #[60, 62, 64, 66, 67, 69, 70, 71];
Pbind(
	\dur, 0.25,
	\midinote, Pseq([ Pseq(fib), Pseq([\rest],2), Pseq(ot)], 2)
	// play message supplies the Event.protoEvent as default
	).play
)

// Let us use the Fibonacci series for duration and legato values also and
// uses these values for duration and legato:
//		1,1, 1/2, 1/3, 1/5, ...
// This is a series 1/fib(n).
// In this particular example, which is only here to demonstrate the components,
// we just play the fib scale up with random legato and a rythm which goes:
//		 1,1, 1/2, 1/2, 1/3, 1/3, 1/3, 1/5, 1/5, 1/5, 1/5,1/5
//		 1 2  3         4              5
// Or "taa-taa-tantan-tateti-dallapicola" in  5/4. Don't ask me about these
// rythm mnemonics! The fib scale contains 6 notes and this rythm pattern has
// 12 events, so we play the fib scale twice over the rythm. Finally we add
// a random fib bass on each beat and then play a sequence of
//		bass only, 2*bass+mel, 2*bass+mel transposed up and then down a 5th and
// 		finally the melody alone and a simple ending
(
var fib, bass, mel, ending,tune;

fib = #[60, 62, 63, 65, 68, 73];

bass = Pbind(
	\tempo, 1.3,
	\legato, 0.1,
	\dur, Pseq([1], 5),
	\midinote, Pxrand(fib - 24, 5)
);
mel = Pbind(
	\tempo, 1.3,
	\legato, Prand([1,1,1/2,1/3, 1/5, 1/8], inf),
	\dur, Pseq([1,1,Pseq([1/2],2),Pseq([1/3],3), Pseq([1/5], 5)], 1),
	\midinote, Pseq( fib , 2)
);
ending = Pbind( \dur, Pgeom(0.05, 1.1, 20), \midinote,Pseq([68,72],10) );
tune = Pseq([
			bass,
			Ppar([ bass, mel ],2),
			Padd(\ctranspose, 7,  Ppar([ bass, mel ],2)),
			Padd(\ctranspose, -7, Ppar([ bass, mel ],2)),
			mel,
			ending
		], 1);

tune.play

)

// Let us now shuffle notes and durations a bit in the melody. We also
// add some rests to the melody line and play two separate melody lines

(
var fib, bass, mel, ending,tune;

fib = #[60, 62, 63, 65, 68, 73];

bass = Pbind(
	\tempo, 1.4,
	\legato, 0.1,
	\dur, Pseq([1], 5),
	\midinote, Pxrand(fib - 24, 5)
);
mel = Pbind(
	\tempo, 1.4,
	\legato, Prand([1,1,1/2,1/3, 1/5, 1/8], inf),
	\dur, Pshuf([1,1,Pseq([1/2],2),Pseq([1/3],3), Pseq([1/5], 5)], 1),
	\midinote, Prand( fib ++ [\rest, \rest] , 10)
);

tune = Pseq([
			Ppar([ bass, mel, mel ],2),
			Padd(\ctranspose, 7,  Ppar([ bass, mel, mel ],2)),
			Padd(\ctranspose, -7, Ppar([ bass, mel, mel ],2))
		], 2);

tune.play

)

// Back to the overtone scale now. Here is a simple example, using
// a randomly "pulsating" ot scale and shuffled long bass notes with even
// durations in 5/4. This is then chromatically transposed upto a 5th and
// in steps following the ot scale

(
var ot,c, b, m;

ot = #[60, 62, 64, 66, 67, 69, 70, 71];

c = Pbind(\dur, 0.1,
		\amp, Pseq([ Pgeom(0.2,0.95,50)],2),
		\midinote, Prand( ot-12, inf),
		\legato, 0.2,
		\pan, Pfunc({1.0.rand2})
	);

b = Pbind( \dur, Prand([5,Pseq([1,1,1,1,1]), Pseq([3,2]) ], 2),
		\amp, Pfunc( {XLine.kr(0.001,0.1, 0.5)} ),
		\midinote, Pshuf( ot-24)
	);

Paddp(\ctranspose,
	Pseq([0,2,4,6,7], inf), Ppar([b,c])).play

)


// You might also consider removing the 1/5 in the melody duration
// if fast "dallapiccolas" upset You! Here I changed it to 3 1/3 notes
(
var fib, bass, mel, ending,tune;

fib = #[60, 62, 63, 65, 68, 73];

bass = Pbind(
 \tempo, 1.5,
 \legato, 0.1,
 \dur, Pseq([1], 5),
 \midinote, Pxrand(fib - 24, 5)
);
mel = Pbind(
 \tempo, 1.5,
 \legato, Prand([1,1,1/2,1/3, 1/5, 1/8], inf),
 \dur, Pshuf([1,1,Pseq([1/2],2),Pseq([1/3],3), Pseq([1/3], 3)], 1),
 \midinote, Prand( fib ++ [\rest, \rest] , 10)
);

tune = Pseq([
   Ppar([ bass, mel, mel ],2),
   Padd(\ctranspose, 7,  Ppar([ bass, mel, mel ],2)),
   Padd(\ctranspose, -7, Ppar([ bass, mel, mel ],2))
  ], inf);

tune.play

)

// Now write Your own piece with the fib and ot scales!
// Or just play around with these simple ones
