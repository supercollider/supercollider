
// bit reduction
// adc

//--samplerate decrease
({	var snd = Blip.ar(LFNoise2.kr(8, 200, 300), LFNoise2.kr(3, 10, 20));	var samplerate = MouseX.kr(1000, s.sampleRate * 0.1, \exponential);	Latch.ar(snd, Impulse.ar(samplerate)); }.play;)

//--bitrate decrease(
{	var snd = Blip.ar(LFNoise2.kr(8, 200, 300), LFNoise2.kr(3, 10, 20));	var samplerate = MouseX.kr(1000, s.sampleRate * 0.5, \exponential);	var bitSize = MouseY.kr(1, 24, \exponential);	var downsamp = Latch.ar(snd, Impulse.ar(samplerate));	var bitRedux = downsamp.round(0.5 ** bitSize);	[downsamp, bitRedux];}.play)