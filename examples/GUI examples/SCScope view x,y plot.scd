//jmc 2003

The SCScope view 'style' property supports 3 styles:
0 = separate waveform plots
1 = overlapping waveform plots
2 = x,y plots of 2 channel pairs.

(
// create a window with a scope view
var f, s;
w = SCWindow("scope test", Rect(128, 64, 512, 512));

n = SCScope(w, Rect(10,10,480,480));
n.bufnum = 0;
n.background = Color.black;
n.resize = 5;
n.style = 2;
n.waveColors = [Color.yellow, Color.blue(1.2), Color.red, Color.green];
w.front;
)

s.boot;

// allocate a 2 channel buffer
s.sendMsg(\b_alloc, 0, 2048, 2);

// play a 2 channel sound into that buffer using ScopeOut.
(
z = {
	var m, f;
	f = SampleRate.ir / 64;
	m = SinOsc.kr(0.001, 0, 0.8, 1).round(1/60);
	ScopeOut.ar([SinOsc.ar(f + 0.01), SinOsc.ar(f * m)], 0)
}.play(s);
)

z.free;

(
z = {
	var in, f;
	f = MouseX.kr(40,8000,1);
	in = {RLPF.ar(BrownNoise.ar(0.5), f, 0.3) }.dup;
	ScopeOut.ar(in, 0);
	Out.ar(0, in);
}.play(s);
)

z.free;


(
z = {
	var in, f, pw;
	f = MouseX.kr(40,8000,1);
	pw = MouseY.kr(0.01,0.99);
	in = RLPF.ar(Pulse.ar([100,100.2],pw,0.5), f, 0.1).softclip;
	ScopeOut.ar(in, 0);
	Out.ar(0, in);
}.play(s);
)

z.free;

(
z = {
	var in, f;
	f = MouseX.kr(40,8000,1);
	in = RLPF.ar(Saw.ar([100,100.2],0.5), f, 0.1).softclip;
	ScopeOut.ar(in, 0);
	Out.ar(0, in);
}.play(s);
)

z.free;



// allocate a 4 channel buffer
s.sendMsg(\b_alloc, 0, 2048, 4);


(
z = {
	var in, f, pw;
	f = MouseX.kr(40,8000,1);
	pw = MouseY.kr(0.01,0.99);
	in = RLPF.ar(Pulse.ar([120,120.12, 200, 200.1],pw,0.5), f, 
0.1).softclip;
	ScopeOut.ar(in, 0);
	Out.ar(0, Mix(in.clump(2)) * 0.5);
}.play(s);
)

z.free;

n.style = 0;
n.style = 1;
n.style = 2;
