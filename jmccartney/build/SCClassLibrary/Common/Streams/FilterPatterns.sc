{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red191\green0\blue0;
\red0\green115\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 FilterPattern\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>pattern;\
\
	*new \{ \cf3 arg\cf2  pattern;\
		^\cf3 super\cf2 .new.pattern_(pattern)\
	\}\
\}\
\
\cf3 Pn\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>repeats;\
	*new \{ \cf3 arg\cf2  pattern, repeats;\
		^\cf3 super\cf2 .new(pattern).repeats_(repeats)\
	\}\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inval;\
			repeats.do(\{\
				inval = pattern.embedInStream(inval);\
			\});\
		\});\
	\}\
\}\
\
\
\cf3 FuncFilterPattern\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>func;\
	\
	*new \{ \cf3 arg\cf2  func, pattern;\
		^\cf3 super\cf2 .new(pattern).func_(func)\
	\}\
\}\
\
\cf3 Pcollect\cf2  : \cf3 FuncFilterPattern\cf2  \{\
	asStream \{\
		^pattern.asStream.collect(func);\
	\}\
\}\
\
\cf3 Pselect\cf2  : \cf3 FuncFilterPattern\cf2  \{\
	asStream \{\
		^pattern.asStream.select(func);\
	\}\
\}\
\
\cf3 Preject\cf2  : \cf3 FuncFilterPattern\cf2  \{\
	asStream \{\
		^pattern.asStream.reject(func);\
	\}\
\}\
\
\
\cf3 Pfset\cf2  : \cf3 FuncFilterPattern\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream, envir;\
\
		envir = \cf3 Event\cf2 .make(func);\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			event = event.copy;\
			event.putAll(envir);\
			stream.next(event);\
		\});\
	\}\
\}\
\
\cf3 Pset\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>name, <>value;\
	\
	*new \{ \cf3 arg\cf2  name, value, pattern;\
		^\cf3 super\cf2 .new(pattern).name_(name).value_(value)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			if (event.notNil, \{ event = event.copy.put(name, value) \});\
			stream.next(event);\
		\});\
	\}\
\}\
\
\cf3 Padd\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			if (event.notNil, \{ event = event.copy.put(name, event.at(name) + value) \});\
			stream.next(event);\
		\});\
	\}\
\}\
\
\cf3 Pmul\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			if (event.notNil, \{ event = event.copy.put(name, event.at(name) * value) \});\
			stream.next(event);\
		\});\
	\}\
\}\
\
\cf3 Pnot\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>name;\
	\
	*new \{ \cf3 arg\cf2  name, pattern;\
		^\cf3 super\cf2 .new(pattern).name_(name)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			event = event.copy;\
			event.put(name, event.at(name).not);\
			stream.next(event);\
		\});\
	\}\
\}\
\
\
\cf3 Psetpost\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			event = stream.next(event);\
			if (event.notNil, \{ event.copy.put(name, value) \});\
		\});\
	\}\
\}\
\
\cf3 Paddpost\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			event = stream.next(event);\
			if (event.notNil, \{ event.copy.put(name, event.at(name) + value) \});\
		\});\
	\}\
\}\
\
\cf3 Pmulpost\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  event;\
			event = stream.next(event);\
			if (event.notNil, \{ event.copy.put(name, event.at(name) * value) \});\
		\});\
	\}\
\}\
\
\
\cf3 Psetp\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  valStream, evtStream, val, outevent;\
			valStream = value.asStream;\
			while(\{\
				val = valStream.next;\
				val.notNil\
			\},\{\
				evtStream = pattern.asStream;\
				while(\{\
					inevent = inevent.copy;\
					inevent.put(name, val);\
					outevent = evtStream.next(inevent);\
					outevent.notNil\
				\},\{\
					inevent = outevent.yield;\
				\});\
			\});\
		\});\
	\}\
\}\
\
\cf3 Paddp\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  valStream, evtStream, val, outevent, prevval;\
			\
			prevval = inevent.at(name);\
			valStream = value.asStream;\
			while(\{\
				val = valStream.next;\
				val.notNil\
			\},\{\
				evtStream = pattern.asStream;\
				while(\{\
					inevent = inevent.copy;\
					inevent.put(name, prevval + val);\
					outevent = evtStream.next(inevent);\
					outevent.notNil\
				\},\{\
					inevent = outevent.yield;\
					prevval = inevent.at(name);\
				\});\
			\});\
		\});\
	\}\
