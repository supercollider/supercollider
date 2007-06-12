{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green115\blue0;
\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Env\cf0  \{ \
	\cf3 // envelope specification for an EnvGen, Env is not a UGen itself\
\cf0 	\cf3 // you must not change an envelope while an EnvGen is running.\
\cf0 	\cf3 // Therefore these inst vars are read only.\
\cf0 	\cf2 var\cf0  <levels;\
	\cf2 var\cf0  <times;\
	\cf2 var\cf0  <curves = \cf4 'lin'\cf0 ;		\cf3 // can also be 'exp', 'sin', 'cos', a float curve value, \
\cf0 								\cf3 // or an array of curve values\
\cf0 	\cf2 var\cf0  <releaseNode;	\cf3 // index of release level, if nil then ignore release;\
\cf0 	\cf2 var\cf0  <loopNode;		\cf3 // index of loop start level, if nil then does not loop;\
\cf0 	\
	\cf2 classvar\cf0  shapeNames;\
	\
	*new \{ \cf2 arg\cf0  levels=#[0,1,0], times=#[1,1], curve=\cf4 'lin'\cf0 , releaseNode, loopNode;\
		^\cf2 super\cf0 .newCopyArgs(levels, times, curve, releaseNode, loopNode)\
	\}\
	*initClass \{\
		shapeNames = \cf2 IdentityDictionary\cf0 [\
			\cf4 \\step\cf0  -> 0,\
			\cf4 \\lin\cf0  -> 1,\
			\cf4 \\linear\cf0  -> 1,\
			\cf4 \\exp\cf0  -> 2,\
			\cf4 \\exponential\cf0  -> 2,\
			\cf4 \\sin\cf0  -> 3,\
			\cf4 \\cos\cf0  -> 4\
		];	\
	\}\
	\cf3 // methods to make some typical shapes :\
\cf0 	\
	\cf3 // fixed duration envelopes\
\cf0 	*triangle \{ \cf2 arg\cf0  dur=1.0, level=1.0;\
		dur = dur * 0.5;\
		^\cf2 this\cf0 .new(\
			[0, level, 0],\
			[dur, dur]\
		)\
	\} \
	*sine \{ \cf2 arg\cf0  dur=1.0, level=1.0;\
		dur = dur * 0.5;\
		^\cf2 this\cf0 .new(\
			[0, level, 0],\
			[dur, dur],\
			\cf4 'sine'\cf0 \
		)\
	\} \
	*perc \{ \cf2 arg\cf0  attackTime=0.01, releaseTime=1.0, level=1.0, curve = -4.0;\
		^\cf2 this\cf0 .new(\
			[0, level, 0],\
			[attackTime, releaseTime], \
			curve\
		)\
	\} \
	\
	\
	\cf3 // envelopes with sustain\
\cf0 	*adsr \{ \cf2 arg\cf0  attackTime=0.01, decayTime=0.3, \
			sustainLevel=0.5, releaseTime=1.0,\
				peakLevel=1.0, curve = -4.0, bias = 0.0;\
		^\cf2 this\cf0 .new(\
			[0, peakLevel, peakLevel * sustainLevel, 0] + bias,\
			[attackTime, decayTime, releaseTime], \
			curve,\
			1\
		)\
	\}\
	\
	\cf3 // changed this..\
\cf0 	*linen \{ \cf2 arg\cf0  attackTime=0.01, sustainLevel=1.0, releaseTime=1.0, curve = \cf4 \\lin\cf0 ;\
		^\cf2 this\cf0 .asr(attackTime, sustainLevel, releaseTime, curve)\
	\}\
	*asr \{ \cf2 arg\cf0  attackTime=0.01, sustainLevel=1.0, releaseTime=1.0, curve = -4.0;\
		^\cf2 this\cf0 .new(\
			[0, sustainLevel, 0],\
			[attackTime, releaseTime], \
			curve,\
			0\
		)\
	\}\
	\
	\cf3 // blend two envelopes\
\cf0 	blend \{ \cf2 arg\cf0  argAnotherEnv, argBlendFrac=0.5;\
		^\cf2 this\cf0 .class.new(\
			levels.blend(argAnotherEnv.levels, argBlendFrac),\
			times.blend(argAnotherEnv.times, argBlendFrac),\
			curves.blend(argAnotherEnv.curves, argBlendFrac),\
			releaseNode,\
			loopNode\
		)\
	\}\
	plot \{\
		\cf2 var\cf0  timeScale;\
		timeScale = 0.01 / times.sum;\
		\cf2 Synth\cf0 .plot(\{ \cf2 arg\cf0  synth;\
			synth.releaseTime = 0.005;\
			\cf2 EnvGen\cf0 .ar(\cf2 this\cf0 , 1, 0, 1, 0, timeScale)\
		\}, 0.01)\
	\}\
	test \{ \cf2 arg\cf0  releaseTime = 3.0;\
		\cf2 Synth\cf0 .play(\{ \cf2 arg\cf0  synth;\
			synth.releaseTime = releaseTime;\
			\cf2 EnvGen\cf0 .ar(\cf2 this\cf0 , \cf2 FSinOsc\cf0 .ar(800, 0.3))\
		\})\
	\}\
	\
	shapeNumber \{ \cf2 arg\cf0  shapeName;\
		^shapeNames.at(shapeName) ? 5\
	\}\
	curveValue \{ \cf2 arg\cf0  curve;\
		if (curve.isValidUGenInput, \{ ^curve \},\{ ^0 \});\
	\}\
	asArray \{\
		\cf2 var\cf0  contents, curvesArray;\
		contents = [levels.at(0), times.size, releaseNode ? -1, loopNode ? -1];\
		curvesArray = curves.asArray;\
		times.size.do(\{ \cf2 arg\cf0  i;\
			contents = contents ++ [\
				levels.at(i+1),\
				times.at(i),\
				\cf2 this\cf0 .shapeNumber(curvesArray.wrapAt(i)),\
				\cf2 this\cf0 .curveValue(curvesArray.wrapAt(i))\
			];\
		\});	\
		^contents\
	\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf5 //	send \{ arg netAddr, bufnum;\
//		var array;\
//		array = this.asArray;\
//		netAddr.performList(\\sendMsg, "buf.setn", bufnum, 0, array.size, array);\
//	\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \}\
\
}