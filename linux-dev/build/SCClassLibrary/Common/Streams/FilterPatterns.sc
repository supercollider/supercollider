
FilterPattern : Pattern {
	var <>pattern;

	*new { arg pattern;
		^super.new.pattern_(pattern)
	}
}

Pn : FilterPattern {
	var <>repeats;
	*new { arg pattern, repeats;
		^super.new(pattern).repeats_(repeats)
	}
	asStream {
		^Routine.new({ arg inval;
			repeats.do({
				inval = pattern.embedInStream(inval);
			});
		});
	}
}


FuncFilterPattern : FilterPattern {
	var <>func;
	
	*new { arg func, pattern;
		^super.new(pattern).func_(func)
	}
}

Pcollect : FuncFilterPattern {
	asStream {
		^pattern.asStream.collect(func);
	}
}

Pselect : FuncFilterPattern {
	asStream {
		^pattern.asStream.select(func);
	}
}

Preject : FuncFilterPattern {
	asStream {
		^pattern.asStream.reject(func);
	}
}


Pfset : FuncFilterPattern {
	asStream {
		var stream, envir;

		envir = Event.make(func);
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = event.copy;
			event.putAll(envir);
			stream.next(event);
		});
	}
}

Pset : FilterPattern {
	var <>name, <>value;
	
	*new { arg name, value, pattern;
		^super.new(pattern).name_(name).value_(value)
	}
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			if (event.notNil, { event = event.copy.put(name, value) });
			stream.next(event);
		});
	}
}

Padd : Pset {
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			if (event.notNil, { event = event.copy.put(name, event.at(name) + value) });
			stream.next(event);
		});
	}
}

Pmul : Pset {
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			if (event.notNil, { event = event.copy.put(name, event.at(name) * value) });
			stream.next(event);
		});
	}
}

Pnot : FilterPattern {
	var <>name;
	
	*new { arg name, pattern;
		^super.new(pattern).name_(name)
	}
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = event.copy;
			event.put(name, event.at(name).not);
			stream.next(event);
		});
	}
}


Psetpost : Pset {
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = stream.next(event);
			if (event.notNil, { event.copy.put(name, value) });
		});
	}
}

Paddpost : Pset {
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = stream.next(event);
			if (event.notNil, { event.copy.put(name, event.at(name) + value) });
		});
	}
}

Pmulpost : Pset {
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = stream.next(event);
			if (event.notNil, { event.copy.put(name, event.at(name) * value) });
		});
	}
}


Psetp : Pset {
	asStream {
		^Routine.new({ arg inevent;
			var valStream, evtStream, val, outevent;
			valStream = value.asStream;
			while({
				val = valStream.next;
				val.notNil
			},{
				evtStream = pattern.asStream;
				while({
					inevent = inevent.copy;
					inevent.put(name, val);
					outevent = evtStream.next(inevent);
					outevent.notNil
				},{
					inevent = outevent.yield;
				});
			});
		});
	}
}

Paddp : Pset {
	asStream {
		^Routine.new({ arg inevent;
			var valStream, evtStream, val, outevent, prevval;
			
			prevval = inevent.at(name);
			valStream = value.asStream;
			while({
				val = valStream.next;
				val.notNil
			},{
				evtStream = pattern.asStream;
				while({
					inevent = inevent.copy;
					inevent.put(name, prevval + val);
					outevent = evtStream.next(inevent);
					outevent.notNil
				},{
					inevent = outevent.yield;
					prevval = inevent.at(name);
				});
			});
		});
	}
}

Pmulp : Pset {
	asStream {
		^Routine.new({ arg inevent;
			var valStream, evtStream, val, outevent, prevval;
			
			prevval = inevent.at(name);
			valStream = value.asStream;
			while({
				val = valStream.next;
				val.notNil
			},{
				evtStream = pattern.asStream;
				while({
					inevent = inevent.copy;
					inevent.put(name, prevval * val);
					outevent = evtStream.next(inevent);
					outevent.notNil
				},{
					inevent = outevent.yield;
					prevval = inevent.at(name);
				});
			});
		});
	}
}


Pfin : FilterPattern {
	var <>count;
	*new { arg count, pattern;
		^super.new(pattern).count_(count)
	}
	asStream { 
		^Routine.new({ arg inevent;
			var item, stream;
		
			stream = pattern.asStream;
			
			count.value.do({
				inevent = stream.next(inevent).yield;
			});
		});
	}
}	