\}\
\
\cf3 Pmulp\cf2  : \cf3 Pset\cf2  \{\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  valStream, evtStream, val, outevent, prevval;\
			\
			prevval = inevent.at(name);\
			valStream = value.asStream;\
			while(\{\
				val = valStream.next;\
				val.notNil\
			\},\{\
				evtStream = pattern.asStream;\
				while(\{\
					inevent = inevent.copy;\
					inevent.put(name, prevval * val);\
					outevent = evtStream.next(inevent);\
					outevent.notNil\
				\},\{\
					inevent = outevent.yield;\
					prevval = inevent.at(name);\
				\});\
			\});\
		\});\
	\}\
\}\
\
\
\cf3 Pfin\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>count;\
	*new \{ \cf3 arg\cf2  count, pattern;\
		^\cf3 super\cf2 .new(pattern).count_(count)\
	\}\
	asStream \{ \
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  item, stream;\
		\
			stream = pattern.asStream;\
			\
			count.value.do(\{\
				inevent = stream.next(inevent).yield;\
			\});\
		\});\
	\}\
\}	\
\
\cf3 Pfindur\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>dur, <>tolerance;\
	*new \{ \cf3 arg\cf2  dur, pattern, tolerance = 0.001;\
		^\cf3 super\cf2 .new(pattern).dur_(dur).tolerance_(tolerance)\
	\}\
	asStream \{ \
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  item, stream, delta, elapsed = 0.0, nextElapsed;\
		\
			stream = pattern.asStream;\
			\
			loop (\{\
				inevent = stream.next(inevent);\
				delta = inevent.delta;\
				nextElapsed = elapsed + delta;\
				if (nextElapsed.round(tolerance) >= dur, \{\
					\cf4 // must always copy an event before altering it.\
\cf2 					inevent = inevent.copy; \
					\cf4 // fix delta time.\
\cf2 					inevent.put(\cf5 \\delta\cf2 , dur - elapsed);\
					inevent = inevent.yield;\
					\cf4 // the note is not truncated here. maybe you want that..\
\cf2 					\
					\cf4 // end of pattern\
\cf2 					\cf3 nil\cf2 .alwaysYield;\
				\},\{\
					elapsed = nextElapsed;\
					inevent = inevent.yield;\
				\});\
			\});\
		\});\
	\}\
\}	\
\
\cf3 Plag\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>lag;\
	*new \{ \cf3 arg\cf2  lag, pattern;\
		^\cf3 super\cf2 .new(pattern).lag_(lag)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream, item;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;	\
			\cf3 var\cf2  newevent;	\
			newevent = inevent.copy;\
			newevent.put(\cf5 'freq'\cf2 , \cf5 \\rest\cf2 );\
			newevent.put(\cf5 'dur'\cf2 , lag);\
			inevent = newevent.yield;\
			loop (\{\
				inevent = stream.next(inevent).yield;\
			\});\
		\});\
	\}\
