{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red191\green0\blue0;\red0\green0\blue191;
\red0\green115\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 // ControlSpec - defines the range and curve of a control.\
\cf2 \
\cf4 Spec\cf2  \{\
	\cf4 classvar\cf2  <>specs;\
	*initClass \{\
		specs = \cf4 IdentityDictionary\cf2 .new;\
	\}\
	asSpec \{ ^\cf4 this\cf2  \}\
\}\
\
\cf4 ControlSpec\cf2  : \cf4 Spec\cf2  \{\
	\cf4 var\cf2  <>minval, <>maxval, <>warp, <>step, <>default, <>units;\
	\
	*new \{ \cf4 arg\cf2  minval=0.0, maxval=1.0, warp=\cf5 'lin'\cf2 , step=0.0, default, units;\
		^\cf4 super\cf2 .newCopyArgs(minval, maxval, warp, step, \
				default ? minval, units ? \cf6 ""\cf2 \
			).init\
	\}\
	init \{ \
		warp = warp.asWarp(\cf4 this\cf2 );\
	\}\
	\
	constrain \{ \cf4 arg\cf2  value;\
		^value.clip(minval, maxval).round(step)\
	\}\
	range \{ ^maxval - minval \}\
	ratio \{ ^maxval / minval \}\
	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^warp.map(value.clip(0.0, 1.0).round(step));\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^warp.unmap(value.clip(minval, maxval)).round(step);\
	\}\
	\
	*initClass \{\
		specs = specs.addAll([\
			\cf3 // set up some ControlSpecs for common mappings\
\cf2 			\cf3 // you can add your own after the fact.\
\cf2 			\
			\cf5 \\unipolar\cf2  -> \cf4 ControlSpec\cf2 (0, 1),\
			\cf5 \\bipolar\cf2  -> \cf4 ControlSpec\cf2 (-1, 1, default: 0),\
\
			\cf5 \\freq\cf2  -> \cf4 ControlSpec\cf2 (20, 20000, \cf5 \\exp\cf2 , 0, 440, units: \cf6 " Hz"\cf2 ),\
			\cf5 \\lofreq\cf2  -> \cf4 ControlSpec\cf2 (0.1, 100, \cf5 \\exp\cf2 , 0, 6, units: \cf6 " Hz"\cf2 ),\
			\cf5 \\widefreq\cf2  -> \cf4 ControlSpec\cf2 (0.1, 20000, \cf5 \\exp\cf2 , 0, 440, units: \cf6 " Hz"\cf2 ),\
			\cf5 \\phase\cf2  -> \cf4 ControlSpec\cf2 (0, 2pi),\
			\cf5 \\rq\cf2  -> \cf4 ControlSpec\cf2 (0.001, 2, \cf5 \\exp\cf2 , 0, 0.707),\
\
			\cf5 \\audiobus\cf2  -> \cf4 ControlSpec\cf2 (0, 128, step: 1),\
			\cf5 \\controlbus\cf2  -> \cf4 ControlSpec\cf2 (0, 4096, step: 1),\
\
			\cf5 \\midi\cf2  -> \cf4 ControlSpec\cf2 (0, 127, default: 64),\
			\cf5 \\midinote\cf2  -> \cf4 ControlSpec\cf2 (0, 127, default: 60),\
			\cf5 \\midivelocity\cf2  -> \cf4 ControlSpec\cf2 (1, 127, default: 64),\
			\
			\cf5 \\db\cf2  -> \cf4 ControlSpec\cf2 (0.ampdb, 1.ampdb, \cf5 \\db\cf2 , units: \cf6 " dB"\cf2 ),\
			\cf5 \\amp\cf2  -> \cf4 ControlSpec\cf2 (0, 1, \cf5 \\amp\cf2 , 0, 0),\
			\cf5 \\boostcut\cf2  -> \cf4 ControlSpec\cf2 (-20, 20, units: \cf6 " dB"\cf2 ),\
			\
			\cf5 \\pan\cf2  -> \cf4 ControlSpec\cf2 (-1, 1, default: 0),\
			\cf5 \\detune\cf2  -> \cf4 ControlSpec\cf2 (-20, 20, default: 0, units: \cf6 " Hz"\cf2 ),\
			\cf5 \\rate\cf2  -> \cf4 ControlSpec\cf2 (0.125, 8, \cf5 \\exp\cf2 , 0, 1),\
			\cf5 \\beats\cf2  -> \cf4 ControlSpec\cf2 (0, 20, units: \cf6 " Hz"\cf2 ),\
			\
			\cf5 \\delay\cf2  -> \cf4 ControlSpec\cf2 (0.0001, 1, \cf5 \\exp\cf2 , 0, 0.3, units: \cf6 " secs"\cf2 )\
		]);\
	\}\
\}\
\
\
\cf3 // Warps specify the mapping from 0..1 and the control range\
\cf2 \
\cf4 Warp\cf2  \{\
	\cf4 classvar\cf2  <>warps;\
	\cf4 var\cf2  <>spec;\
	*new \{ \cf4 arg\cf2  spec;\
		^\cf4 super\cf2 .newCopyArgs(spec.asSpec);\
	\}\
	map \{ \cf4 arg\cf2  value; ^value \}\
	unmap \{ \cf4 arg\cf2  value; ^value \}\
\
	*asWarp \{ \cf4 arg\cf2  spec; ^\cf4 this\cf2 .new(spec) \}\
	*initClass \{\
		\cf3 // support Symbol::asWarp\
\cf2 		warps = \cf4 IdentityDictionary\cf2 [\
			\cf5 \\lin\cf2  -> \cf4 LinearWarp\cf2 ,\
			\cf5 \\exp\cf2  -> \cf4 ExponentialWarp\cf2 ,\
			\cf5 \\sin\cf2  -> \cf4 SineWarp\cf2 ,\
			\cf5 \\cos\cf2  -> \cf4 CosineWarp\cf2 ,\
			\cf5 \\amp\cf2  -> \cf4 FaderWarp\cf2 ,\
			\cf5 \\db\cf2  -> \cf4 DbFaderWarp\cf2 \
		];\
		\cf3 // CurveWarp is specified by a number, not a Symbol\
\cf2 	\}\
	\
\}\
\
\cf4 LinearWarp\cf2  : \cf4 Warp\cf2  \{\
	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^value * spec.range + spec.minval\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^(value - spec.minval) / spec.range\
	\}\
