{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green112\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 PhaseVocoderUGen\cf0  : \cf2 UGen\cf0 \
\{\
	\cf2 var\cf0  <fftSize, cosTable, inputWindow, outputWindow;\
	\
	init \{ \cf2 arg\cf0  argFFTSize ... argInputs;\
		inputs = argInputs;\
		fftSize = argFFTSize;\
		cosTable = \cf2 Library\cf0 .at(\cf3 \\signal\cf0 , \cf3 \\cosTable\cf0 , fftSize);\
		inputWindow = outputWindow = \cf2 Library\cf0 .at(\cf3 \\signal\cf0 , \cf3 \\welch\cf0 , fftSize);\
	\}\
\}\
\
\cf2 PV_Noop\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	\cf4 // this class just for debugging\
\cf0 	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in)\
	\}\
\}\
\
\cf2 PV_Scope\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in)\
	\}\
	init \{ \cf2 arg\cf0  argFFTSize ... argInputs;\
		inputs = argInputs;\
		fftSize = argFFTSize;\
		cosTable = \cf2 Library\cf0 .at(\cf3 \\signal\cf0 , \cf3 \\cosTable\cf0 , fftSize);\
		inputWindow = outputWindow = \cf2 Library\cf0 .at(\cf3 \\signal\cf0 , \cf3 \\hanning\cf0 , fftSize);\
	\}\
\}\
\
\cf2 PV_TimeAverageScope\cf0  : \cf2 PV_Scope\cf0  \{\}\
\
\cf2 PV_ApxTest\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	\cf4 // this class just for debugging\
\cf0 	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in)\
	\}\
\}\
\
\cf2 PV_MagSquared\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in)\
	\}\
\}\
\
\cf2 PV_MagNoise\cf0  : \cf2 PV_MagSquared\cf0  \{\}\
\
\
\cf2 PV_AboveThresh\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, threshold = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, threshold)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, threshold = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, threshold)\
	\}\
\}\
\
\cf2 PV_BelowThresh\cf0  : \cf2 PV_AboveThresh\cf0  \{\}\
\cf2 PV_MagClip\cf0  : \cf2 PV_AboveThresh\cf0  \{\}\
\cf2 PV_LocalMax\cf0  : \cf2 PV_AboveThresh\cf0  \{\}\
\
\cf2 PV_BinShift\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, stretch = 1.0, shift = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, stretch, shift)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, stretch = 1.0, shift = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, stretch, shift)\
	\}\
\}\
\
\cf2 PV_MagShift\cf0  : \cf2 PV_BinShift\cf0  \{\}\
\
\
\cf2 PV_BrickWall\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, wipe)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, wipe)\
	\}\
\}\
\
\cf2 PV_MagOnePole\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, feedback = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, feedback)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, feedback = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, feedback)\
	\}\
\}\
\
\cf2 PV_MagAllTimeAverage\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in)\
	\}\
\}\
\
\cf2 PV_MagPeakDecay\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, decay = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, decay)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, decay = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, decay)\
	\}\
\}\
\
\cf2 PV_MagSmear\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, bins = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, bins)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, bins = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, bins)\
	\}\
\}\
\
\cf2 PV_TimeSmear\cf0  : \cf2 PV_MagSmear\cf0  \{\}\
\
\cf2 PV_RandComb\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, wipe, trig)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, wipe, trig)\
	\}\
\}\
\
\cf2 PV_MagFreeze\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, freeze = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, freeze)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, freeze = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, freeze)\
	\}\
\}\
\
\cf2 PV_LoBitEncoder\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, levels = 8.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, levels)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, levels = 8.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, levels)\
	\}\
\}\
\
\cf2 PV_BinScramble\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0, width = 0.2, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, wipe, width, trig)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, wipe = 0.0, width = 0.2, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, wipe, width, trig)\
	\}\
\}\
\
\cf2 PV_Diffuser\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, trig)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, trig)\
	\}\
\}\
\
\cf2 PV_MagMultiply\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, inA, inB)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, inA, inB)\
	\}\
\}\
\
\cf2 PV_PhasePaste\cf0  : \cf2 PV_MagMultiply\cf0  \{\}\
\cf2 PV_Max\cf0  : \cf2 PV_MagMultiply\cf0  \{\}\
\cf2 PV_Min\cf0  : \cf2 PV_MagMultiply\cf0  \{\}\
\
\cf2 PV_BinWipe\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, inA, inB, wipe)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, inA, inB, wipe)\
	\}\
\}\
\
\cf2 PV_RandWipe\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, inA, inB, wipe, trig)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0, trig = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, inA, inB, wipe, trig)\
	\}\
\}\
\
\
\cf2 PV_RectComb\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, in, numTeeth, phase, width)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, in = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, in, numTeeth, phase, width)\
	\}\
\}\
\
\
\cf2 PV_RectComb2\cf0  : \cf2 PhaseVocoderUGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , fftSize, inA, inB, numTeeth, phase, width)\
	\}\
	*kr \{ \cf2 arg\cf0  fftSize = 1024, inA = 0.0, inB = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , fftSize, inA, inB, numTeeth, phase, width)\
	\}\
\}\
\
\
}