\}\
\
\cf4 //Pbindf : FilterPattern \{\
//	var <>patternpairs;\
//	*new \{ arg ... pairs;\
//		if (pairs.size.even, \{ "Pbindf should have odd number of args.\\n".error; this.halt \});\
//		^super.new(pairs.last).patternpairs_(pairs)\
//	\}\
//	asStream \{\
//		var patstream, streampairs, endval;\
//		\
//		patstream = pattern.asStream;\
//		\
//		streampairs = patternpairs.copy;\
//		endval = streampairs.size - 2;\
//		forBy (1, endval, 2, \{ arg i;\
//			streampairs.put(i, streampairs.at(i).asStream);\
//		\});\
//		\
//		^FuncStream.new(\{ arg inevent;\
//			var sawNil = false;\
//			\
//			inevent = patstream.next(inevent);\
//			if (inevent.isNil, \{\
//				nil\
//			\},\{\
//				forBy (0, endval, 2, \{ arg i;\
//					var name, stream, item;\
//					name = streampairs.at(i);\
//					stream = streampairs.at(i+1);\
//				\
//					item = stream.next(inevent);\
//					if (item.isNil, \{\
//						sawNil = true;\
//					\},\{\
//						inevent.put(name, item);\
//					\});\
//				\});\
//				if (sawNil, \{ nil \},\{ inevent \});\
//			\});\
//		\},\{\
//			patstream.reset;\
//		\});\
//	\}\
//\}\
\cf2 \
\
\cf3 Pbindf\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>patternpairs;\
	*new \{ \cf3 arg\cf2  ... pairs;\
		if (pairs.size.even, \{ \
			\cf6 "Pbindf should have odd number of args.\\n"\cf2 .error; \
			pairs.dump;\
			\cf3 this\cf2 .halt \
		\});\
		^\cf3 super\cf2 .new(pairs.last).patternpairs_(pairs)\
	\}\
	asStream \{\
		\cf3 var\cf2  patstream, streampairs, endval;\
		\
		patstream = pattern.asStream;\
\
		streampairs = patternpairs.copy;\
		endval = streampairs.size - 2;\
		forBy (1, endval, 2, \{ \cf3 arg\cf2  i;\
			streampairs.put(i, streampairs.at(i).asStream);\
		\});\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  sawNil = \cf3 false\cf2 ;\
			inevent = inevent.copy;\
			forBy (0, endval, 2, \{ \cf3 arg\cf2  i;\
				\cf3 var\cf2  name, stream, streamout;\
				name = streampairs.at(i);\
				stream = streampairs.at(i+1);\
				\
				streamout = stream.next(inevent);\
				\
				if (streamout.isNil, \{\
					sawNil = \cf3 true\cf2 ;\
				\},\{\
					if (name.isKindOf(\cf3 SequenceableCollection\cf2 ), \{					\
						streamout.do(\{ \cf3 arg\cf2  val, i;\
							inevent.put(name.at(i), val);\
						\});\
					\},\{\
						inevent.put(name, streamout);\
					\});\
				\});\
			\});\
			if (sawNil, \{ \cf3 nil\cf2  \},\{ \
				inevent = patstream.next(inevent);\
			\});\
		\},\{\
			patstream.reset;\
			\
			streampairs = patternpairs.copy;\
			endval = streampairs.size - 1;\
			forBy (1, endval, 2, \{ \cf3 arg\cf2  i;\
				streampairs.put(i, streampairs.at(i).reset);\
			\});\
		\});\
	\}\
\}\
\
\cf3 Pstutter\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>n;\
	*new \{ \cf3 arg\cf2  n, pattern;\
		^\cf3 super\cf2 .new(pattern).n_(n)\
	\}\
	asStream \{ \
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  outevent, stream;\
		\
			stream = pattern.asStream;\
		\
			while (\{\
				(outevent = stream.next(inevent)).notNil\
			\},\{\
				n.do(\{\
					inevent = outevent.copy.yield;\
				\});\
			\});\
		\});\
	\}\
\}	\
\
\cf3 Pfx\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>effects, <>numChannels, <>postroll, <>decay;\
	\
	*new \{ \cf3 arg\cf2  pattern, effects, numChannels=2, postroll = 0.5, decay = 0.5;\
		^\cf3 super\cf2 .new(pattern).effects_(effects).numChannels_(numChannels)\
			.postroll_(postroll).decay_(decay)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  out, tspawn, synth, outevent, copyevent, numfx;\
			\
			\cf4 // create a synth\
\cf2 			effects = effects.asArray;\
			numfx = effects.size;\
			tspawn = \cf3 Array\cf2 .new(numfx);\
			inevent.use(\{\
				synth = \cf3 Synth\cf2 .new(\{\
					\cf3 Mix\cf2 .arFill(numfx, \{ \cf3 arg\cf2  i;\
						\cf3 var\cf2  z;\
						out = \cf3 BasicSpawn\cf2 .ar(numChannels);\
						tspawn = tspawn.add(out.source);\
						effects.at(i).valueEnvir( out );\
					\});							 \
				\});\
			\});\
			if (synth.notNil, \{	\
				inevent.at(\cf5 \\fxArray\cf2 ).clipAt( inevent.at(\cf5 \\fxIndex\cf2 ) ).add( synth );							\
				while (\{\
					copyevent = inevent.copy;\
					copyevent.put(\cf5 \\fxArray\cf2 , tspawn);\
					(outevent = stream.next(copyevent)).notNil\
				\},\{\
					inevent = outevent.yield;\
				\});\
				synth.sched(postroll, 1, \{ synth.fadeEnd(decay) \});\
			\});\
		\});\
	\}\
