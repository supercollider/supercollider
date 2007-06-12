{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green112\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\
\cf2 XY\cf0  : \cf2 MultiOutUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  xscale = 1.0, yscale = 1.0, xoff = 0.0, yoff = 0.0, rot = 0.0, rate = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , xscale, yscale, xoff, yoff, rot, rate)\
	\}\
	*kr \{ \cf2 arg\cf0  xscale = 1.0, yscale = 1.0, xoff = 0.0, yoff = 0.0, rot = 0.0, rate = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 ,  xscale, yscale, xoff, yoff, rot, rate)\
	\}\
	init \{ \cf2 arg\cf0  ... argInputs;\
		inputs = argInputs;\
		channels = [ \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,0), \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,1), \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,2) ];\
		^channels\
	\}\
\}	\
\
\cf2 ImageWarp\cf0  : \cf2 MultiOutUGen\cf0  \{\
	\cf2 var\cf0  pic;\
	\
	*ar \{ \
		\cf2 arg\cf0  pic, x=0.0, y=0.0, interpolationType = 1;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , pic, x, y, interpolationType)\
	\}\
	*kr \{\
		\cf2 arg\cf0  pic, x=0.0, y=0.0, interpolationType = 1;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , pic, x, y, interpolationType)\
	\}\
	\
	init \{ \cf2 arg\cf0  aPic ... theInputs;\
 		inputs = theInputs;\
 		pic = aPic;\
		channels = [ \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,0), \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,1), \cf2 OutputProxy\cf0 .kr1(\cf2 this\cf0 ,2) ];\
		^channels\
 	\}\
\}\
}