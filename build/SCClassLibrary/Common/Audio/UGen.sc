{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 UGen\cf0  : \cf2 AbstractFunction\cf0  \{\
	\cf2 classvar\cf0  <>buildSynthDef; \cf3 // the synth currently under construction\
\cf0 	\cf2 var\cf0  <>synthDef;\
	\cf2 var\cf0  <>inputs;\
	\cf2 var\cf0  <>rate = \cf4 'audio'\cf0 ;\
	\
	\cf2 var\cf0  <>synthIndex = -1;\
	\
	\cf2 var\cf0  <>antecedents, <>descendants; \cf3 // topo sorting\
\cf0 	\
	\cf3 // instance creation\
\cf0 	*new1 \{ \cf2 arg\cf0  rate ... args;\
		^\cf2 super\cf0 .new.rate_(rate).addToSynth.performList(\cf4 \\init\cf0 , args); \
	\}\
 	*multiNew \{ \cf2 arg\cf0  ... args;\
		^\cf2 this\cf0 .multiNewList(args);\
	\}\
	*multiNewList \{ \cf2 arg\cf0  args;\
		\cf2 var\cf0  size = 1, newArgs, results;	\
		args.do(\{ \cf2 arg\cf0  item; \
			(item.class == \cf2 Array\cf0 ).if(\{ size = max(size, item.size) \});\
		\});\
		if (size == 1, \{ ^\cf2 this\cf0 .performList(\cf4 \\new1\cf0 , args) \});\
		newArgs = \cf2 Array\cf0 .newClear(args.size);\
		results = \cf2 Array\cf0 .newClear(size);\
		size.do(\{ \cf2 arg\cf0  i;\
			args.do(\{ \cf2 arg\cf0  item, j;\
				newArgs.put(j, if (item.class == \cf2 Array\cf0 , \{ item.wrapAt(i) \},\{ item \}));\
			\});\
			results.put(i, \cf2 this\cf0 .performList(\cf4 \\new1\cf0 , newArgs));\
		\});\
		^results\
	\}\
\
 	init \{ \cf2 arg\cf0  ... theInputs;\
 		\cf3 // store the inputs as an array\
\cf0  		inputs = theInputs;\
 	\}\
 	\
 	madd \{ \cf2 arg\cf0  mul = 1.0, add = 0.0; 		\
 		^\cf2 MulAdd\cf0 (\cf2 this\cf0 , mul, add);\
 	\}\
 	 	\
	addToSynth \{\
		synthDef = buildSynthDef;\
		if (synthDef.notNil, \{ synthDef.addUGen(\cf2 this\cf0 ) \});\
	\}\
	\
	collectConstants \{\
		inputs.do(\{ \cf2 arg\cf0  input;\
			if (input.isKindOf(\cf2 SimpleNumber\cf0 ), \{ synthDef.addConstant(input.asFloat)  \});\
		\}); \
	\}\
	\
	*new \{ ^\cf2 this\cf0 .shouldNotImplement \}\
	*newClear \{ ^\cf2 this\cf0 .shouldNotImplement \}\
			\
	source \{ ^\cf2 this\cf0  \}\
	isValidUGenInput \{ ^\cf2 true\cf0  \}\
	checkInputs \{ ^\cf2 true\cf0  \}\
\
	degreeToKey \{ \cf2 arg\cf0  scale, stepsPerOctave=12;\
		^\cf2 DegreeToKey\cf0 .kr(scale, \cf2 this\cf0 , stepsPerOctave)\
	\}\
	\
	outputIndex \{ ^0 \}\
	specialIndex \{ ^0 \}\
	\
	\cf3 // PRIVATE\
\cf0 	\cf3 // function composition\
\cf0 	composeUnaryOp \{ \cf2 arg\cf0  aSelector;\
		^\cf2 UnaryOpUGen\cf0 .new(aSelector, \cf2 this\cf0 )\
	\}\
	composeBinaryOp \{ \cf2 arg\cf0  aSelector, anInput;\
		if (anInput.isValidUGenInput, \{\
			^\cf2 BinaryOpUGen\cf0 .new(aSelector, \cf2 this\cf0 , anInput)\
		\},\{\
			anInput.performBinaryOpOnUGen(aSelector, \cf2 this\cf0 );\
		\});\
	\}\
	reverseComposeBinaryOp \{ \cf2 arg\cf0  aSelector, aUGen;\
		^\cf2 BinaryOpUGen\cf0 .new(aSelector, aUGen, \cf2 this\cf0 )\
	\}\
	composeNAryOp \{ \cf2 arg\cf0  aSelector, anArgList;\
		^\cf2 this\cf0 .notYetImplemented\
	\}\
	\
	\cf3 // complex support	\
\cf0 	\
	asComplex \{ ^\cf2 Complex\cf0 .new(\cf2 this\cf0 , 0.0) \}\
	performBinaryOpOnComplex \{ \cf2 arg\cf0  aSelector, aComplex; ^aComplex.perform(aSelector, \cf2 this\cf0 .asComplex) \}\
	\
	if \{ \cf2 arg\cf0  trueUGen, falseUGen;\
		^(\cf2 this\cf0  * (trueUGen - falseUGen)) + falseUGen;\
	\}\
\
	rateNumber \{\
		if (rate == \cf4 \\audio\cf0 , \{ ^2 \});\
		if (rate == \cf4 \\control\cf0 , \{ ^1 \});\
		^0\
	\}\
	writeInputSpec \{ \cf2 arg\cf0  file, synthDef;\
		file.putInt16(synthIndex); \
		file.putInt16(\cf2 this\cf0 .outputIndex);\
	\}\
	writeOutputSpec \{ \cf2 arg\cf0  file;\
		file.putInt8(\cf2 this\cf0 .rateNumber);\
	\}\
	writeOutputSpecs \{ \cf2 arg\cf0  file;\
		\cf2 this\cf0 .writeOutputSpec(file);\
	\}\
	numInputs \{ ^inputs.size \}\
	numOutputs \{ ^1 \}\
	\
	name \{ \cf2 \
\cf0 		\cf2 ^this\cf0 .class.name.asString;\
	\}\
	writeDef \{ \cf2 arg\cf0  file;\
		\cf3 //[\\WR, this.class.name, rate, this.numInputs, this.numOutputs].postln;\
\cf0 		\cf2 file\cf0 .putPascalString(this.name);\
		file.putInt8(\cf2 this\cf0 .rateNumber);\
		file.putInt16(\cf2 this\cf0 .numInputs);\
		file.putInt16(\cf2 this\cf0 .numOutputs); \
		file.putInt16(\cf2 this\cf0 .specialIndex);\
		\cf3 // write wire spec indices.\
\cf0 		inputs.do(\{ \cf2 arg\cf0  input; \
			input.writeInputSpec(file, synthDef);\
		\});\
		\cf2 this\cf0 .writeOutputSpecs(file);\
		\cf3 //[this.class.name, file.length].postln;\
\cf0 	\}\
\
\cf3 ///////////////////////////////////////////////////////////////\
\cf0 \
	initTopoSort \{\
		inputs.do(\{ \cf2 arg\cf0  input;\
			if (input.isKindOf(\cf2 UGen\cf0 ), \{\
				antecedents.add(input.source);\
				input.source.descendants.add(\cf2 this\cf0 );\
			\});\
		\})\
	\}\
	\
	makeAvailable \{\
		if (antecedents.size == 0, \{\
			if (\cf2 this\cf0 .rate == \cf4 'audio'\cf0 , \{\
				synthDef.arAvailable = synthDef.arAvailable.add(\cf2 this\cf0 );\
			\},\{\
				synthDef.krAvailable = synthDef.krAvailable.add(\cf2 this\cf0 );\
			\});\
		\});\
	\}\
	\
	removeAntecedent \{ \cf2 arg\cf0  ugen;\
		antecedents.remove(ugen);\
		\cf2 this\cf0 .makeAvailable;\
	\}\
		\
	schedule \{ \cf2 arg\cf0  outStack;\
		descendants.do(\{ \cf2 arg\cf0  ugen;\
			ugen.removeAntecedent(\cf2 this\cf0 );\
		\});\
		^outStack.add(\cf2 this\cf0 );\
	\}\
	< \{ \cf2 arg\cf0  that; ^\cf2 this\cf0 .synthIndex < that.synthIndex \}\
\}\
\
\cf2 MultiOutUGen\cf0  : \cf2 UGen\cf0  \{\
	\cf3 // a class for UGens with multiple outputs\
\cf0 	\cf2 var\cf0  channels;\
\
	initOutputs \{ \cf2 arg\cf0  numChannels, rate;\
		channels = \cf2 Array\cf0 .fill(numChannels, \{ \cf2 arg\cf0  i; \
			\cf2 OutputProxy\cf0 (rate, \cf2 this\cf0 , i); \
		\});\
		if (numChannels == 1, \{\
			^channels.at(0)\
		\});\
		^channels\
	\}\
	\
	numOutputs \{ ^channels.size \}\
	writeOutputSpecs \{ \cf2 arg\cf0  file;\
		channels.do(\{ \cf2 arg\cf0  output; output.writeOutputSpec(file); \});\
	\}\
	synthIndex_ \{ \cf2 arg\cf0  index;\
		channels.do(\{ \cf2 arg\cf0  output; output.synthIndex_(index); \});\
	\}\
\}	\
\
\cf2 OutputProxy\cf0  : \cf2 UGen\cf0  \{\
	\cf2 var\cf0  <>source, <>outputIndex;\
	*new \{ \cf2 arg\cf0  rate, itsSourceUGen, index;\
		^\cf2 super\cf0 .new1(rate, itsSourceUGen, index)\
	\}\
	addToSynth \{\
		synthDef = buildSynthDef;\
	\}\
	init \{ \cf2 arg\cf0  argSource, argIndex;\
		source = argSource;\
		outputIndex = argIndex;\
		synthIndex = source.synthIndex;\
	\}\
\}\
}