{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red191\green0\blue0;
\red0\green115\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 SynthDef\cf2  \{	\
	\cf3 var\cf2  <>name;\
	\cf3 var\cf2  <>controlNames;\
	\cf3 var\cf2  <>controls;\
	\cf3 var\cf2  <>children;\
		\
	\cf3 var\cf2  <>constants;\
	\cf3 var\cf2  <>constantSet;\
	\
	\cf4 // topo sort\
\cf2 	\cf3 var\cf2  <>arAvailable, <>krAvailable;\
	\
	*new \{ \cf3 arg\cf2  name, ugenGraphFunc;\
		^\cf3 super\cf2 .new.name_(name).build(ugenGraphFunc).postln;\
	\}\
	\
	addUGen \{ \cf3 arg\cf2  ugen;\
		ugen.synthIndex = children.size;\
		children = children.add(ugen)\
	\}\
	removeUGen \{ \cf3 arg\cf2  ugen;\
		children = children.remove(ugen)\
	\}\
	\
	build \{ \cf3 arg\cf2  func;\
		\cf3 var\cf2  controls;\
		\
		\cf3 UGen\cf2 .buildSynthDef = \cf3 this\cf2 ;\
		constants = \cf3 Dictionary\cf2 .new;\
		constantSet = \cf3 Set\cf2 .new;\
		\
		controls = \cf3 this\cf2 .buildControlsFromArgs(func);\
		\cf5 \\func\cf2 .postln;\
		func.valueArray(controls);\
		\
		\cf5 \\checkInputs\cf2 .postln;\
		if (\cf3 this\cf2 .checkInputs.not, \{ ^\cf3 nil\cf2  \});\
		\cf5 \\collectConstants\cf2 .postln;\
		\cf3 this\cf2 .collectConstants;\
		\
		\cf4 // re-sort graph. reindex.\
\cf2 		\cf4 //this.dumpUGens;\
\cf2 		\cf5 \\topologicalSort\cf2 .postln;\
		\cf3 this\cf2 .topologicalSort;\
		\cf5 \\indexUGens\cf2 .postln;\
		\cf3 this\cf2 .indexUGens;\
		\cf4 //"\\n".postln;\
\cf2 		\cf4 //this.dumpUGens;\
\cf2 	\}\
	buildControlsFromArgs \{ \cf3 arg\cf2  func;\
		\cf3 var\cf2  def, size, names, values, controls;\
		\cf3 var\cf2  irnames, irvalues, ircontrols, irpositions;\
		\cf3 var\cf2  krnames, krvalues, krcontrols, krpositions;\
\
		def = func.def;\
		names = def.argNames;\
		if (names.isNil, \{ ^\cf3 nil\cf2  \});\
		\
		\cf4 // OK what we do here is separate the ir and kr rate arguments,\
\cf2 		\cf4 // create one Control ugen for all of the ir and one for all of \
\cf2 		\cf4 // the kr, and then construct the argument array from combining \
\cf2 		\cf4 // the OutputProxies of these two Control ugens in the original order.\
\cf2 		size = names.size;\
		values = def.prototypeFrame.copy.extend(size);\
		values = values.collect(\{ \cf3 arg\cf2  value; value ? 0.0 \});\
		names.do(\{ \cf3 arg\cf2  name, i; \
			\cf3 var\cf2  c, value;\
			c = name.asString.at(0);\
			value = values.at(i);\
			if (c == $i, \{\
				irnames = irnames.add(name);\
				irvalues = irvalues.add(value);\
				irpositions = irpositions.add(i);\
			\},\{\
				krnames = krnames.add(name);\
				krvalues = krvalues.add(value);\
				krpositions = krpositions.add(i);\
			\});\
		\});\
		if (irnames.size > 0, \{\
			ircontrols = \cf3 Control\cf2 .names(irnames).ir(irvalues);\
		\});\
		if (krnames.size > 0, \{\
			krcontrols = \cf3 Control\cf2 .names(krnames).kr(krvalues);\
		\});\
		controls = \cf3 Array\cf2 .newClear(size);\
		ircontrols.asArray.do(\{ \cf3 arg\cf2  control, i; \
			controls.put(irpositions.at(i), control);\
		\});\
		krcontrols.asArray.do(\{ \cf3 arg\cf2  control, i; \
			controls.put(krpositions.at(i), control);\
		\});\
		\
		^controls\
	\}\
	\
	checkInputs \{\
		children.do(\{ \cf3 arg\cf2  ugen; \
			if (ugen.checkInputs.not, \{ \
				\cf3 Post\cf2  << ugen.class << \cf6 " has bad inputs: "\cf2  \
					<< ugen.inputs << \cf6 ".\\n"\cf2 ;\
				^\cf3 false\cf2  \
			\});\
		\});\
		^\cf3 true\cf2 \
	\}\
	\
	addConstant \{ \cf3 arg\cf2  value;\
		if (constantSet.includes(value).not, \{\
			constantSet.add(value);\
			constants.put(value, constants.size);\
		\});\
	\}\
	collectConstants \{\
		children.do(\{ \cf3 arg\cf2  ugen; \
			ugen.collectConstants;\
		\});\
	\}\
	writeDefFile \{\
		[\cf3 this\cf2 ].writeDefFile(name)\
	\}\
	writeDef \{ \cf3 arg\cf2  file;\
		\cf4 // This describes the file format for the synthdef files.\
\cf2 		\
		file.putPascalString(name);\
		\
		\cf3 this\cf2 .writeConstants(file);\
\
		file.putInt16(controls.size);\
		controls.do(\{ \cf3 arg\cf2  item;\
			file.putFloat(item);\
		\});\
				\
		file.putInt16(controlNames.size);\
		controlNames.do(\{ \cf3 arg\cf2  item;\
			if (item.name.notNil, \{\
				file.putPascalString(item.name);\
				file.putInt16(item.index);\
			\});\
		\});\
\
		\cf4 //children.size.postln;\
\cf2 		file.putInt16(children.size);\
		children.do(\{ \cf3 arg\cf2  item;\
			item.writeDef(file);\
		\});\
		\cf4 //[this.class.name, file.length].postln;\
\cf2 	\}\
	writeConstants \{ \cf3 arg\cf2  file;\
		\cf3 var\cf2  array;\
		array = \cf3 Array\cf2 .newClear(constants.size);\
		constants.keysValuesDo(\{ \cf3 arg\cf2  value, index;\
			array.put(index, value);\
		\});\
\
		\cf4 //constants.size.postln;\
\cf2 		\cf4 //array.postln;\
\cf2 \
		file.putInt16(constants.size);\
		array.do(\{ \cf3 arg\cf2  item; \
			file.putFloat(item) \
		\});\
	\}\
\
	\cf4 // multi channel expansion causes a non optimal breadth-wise ordering of the graph.\
\cf2 	\cf4 // the topological sort below follows branches in a depth first order,\
\cf2 	\cf4 // so that cache performance of connection buffers is optimized.\
\cf2 	\
	\
	initTopoSort \{\
		arAvailable = krAvailable = \cf3 nil\cf2 ;\
		children.do(\{ \cf3 arg\cf2  ugen;\
			ugen.antecedents = \cf3 Set\cf2 .new;\
			ugen.descendants = \cf3 Set\cf2 .new;\
		\});\
		children.do(\{ \cf3 arg\cf2  ugen;\
			\cf4 // this populates the descendants and antecedents\
\cf2 			ugen.initTopoSort;\
		\});\
		children.do(\{ \cf3 arg\cf2  ugen;\
			ugen.descendants = ugen.descendants.asSortedList;\
			ugen.makeAvailable; \cf4 // all ugens with no antecedents are made available\
\cf2 		\});\
	\}\
	topologicalSort \{\
		\cf3 var\cf2  outStack;\
		\cf3 this\cf2 .initTopoSort;\
		while (\{ krAvailable.size + arAvailable.size > 0 \},\{\
			while (\{ krAvailable.size > 0 \},\{\
				outStack = krAvailable.pop.schedule(outStack);\
			\});\
			if (arAvailable.size > 0, \{\
				outStack = arAvailable.pop.schedule(outStack);\
			\});\
		\});\
		\
		children = outStack;\
	\}\
	indexUGens \{\
		children.do(\{ \cf3 arg\cf2  ugen, i;\
			ugen.synthIndex = i;\
		\});\
	\}\
	\
	dumpUGens \{\
		name.postln;\
		children.do(\{ \cf3 arg\cf2  ugen, i;\
			\cf3 var\cf2  inputs;\
			if (ugen.inputs.notNil, \{\
				inputs = ugen.inputs.collect(\{ \cf3 arg\cf2  in; \
					if (in.isKindOf(\cf3 UGen\cf2 ), \{ in.source \},\{ in \})\
				\});\
			\});\
			[i, ugen, ugen.rate, ugen.synthIndex, inputs].postln;\
		\});\
	\}\
\}\
\
}