Pfindur : FilterPattern {
	var <>dur, <>tolerance;
	*new { arg dur, pattern, tolerance = 0.001;
		^super.new(pattern).dur_(dur).tolerance_(tolerance)
	}
	asStream { 
		^Routine.new({ arg inevent;
			var item, stream, delta, elapsed = 0.0, nextElapsed;
		
			stream = pattern.asStream;
			
			loop ({
				inevent = stream.next(inevent);
				delta = inevent.delta;
				nextElapsed = elapsed + delta;
				if (nextElapsed.round(tolerance) >= dur, {
					// must always copy an event before altering it.
					inevent = inevent.copy; 
					// fix delta time.
					inevent.put(\delta, dur - elapsed);
					inevent = inevent.yield;
					// the note is not truncated here. maybe you want that..
					
					// end of pattern
					nil.alwaysYield;
				},{
					elapsed = nextElapsed;
					inevent = inevent.yield;
				});
			});
		});
	}
}	

Plag : FilterPattern {
	var <>lag;
	*new { arg lag, pattern;
		^super.new(pattern).lag_(lag)
	}
	asStream {
		var stream, item;
		
		stream = pattern.asStream;
		
		^Routine.new({ arg inevent;	
			var newevent;	
			newevent = inevent.copy;
			newevent.put('freq', \rest);
			newevent.put('dur', lag);
			inevent = newevent.yield;
			loop ({
				inevent = stream.next(inevent).yield;
			});
		});
	}
}

//Pbindf : FilterPattern {
//	var <>patternpairs;
//	*new { arg ... pairs;
//		if (pairs.size.even, { "Pbindf should have odd number of args.\n".error; this.halt });
//		^super.new(pairs.last).patternpairs_(pairs)
//	}
//	asStream {
//		var patstream, streampairs, endval;
//		
//		patstream = pattern.asStream;
//		
//		streampairs = patternpairs.copy;
//		endval = streampairs.size - 2;
//		forBy (1, endval, 2, { arg i;
//			streampairs.put(i, streampairs.at(i).asStream);
//		});
//		
//		^FuncStream.new({ arg inevent;
//			var sawNil = false;
//			
//			inevent = patstream.next(inevent);
//			if (inevent.isNil, {
//				nil
//			},{
//				forBy (0, endval, 2, { arg i;
//					var name, stream, item;
//					name = streampairs.at(i);
//					stream = streampairs.at(i+1);
//				
//					item = stream.next(inevent);
//					if (item.isNil, {
//						sawNil = true;
//					},{
//						inevent.put(name, item);
//					});
//				});
//				if (sawNil, { nil },{ inevent });
//			});
//		},{
//			patstream.reset;
//		});
//	}
//}


Pbindf : FilterPattern {
	var <>patternpairs;
	*new { arg ... pairs;
		if (pairs.size.even, { 
			"Pbindf should have odd number of args.\n".error; 
			pairs.dump;
			this.halt 
		});
		^super.new(pairs.last).patternpairs_(pairs)
	}
	asStream {
		var patstream, streampairs, endval;
		
		patstream = pattern.asStream;

		streampairs = patternpairs.copy;
		endval = streampairs.size - 2;
		forBy (1, endval, 2, { arg i;
			streampairs.put(i, streampairs.at(i).asStream);
		});
		
		^FuncStream.new({ arg inevent;
			var sawNil = false;
			inevent = inevent.copy;
			forBy (0, endval, 2, { arg i;
				var name, stream, streamout;
				name = streampairs.at(i);
				stream = streampairs.at(i+1);
				
				streamout = stream.next(inevent);
				
				if (streamout.isNil, {
					sawNil = true;
				},{
					if (name.isSequenceableCollection, {					
						streamout.do({ arg val, i;
							inevent.put(name.at(i), val);
						});
					},{
						inevent.put(name, streamout);
					});
				});
			});
			if (sawNil, { nil },{ 
				inevent = patstream.next(inevent);
			});
		},{
			patstream.reset;
			
			streampairs = patternpairs.copy;
			endval = streampairs.size - 1;
			forBy (1, endval, 2, { arg i;
				streampairs.put(i, streampairs.at(i).reset);
			});
		});
	}
}

