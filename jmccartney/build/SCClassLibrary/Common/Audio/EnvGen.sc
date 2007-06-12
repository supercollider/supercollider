{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Done\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src)\
	\}\
\}\
\
\cf3 FreeSelf\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src)\
	\}\
\}\
\
\cf3 PauseSelf\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src)\
	\}\
\}\
\
\cf3 FreeSelfWhenDone\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src)\
	\}\
\}\
\
\cf3 PauseSelfWhenDone\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src)\
	\}\
\}\
\
\cf3 Pause\cf2  : \cf3 UGen\cf2  \{	\
	*kr \{ \cf3 arg\cf2  src, id;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , src, id)\
	\}\
\}\
\
\cf5 // the doneAction arg lets you cause the EnvGen to stop or end the \
// synth without having to use a PauseSelfWhenDone or FreeSelfWhenDone ugen. \
// It is more efficient.\
// doneAction = 0   do nothing when the envelope has ended.\
// doneAction = 1   pause the synth running, it is still resident.\
// doneAction = 2   remove the synth and deallocate it.\
\cf2 \
\cf3 EnvGen\cf2  : \cf3 UGen\cf2  \{ \cf5 // envelope generator	\
\cf2 	*ar \{ \cf3 arg\cf2  envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;\
		^\cf3 this\cf2 .multiNewList([\cf4 'audio'\cf2 , gate, levelScale, levelBias, timeScale, doneAction] \
			++ envelope.asArray)\
	\}\
	*kr \{ \cf3 arg\cf2  envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;\
		^\cf3 this\cf2 .multiNewList([\cf4 'control'\cf2 , gate, levelScale, levelBias, timeScale, doneAction] \
			++ envelope.asArray)\
	\}\
\}\
\
\cf5 //BufEnvGen : UGen \{ // envelope generator	\
//	*ar \{ arg bufnum, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;\
//		^this.multiNew('audio', bufnum, gate, levelScale, levelBias, timeScale, doneAction)\
//	\}\
//	*kr \{ arg bufnum, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;\
//		^this.multiNew('control', bufnum, gate, levelScale, levelBias, timeScale, doneAction)\
//	\}\
//\}\
\cf2 \
\cf3 Linen\cf2  : \cf3 UGen\cf2  \{\
	*kr \{ \cf3 arg\cf2  gate = 1.0, attackTime = 0.01, susLevel = 1.0, releaseTime = 1.0, doneAction = 0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , gate, attackTime, susLevel, releaseTime, doneAction)\
	\}\
\}\
\
}