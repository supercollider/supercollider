{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 Peep\cf0  \{\
	*ar \{ arg in, label=\cf3 "peep"\cf0 , period=1;\
		^\cf2 this\cf0 .multiNew('audio', in, label, period)\
	\}\
	*kr \{ arg in, label=\cf3 "peep"\cf0 , period=1;\
		^\cf2 this\cf0 .multiNew('control', in, label, period)\
	\}\
\}\
		}