Pstutter : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.new(pattern).n_(n)
	}
	asStream { 
		^Routine.new({ arg inevent;
			var outevent, stream;
		
			stream = pattern.asStream;
		
			while ({
				(outevent = stream.next(inevent)).notNil
			},{
				n.do({
					inevent = outevent.copy.yield;
				});
			});
		});
	}
}	

Pfx : FilterPattern {
	var <>effects, <>numChannels, <>postroll, <>decay;
	
	*new { arg pattern, effects, numChannels=2, postroll = 0.5, decay = 0.5;
		^super.new(pattern).effects_(effects).numChannels_(numChannels)
			.postroll_(postroll).decay_(decay)
	}
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^Routine.new({ arg inevent;
			var out, tspawn, synth, outevent, copyevent, numfx;
			
			// create a synth
			effects = effects.asArray;
			numfx = effects.size;
			tspawn = Array.new(numfx);
			inevent.use({
				synth = Synth.new({
					Mix.arFill(numfx, { arg i;
						var z;
						out = BasicSpawn.ar(numChannels);
						tspawn = tspawn.add(out.source);
						effects.at(i).valueEnvir( out );
					});							 
				});
			});
			if (synth.notNil, {	
				inevent.at(\fxArray).clipAt( inevent.at(\fxIndex) ).add( synth );							
				while ({
					copyevent = inevent.copy;
					copyevent.put(\fxArray, tspawn);
					(outevent = stream.next(copyevent)).notNil
				},{
					inevent = outevent.yield;
				});
				synth.sched(postroll, 1, { synth.fadeEnd(decay) });
			});
		});
	}
}

Pmono : FilterPattern {
	var <>instruments, <>numChannels, <>postroll, <>decay;
	
	*new { arg pattern, instruments, numChannels=2, postroll = 0.5, decay = 0.5;
		^super.new(pattern).instruments_(instruments).numChannels_(numChannels)
			.postroll_(postroll).decay_(decay)
	}
	asStream {
		var stream;
		
		stream = pattern.asStream;
		
		^Routine.new({ arg inevent;
			var out, synth, outevent, copyevent, numMonos, plugs;
			
			// create a synth
			numMonos = instruments.size;
			plugs = Array.new(numMonos);
			inevent.use({
				synth = Synth.new({
					var env;
					env = Env.new( #[1,1,1,0], [0.5, postroll, decay], \linear, 1);
					this.makePlugs;
					Mix.arFill(numMonos, { arg i;
						var instrFunc, iargs;
						instrFunc = instruments.at(i);
						iargs = this.makePlugs(instrFunc, inevent, plugs);
						instrFunc.valueArrayEnvir( iargs )
					}) * EnvGen.kr(env);							 
				});
			});
			if (synth.notNil, {
				inevent.at(\fxArray).at( inevent.at(\fxIndex) ).add( synth );							
				while ({
					copyevent = inevent.copy;
					copyevent.put(\inMono, plugs);
					(outevent = stream.next(copyevent)).notNil
				},{
					inevent = outevent.yield;
				});
				synth.release;
			});
		});
	}
	makePlugs { arg instrFunc, event, plugs;
		var def, numArgs, iplugs, iargs, argNames, plug, argName;
		def = instrFunc.def;
		numArgs = def.numArgs;
		iplugs = Array.new(numArgs);
		iargs = Array.new(numArgs);
		argNames = def.argNames;
		numArgs.do({ arg i; 
			var defVal;
			defVal = def.prototypeFrame.at(i);
			argName = argNames.at(i);
			if (defVal.isSimpleNumber, {
				plug = Plug.kr(defVal, 0.0, argName);
				iplugs.add( plug );
				iargs.add( plug );
			},{
				iargs.add( event.at(argName) );
			});
		});
		plugs.add(iplugs);
		^iargs;
	}
	setPlugs { arg event;
		event.at(\inMono).at( event.at(\monoIndex) ).do({ arg plug; 
			plug.source = event.at(plug.index); 
		});
	}		
}
	
	
Pwhile : FuncFilterPattern {
	asStream {
		^Routine.new({ arg inval;
			while({ func.value },{
				inval = pattern.embedInStream(inval);
			});
		});
	}
}

