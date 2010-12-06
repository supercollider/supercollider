
s.boot;
s.quit;

(
SynthDef("silenceFFT", {
	var in, chain;
	in = WhiteNoise.ar(0.01);
	chain = FFT(0, in);
	Out.ar(0, IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 0, 2048, 1);
s.sendMsg("/s_new", "silenceFFT", 2002, 1, 0);
s.sendMsg("/n_free", 2002);

(
SynthDef("sineFFT", {
	var in, chain;
	in = SinOsc.ar(SinOsc.kr(SinOsc.kr(0.08,0,6,6.2).squared, 0, 100,800));
	chain = FFT(0, in);
	Out.ar(0, IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 0, 2048, 1);
s.sendMsg("/s_new", "sineFFT", 2002, 1, 0);
s.sendMsg("/n_free", 2002);

(
SynthDef("magAbove", {
	var in, chain;
	in = SinOsc.ar(SinOsc.kr(SinOsc.kr(0.08,0,6,6.2).squared, 0, 100,800));
	//in = WhiteNoise.ar(0.2);
	chain = FFT(0, in);
	chain = PV_MagAbove(chain, 310); 
	Out.ar(0, 0.5 * IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 0, 2048, 1);
s.sendMsg("/s_new", "magAbove", 2002, 1, 0);
s.sendMsg("/s_trace", 2002)
s.sendMsg("/n_free", 2002);

(
SynthDef("brick", {
	var in, chain;
	in = {WhiteNoise.ar(0.2)}.dup;
	chain = FFT(0, in);
	chain = PV_BrickWall(chain, SinOsc.kr(0.1)); 
	Out.ar(0, IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 0, 2048, 1);
s.sendMsg("/s_new", "brick", 2002, 1, 0);
s.sendMsg("/s_trace", 2002)
s.sendMsg("/n_free", 2002);

(
SynthDef("randcomb", { arg bufnum=0;
	var in, chain;
	in = {WhiteNoise.ar(0.8)}.dup;
	chain = FFT(bufnum, in);
	chain = PV_RandComb(chain, 0.95, Impulse.kr(0.4)); 
	Out.ar(0, IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 1, 2048, 1);
s.sendMsg("/s_new", "randcomb", 2003, 1, 0, \bufnum, 1);
s.sendMsg("/s_trace", 2003)
s.sendMsg("/n_free", 2003);

(
SynthDef("rectcomb", { arg bufnum=0;
	var in, chain;
	in = {WhiteNoise.ar(0.2)}.dup;
	chain = FFT(bufnum, in);
	chain = PV_RectComb(chain, 8, LFTri.kr(0.097,0,0.4,0.5), 
		LFTri.kr(0.24,0,-0.5,0.5)); 
	Out.ar(0, IFFT(chain));
}).add;
)

s.sendMsg("/b_alloc", 0, 2048, 1);
s.sendMsg("/s_new", "rectcomb", 2002, 1, 0, \bufnum, 1);
s.sendMsg("/s_trace", 2002)
s.sendMsg("/n_free", 2002);

s.quit;

