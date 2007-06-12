//
//EventPlayer {
//	var <>parentEvent;
//	
//	*new { arg event;
//		var player;
//		event = event ?? { () };
//		event.parent = this.defaultParentEvent;
//		player = super.newCopyArgs(event);
//		event[\player] = player;
//		^player
//	}
//	*defaultParentEvent {
//		^this.subclassResponsibility(thisMethod)
//	}
//	playEvent { arg event;
//		event.parent = parentEvent;
//		event.use({ ~finish.value; });
//		event.postln;
//	}
//}
//
//NotePlayer : EventPlayer {
//	classvar <>defaultParentEvent;
//	
//	*initClass {
//
//		SynthDef(\default, { arg out=0, freq=440, amp=0.1, pan=0, gate=1;
//			var z;
//			z = LPF.ar(
//				Mix.new(VarSaw.ar(freq + [0, Rand(-0.4,0.0), Rand(0.0,0.4)], 0, 0.3)),
//				XLine.kr(Rand(4000,5000), Rand(2500,3200), 1)) * Linen.kr(gate, 0.01, amp * 0.7, 0.3, 2);
//			Out.ar(out, Pan2.ar(z, pan));
//		}, [\ir]).store;
//		
//		defaultParentEvent = Event.make({
//			// fill prototype event with default property value
//			~tempo = nil;
//			
//			~dur = 1.0;
//			~stretch = 1.0;
//			~legato = 0.8;
//			~sustain = #{ ~dur * ~legato * ~stretch };
//			~lag = 0.0;
//			~strum = 0.0;
//			
//			~amp = #{ ~db.dbamp };
//			~db = -20.0;
//			~velocity = 64; 		// MIDI units 0-127
//			~pan = 0.0; 			// pan center
//			
//			~mtranspose = 0;
//			~gtranspose = 0.0;
//			~ctranspose = 0.0;
//			
//			~octave = 5.0;
//			~root = 0.0;		// root of the scale
//			~degree = 0;
//			~scale = #[0, 2, 4, 5, 7, 9, 11]; // diatonic major scale
//			~stepsPerOctave = 12.0;
//			~detune   = 0.0;		// detune in Hertz
//			
//			~note = #{
//				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
//			};
//			~midinote = #{
//				((~note.value + ~gtranspose + ~root) / ~stepsPerOctave + ~octave) * 12.0; 
//			};
//			~freq = #{
//				(~midinote.value + ~ctranspose).midicps;
//			};
//
//			~instrument = \default;
//			
//			~group = 0;
//			~out = 0;
//			~addAction = 0;
//			
//			~finish = #{
//				// do final calculations
//				~freq = ~freq.value + ~detune;
//				~amp = ~amp.value;
//				~sustain = ~sustain.value;
//				~server = ~server.value;
//			};
//									
//			~player = NotePlayer.new;
//			~synthLib = SynthDescLib.global;
//
//			~server = { Server.default };
//		});
//	}
//	playOneEvent { arg server, addAction, group, desc, lag, sustain, msgArgs;
//		var id;
////		var ttbeats, ttseconds;
//		
//		id = server.nextNodeID;
//		
//		//send the note on bundle
//		server.sendBundle(lag, [9, desc.name, id, addAction, group] ++ msgArgs); 
////		ttbeats = thisThread.beats;
////		ttseconds = thisThread.seconds;
//				
//		if (desc.hasGate) {
//			// send note off bundle.
//			thisThread.clock.sched(sustain, { 
//	//			if (thisThread.seconds <= ttseconds) {
//	//				[\ooo, ttbeats, ttseconds, thisThread.beats, thisThread.seconds, dur].postln;
//	//				TempoClock.default.prDump;
//	//			};
//				server.sendBundle(lag, [15, id, \gate, 0]); //15 == n_set
//			});
//		};
//	}
//	playEvent { arg event;
//		var freqs, lag, dur, strum, sustain, desc, bndl, server, addAction, group, tempo;
//		event.parent = parentEvent;
//		event.use({
//			~finish.value; // finish the event
//			freqs = ~freq;
//			
//			tempo = ~tempo;
//			if (tempo.notNil) {
//				thisThread.clock.tempo = tempo;
//			};
//							
//			if (freqs.isKindOf(Symbol).not) {
//				server = ~server;
//				addAction = ~addAction;
//				group = ~group;
//				lag = ~lag + server.latency;
//				strum = ~strum;
//				sustain = ~sustain;
//	
//				desc = ~synthLib.synthDescs[~instrument.asSymbol];
//				if (desc.isNil) { error("instrument " ++ ~instrument ++ " not found."); ^nil };
//
//				bndl = desc.msgFunc.valueEnvir.flop;
//				bndl.do {|msgArgs, i|
//					this.playOneEvent(server, addAction, group, desc, i * strum + lag, sustain, msgArgs);
//				};
//			};
//		});
//	}
//}
//
//TempoEventPlayer : EventPlayer {
//	classvar <>defaultParentEvent;
//	
//	*initClass {
//		defaultParentEvent = Event.make({
//			~tempo = nil;
//			
//			~dur = 1.0;
//			~stretch = 1.0;
//		});
//	}
//	playEvent { arg event;
//		var tempo;
//		tempo = event[\tempo];
//		if (tempo.notNil) {
//			thisThread.clock.tempo = tempo;
//		};
//	}
//}
//
///*
//
//delta time is in beats.
//convert delta time to absolute time.
//note off in beats
//
//
//delta time in beats.
//latency in seconds.
//
//tempo = beats/second
//wake up time = delta time/tempo - next latency
//
//
//
//
//one pattern
//
//prototype events:
//poly note w/ dur
//poly note w/ separarate on and off
//mono note
//tempo change
//control bus change
//
//Pevent(event)  
//merge events.
//	a) make a copy of the larger event and put the fields of the smaller event in it.
//	b)if properties are not a dictionary then can append properties.
//		make a dictionary only at the end.
//	c) parent chaining
//	
//
//
//Ppar -> merge parallel lists.
//*/
//
///*
//
//		defaultParentEvent = Event.make({
//			// fill prototype event with default property value
//			~tempo = nil;
//			
//			~dur = 1.0;
//			~stretch = 1.0;
//			~legato = 0.8;
//			~sustain = #{ ~dur * ~legato * ~stretch };
//			~lag = 0.0;
//			~strum = 0.0;
//			
//			~amp = #{ ~db.dbamp };
//			~db = -20.0;
//			~velocity = 64; 		// MIDI units 0-127
//			~pan = 0.0; 			// pan center
//			
//			~mtranspose = 0;
//			~gtranspose = 0.0;
//			~ctranspose = 0.0;
//			
//			~octave = 5.0;
//			~root = 0.0;		// root of the scale
//			~degree = 0;
//			~scale = #[0, 2, 4, 5, 7, 9, 11]; // diatonic major scale
//			~stepsPerOctave = 12.0;
//			~detune   = 0.0;		// detune in Hertz
//			
//			~note = #{
//				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
//			};
//			~midinote = #{
//				((~note.value + ~gtranspose + ~root) / ~stepsPerOctave + ~octave) * 12.0; 
//			};
//			~freq = #{
//				(~midinote.value + ~ctranspose).midicps;
//			};
//
//			~instrument = \default;
//			
//			~group = 0;
//			~out = 0;
//			~addAction = 0;
//			
//			~finish = #{
//				// do final calculations
//				~freq = ~freq.value + ~detune;
//				~amp = ~amp.value;
//				~sustain = ~sustain.value;
//			};
//									
//			~player = NotePlayer.new;
//			~synthLib = SynthDescLib.global;
//
//			~server = Server.default;
//		});
//
//
//		pitchEvent = ().make {
//			~mtranspose = 0;
//			~gtranspose = 0.0;
//			~ctranspose = 0.0;
//			
//			~octave = 5.0;
//			~root = 0.0;		// root of the scale
//			~degree = 0;
//			~scale = #[0, 2, 4, 5, 7, 9, 11]; // diatonic major scale
//			~stepsPerOctave = 12.0;
//			~detune   = 0.0;		// detune in Hertz
//			
//			~note = #{
//				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
//			};
//			~midinote = #{
//				((~note.value + ~gtranspose + ~root) / ~stepsPerOctave + ~octave) * 12.0; 
//			};
//			~freq = #{
//				(~midinote.value + ~ctranspose).midicps;
//			};
//		};
//
//		durEvent = ().make {
//			~tempo = nil;
//			
//			~dur = 1.0;
//			~stretch = 1.0;
//			~legato = 0.8;
//			~sustain = #{ ~dur * ~legato * ~stretch };
//			~lag = 0.0;
//			~strum = 0.0;
//		};
//		
//		ampEvent = ().make {
//			~amp = #{ ~db.dbamp };
//			~db = -20.0;
//			~velocity = 64; 		// MIDI units 0-127
//			~pan = 0.0; 			// pan center
//		};
//		
//		serverEvent = ().make {
//			~instrument = \default;
//			
//			~group = 0;
//			~out = 0;
//			~addAction = 0;
//									
//			~synthLib = SynthDescLib.global;
//
//			~server = { Server.default };
//		};
//
//		noteEvent = ().putAll(pitchEvent, ampEvent, durEvent, serverEvent);
//		
//		noteEvent[\play] = {
//			var freqs, lag, dur, strum, sustain, desc, bndl, server, addAction, group, tempo;
//			freqs = ~freq;
//			
//			tempo = ~tempo;
//			if (tempo.notNil) {
//				thisThread.clock.tempo = tempo;
//			};
//							
//			if (freqs.isKindOf(Symbol).not) {
//				server = ~server;
//				addAction = ~addAction;
//				group = ~group;
//				lag = ~lag + server.latency;
//				strum = ~strum;
//				sustain = ~sustain;
//	
//				desc = ~synthLib.synthDescs[~instrument.asSymbol];
//				if (desc.isNil) { error("instrument " ++ ~instrument ++ " not found."); ^nil };
//
//				bndl = desc.msgFunc.valueEnvir.flop;
//				bndl.do {|msgArgs, i|
//					var id, latency;
//					
//					latency = i * strum + lag;
//					id = server.nextNodeID;
//					
//					//send the note on bundle
//					server.sendBundle(lag, [9, desc.name, id, addAction, group] ++ msgArgs); 
//							
//					if (desc.hasGate) {
//						// send note off bundle.
//						thisThread.clock.sched(sustain) { 
//							server.sendBundle(lag, [15, id, \gate, 0]); //15 == n_set
//						};
//					};
//				};
//			};
//		};
//		
//		nodeSetEvent = ().putAll(pitchEvent, ampEvent, durEvent, serverEvent);
//		
//		nodeSetEvent[\play] = {
//			var freqs, lag, dur, strum, sustain, desc, bndl, server, addAction, group, tempo;
//			freqs = ~freq;
//			
//			tempo = ~tempo;
//			if (tempo.notNil) {
//				thisThread.clock.tempo = tempo;
//			};
//							
//			if (freqs.isKindOf(Symbol).not) {
//				server = ~server;
//				addAction = ~addAction;
//				group = ~group;
//				lag = ~lag + server.latency;
//				strum = ~strum;
//				sustain = ~sustain;
//	
//				desc = ~synthLib.synthDescs[~instrument.asSymbol];
//				if (desc.isNil) { error("instrument " ++ ~instrument ++ " not found."); ^nil };
//
//				bndl = ([15, ~id] ++ desc.msgFunc.valueEnvir).flop;
//				bndl.do {|msgArgs, i|
//					var latency;
//					
//					latency = i * strum + lag;
//					
//					server.sendBundle(lag, msgArgs); 
//				};
//			};
//		};
//		
//		controlBusEvent = ().putAll(durEvent, serverEvent);
//		
//		controlBusEvent[\play] = {
//			var server, latency, array;
//			server = ~server;
//			lag = ~lag + server.latency;
//			array = ~array;
//			server.sendBundle(latency, [\c_setn, ~out, array.size] ++ array);
//		}
//};
//
//*/