\}\
\
\cf3 Pmono\cf2  : \cf3 FilterPattern\cf2  \{\
	\cf3 var\cf2  <>instruments, <>numChannels, <>postroll, <>decay;\
	\
	*new \{ \cf3 arg\cf2  pattern, instruments, numChannels=2, postroll = 0.5, decay = 0.5;\
		^\cf3 super\cf2 .new(pattern).instruments_(instruments).numChannels_(numChannels)\
			.postroll_(postroll).decay_(decay)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream;\
		\
		stream = pattern.asStream;\
		\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inevent;\
			\cf3 var\cf2  out, synth, outevent, copyevent, numMonos, plugs;\
			\
			\cf4 // create a synth\
\cf2 			numMonos = instruments.size;\
			plugs = \cf3 Array\cf2 .new(numMonos);\
			inevent.use(\{\
				synth = \cf3 Synth\cf2 .new(\{\
					\cf3 var\cf2  env;\
					env = \cf3 Env\cf2 .new( #[1,1,1,0], [0.5, postroll, decay], \cf5 \\linear\cf2 , 1);\
					\cf3 this\cf2 .makePlugs;\
					\cf3 Mix\cf2 .arFill(numMonos, \{ \cf3 arg\cf2  i;\
						\cf3 var\cf2  instrFunc, iargs;\
						instrFunc = instruments.at(i);\
						iargs = \cf3 this\cf2 .makePlugs(instrFunc, inevent, plugs);\
						instrFunc.valueArrayEnvir( iargs )\
					\}) * \cf3 EnvGen\cf2 .kr(env);							 \
				\});\
			\});\
			if (synth.notNil, \{\
				inevent.at(\cf5 \\fxArray\cf2 ).at( inevent.at(\cf5 \\fxIndex\cf2 ) ).add( synth );							\
				while (\{\
					copyevent = inevent.copy;\
					copyevent.put(\cf5 \\inMono\cf2 , plugs);\
					(outevent = stream.next(copyevent)).notNil\
				\},\{\
					inevent = outevent.yield;\
				\});\
				synth.release;\
			\});\
		\});\
	\}\
	makePlugs \{ \cf3 arg\cf2  instrFunc, event, plugs;\
		\cf3 var\cf2  def, numArgs, iplugs, iargs, argNames, plug, argName;\
		def = instrFunc.def;\
		numArgs = def.numArgs;\
		iplugs = \cf3 Array\cf2 .new(numArgs);\
		iargs = \cf3 Array\cf2 .new(numArgs);\
		argNames = def.argNames;\
		numArgs.do(\{ \cf3 arg\cf2  i; \
			\cf3 var\cf2  defVal;\
			defVal = def.prototypeFrame.at(i);\
			argName = argNames.at(i);\
			if (defVal.isKindOf(\cf3 SimpleNumber\cf2 ), \{\
				plug = \cf3 Plug\cf2 .kr(defVal, 0.0, argName);\
				iplugs.add( plug );\
				iargs.add( plug );\
			\},\{\
				iargs.add( event.at(argName) );\
			\});\
		\});\
		plugs.add(iplugs);\
		^iargs;\
	\}\
	setPlugs \{ \cf3 arg\cf2  event;\
		event.at(\cf5 \\inMono\cf2 ).at( event.at(\cf5 \\monoIndex\cf2 ) ).do(\{ \cf3 arg\cf2  plug; \
			plug.source = event.at(plug.index); \
		\});\
	\}		\
\}\
	\
	\
\cf3 Pwhile\cf2  : \cf3 FuncFilterPattern\cf2  \{\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inval;\
			while(\{ func.value \},\{\
				inval = pattern.embedInStream(inval);\
			\});\
		\});\
	\}\
\}\
\
}