\}\
\
\cf4 ExponentialWarp\cf2  : \cf4 Warp\cf2  \{\
	\cf3 // minval and maxval must both be non zero and have the same sign.\
\cf2 	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^(spec.ratio ** value) * spec.minval\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^log(value/spec.minval) / log(spec.ratio)\
	\}\
\}\
\
\cf4 CurveWarp\cf2  : \cf4 Warp\cf2  \{\
	\cf4 var\cf2  a, b, grow, curve;\
	*new \{ \cf4 arg\cf2  spec, curve = -2;\
		\cf3 // prevent math blow up\
\cf2 		if (abs(curve) < 0.001, \{ ^\cf4 LinearWarp\cf2 (spec) \});\
		\
		^\cf4 super\cf2 .new(spec.asSpec).init(curve);\
	\}\
	init \{ \cf4 arg\cf2  argCurve;\
		curve = argCurve;\
		grow = exp(curve);\
		a = spec.range / (1.0 - grow);\
		b = spec.minval + a;\
	\}\
	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^b - (a * pow(grow, value));\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^log((b - value) / a) / curve\
	\}\
\}\
\
\cf4 CosineWarp\cf2  : \cf4 LinearWarp\cf2  \{\
	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^\cf4 super\cf2 .map(cos(pi * value) * 0.5 + 0.5)\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		acos(\cf4 super\cf2 .unmap(value) * 2.0 - 1.0) / pi\
	\}\
\}\
\
\cf4 SineWarp\cf2  : \cf4 LinearWarp\cf2  \{\
	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^\cf4 super\cf2 .map(sin(0.5pi * value))\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		asin(\cf4 super\cf2 .unmap(value)) / 0.5pi\
	\}\
\}\
\
\cf4 FaderWarp\cf2  : \cf4 Warp\cf2  \{\
	\cf3 //  useful mapping for amplitude faders\
\cf2 	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^value.squared\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^value.sqrt\
	\}\
\}\
\
\cf4 DbFaderWarp\cf2  : \cf4 Warp\cf2  \{\
	\cf3 //  useful mapping for amplitude faders\
\cf2 	map \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from [0..1] to spec range\
\cf2 		^value.squared.ampdb\
	\}\
	unmap \{ \cf4 arg\cf2  value;\
		\cf3 // maps a value from spec range to [0..1]\
\cf2 		^value.dbamp.sqrt\
	\}\
\}\
\
}