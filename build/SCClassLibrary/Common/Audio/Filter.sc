{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	OnePole.ar(in, coef, mul, add) // one pole filter\
	OneZero.ar(in, coef, mul, add) // one zero filter\
	\
	TwoPole.ar(in, freq, radius, mul, add) // two pole filter\
	TwoZero.ar(in, freq, radius, mul, add) // two pole filter\
	\
	Integrator.ar(in, coef, mul, add) // integrator\
	\
	RLPF.ar(in, freq, q, mul, add) // 2nd order Resonant LowPass Filter\
	RHPF.ar(in, freq, q, mul, add) // 2nd order Resonant HighPass Filter\
\
	LPF.ar(in, freq, mul, add) // 2nd order butterworth LowPass Filter\
	HPF.ar(in, freq, mul, add) // 2nd order butterworth HighPass Filter\
	BPF.ar(in, freq, bw, mul, add) // 2nd order butterworth BandPass Filter\
	BRF.ar(in, freq, bw, mul, add) // 2nd order butterworth Band Reject Filter\
	\
	LPZ1.ar(in, mul, add) // two point sum\
	HPZ1.ar(in, mul, add) // two point difference\
	Slope.ar(in, mul, add) // instantaneous slew rate\
	\
	LPZ2.ar(in, mul, add) // special case two zero lowpass filter. impulse resp: 1 2 1\
	HPZ2.ar(in, mul, add) // special case two zero highpass filter. impulse resp: 1 -2 1\
	BPZ2.ar(in, mul, add) // special case two zero midpass filter. impulse resp: 1 0 1\
	BRZ2.ar(in, mul, add) // special case two zero midcut filter. impulse resp: 1 0 -1\
	\
*/\cf0 \
\
\cf3 Filter\cf0  : \cf3 UGen\cf0  \{\
 	checkInputs \{\
 		if (rate == \cf4 'audio'\cf0 , \{\
 			if (inputs.at(0).rate != \cf4 'audio'\cf0 , \{ ^\cf3 false\cf0  \});\
 		\});\
 		^\cf3 true\cf0 \
 	\}\
\}\
\
\cf3 Resonz\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, bwr = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, bwr).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, bwr = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, bwr).madd(mul, add)\
	\}\
\}\
\
\cf3 OnePole\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, coef = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, coef).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, coef = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, coef).madd(mul, add)\
	\}\
\}\
\
\cf3 OneZero\cf0  : \cf3 OnePole\cf0  \{\}\
\
\cf3 TwoPole\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, radius = 0.8, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, radius).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, radius = 0.8, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, radius).madd(mul, add)\
	\}\
\}\
\
\cf3 TwoZero\cf0  : \cf3 TwoPole\cf0  \{\}\
\cf3 APF\cf0  : \cf3 TwoPole\cf0  \{\}\
\
\cf3 Integrator\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, coef = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, coef).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, coef = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, coef).madd(mul, add)\
	\}\
\}\
\
\cf3 Decay\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, decayTime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, decayTime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, decayTime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, decayTime).madd(mul, add)\
	\}\
\}\
\
\cf3 Decay2\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, attackTime = 0.01, decayTime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, attackTime, decayTime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, attackTime = 0.01, decayTime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, attackTime, decayTime).madd(mul, add)\
	\}\
\}\
\
\cf3 Lag\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, lagTime = 0.1, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, lagTime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, lagTime = 0.1, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, lagTime).madd(mul, add)\
	\}\
\}\
\
\cf3 Lag2\cf0  : \cf3 Lag\cf0  \{\}\
\cf3 Lag3\cf0  : \cf3 Lag\cf0  \{\}\
\cf3 Ramp\cf0  : \cf3 Lag\cf0  \{\}\
\
\cf3 LeakDC\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, coef = 0.995, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, coef).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, coef = 0.9, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, coef).madd(mul, add)\
	\}\
\}\
\
\
\cf3 RLPF\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, rq).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, rq).madd(mul, add)\
	\}\
\}\
\
\cf3 RHPF\cf0  : \cf3 RLPF\cf0  \{\}\
\
\
\cf3 LPF\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq).madd(mul, add)\
	\}\
\}\
\
\cf3 HPF\cf0  : \cf3 LPF\cf0  \{\}\
\
\cf3 BPF\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, rq).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, rq).madd(mul, add)\
	\}\
\}\
\
\cf3 BRF\cf0  : \cf3 BPF\cf0  \{\}\
\
\cf3 ParEQ\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, rq, db).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, rq, db).madd(mul, add)\
	\}\
\}\
\
\cf3 LPZ1\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in).madd(mul, add)\
	\}\
\}\
\
\cf3 HPZ1\cf0  : \cf3 LPZ1\cf0  \{\}\
\
\cf3 Slope\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in).madd(mul, add)\
	\}\
\}\
\
\cf3 LPZ2\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in).madd(mul, add)\
	\}\
\}\
\
\cf3 HPZ2\cf0  : \cf3 LPZ2\cf0  \{\}\
\
\cf3 BPZ2\cf0  : \cf3 LPZ2\cf0  \{\}\
\
\cf3 BRZ2\cf0  : \cf3 LPZ2\cf0  \{\}\
\
\cf3 Median\cf0  : \cf3 Filter\cf0  \{\
	*ar \{ \cf3 arg\cf0  length=3, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , length, in).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  length=3, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , length, in).madd(mul, add)\
	\}\
\}\
\
\cf3 AvgAbsAmp\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, coef = 0.999, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, coef).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, coef = 0.999, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, coef).madd(mul, add)\
	\}\
\}\
\
\cf3 Slew\cf0  : \cf3 Filter\cf0  \{\
	*ar \{ \cf3 arg\cf0  in = 0.0, up = 1.0, dn = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, up, dn).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, up = 1.0, dn = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, up, dn).madd(mul, add)\
	\}\
\}\
\
\cf3 RLPF4\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 0.5, res = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, res).madd(mul, add)\
	\}\
\}\
\
\cf3 FOS\cf0  : \cf3 Filter\cf0  \{\
	*ar \{ \cf3 arg\cf0  in = 0.0, a0 = 0.0, a1 = 0.0, b1 = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, a0, a1, b1).madd(mul, add)\
	\}\
\}\
\
\cf3 SOS\cf0  : \cf3 Filter\cf0  \{\
	*ar \{ \cf3 arg\cf0  in = 0.0, a0 = 0.0, a1 = 0.0, a2 = 0.0, b1 = 0.0, b2 = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, a0, a1, a2, b1, b2).madd(mul, add)\
	\}\
\}\
\
\
\cf3 Ringz\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, decaytime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, decaytime).madd(mul, add)\
	\}\
\}\
\
\cf3 Formlet\cf0  : \cf3 Filter\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, attacktime = 1.0, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, freq, attacktime, decaytime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, freq = 440.0, attacktime = 1.0, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, freq, attacktime, decaytime).madd(mul, add)\
	\}\
\}\
\
\
\cf3 EndThresh\cf0  : \cf3 Filter\cf0  \{\
\
	*ar \{ \cf3 arg\cf0  in = 0.0, amp = 0.00001, time = 0.2;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, amp, time)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0, amp = 0.00001, time = 0.2;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in, amp, time)\
	\}\
\}\
\
\cf3 FlagNaN\cf0  : \cf3 Filter\cf0  \{\
\
	*ar \{ \cf3 arg\cf0  in = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in)\
	\}\
	*kr \{ \cf3 arg\cf0  in = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , in)\
	\}\
\}\
\
}