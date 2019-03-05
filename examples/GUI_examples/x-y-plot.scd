//jmc 2003


(
Ndef(\lissajous, {
	var m, f;
	f = SampleRate.ir / 64;
	m = SinOsc.kr(0.001, 0, 0.8, 1).round(1/60);
	[SinOsc.ar(f + 0.01), SinOsc.ar(f * m)]
}).play(vol: 0.01).scope.style_(2).size_(400);
)


(
Ndef(\lissajous, {
	var in, f;
	f = MouseX.kr(40,8000,1);
	in = {RLPF.ar(BrownNoise.ar(0.5), f, 0.3) }.dup;
}).play(vol: 0.1).scope.style_(2).size_(400);
)


(
Ndef(\lissajous, {
	var in, f, pw;
	f = MouseX.kr(40,8000,1);
	pw = MouseY.kr(0.01,0.99);
	in = RLPF.ar(Pulse.ar([100,100.2],pw,0.5), f, 0.1).softclip;
}).play(vol: 0.05).scope.style_(2).size_(400);
)

(
Ndef(\lissajous, {
	var in, f;
	f = MouseX.kr(40,8000,1);
	in = RLPF.ar(Saw.ar([100,100.2],0.5), f, 0.1).softclip;
}).play(vol: 0.05).scope.style_(2).size_(400);
)

(
Ndef(\lissajous, {
	var in, f, pw;
	f = MouseX.kr(40,8000,1);
	pw = MouseY.kr(0.01,0.99);
	in = RLPF.ar(Pulse.ar([120, 120.12, 200, 200.1], pw, 0.5), f, 0.1).softclip;
}).play(vol: 0.05).scope.style_(2).size_(400);
)
