// Events are returned by Pattern Streams

Event : Environment {
	classvar defaultParentEvent;
	classvar <parentEvents;
	classvar <partialEvents;

	*new { arg n=8, proto, parent, know=true; 
		^super.new(n, proto, parent, know)
	}
	
	*default {
		^Event.new(8, nil, defaultParentEvent, true);
	}
	*silent { arg dur = 1.0;
		^Event.new(8, nil, defaultParentEvent, true).put(\type, \rest).put(\dur, dur)
	}
	
	next { arg inval; ^composeEvents(inval, this) }
	
	delta {
		_Event_Delta
		^this.primitiveFailed;
		/*
		// implemented by primitive for speed
		var delta;
		delta = this.at('delta');
		if (delta.notNil, { ^delta },{ ^this.at('dur') * this.at('stretch') });
		*/
	}
	play {
		if (parent.isNil) {
			parent = defaultParentEvent;
		};
		this.use {
			this[\play].value;	
		};
		^this.delta
	}
	
	printOn { arg stream, itemsPerLine = 5;
		var max, itemsPerLinem1, i=0;
		itemsPerLinem1 = itemsPerLine - 1;
		max = this.size;
		stream << "( ";
		this.keysValuesDo({ arg key, val; 
			stream <<< key << ": " << val; 
			if ((i=i+1) < max, { stream.comma.space;
				if (i % itemsPerLine == itemsPerLinem1, { stream.nl.space.space });
			});
		});
		stream << " )";
	}
	
	storeOn { arg stream, itemsPerLine = 5;
		var max, itemsPerLinem1, i=0;
		itemsPerLinem1 = itemsPerLine - 1;
		max = this.size;
		stream << "( ";
		this.keysValuesDo({ arg key, val; 
			stream <<< key << ": " <<< val; 
			if ((i=i+1) < max, { stream.comma.space;
				if (i % itemsPerLine == itemsPerLinem1, { stream.nl.space.space });
			});
		});
		stream << " )";
	}
	
	*initClass {
		Class.initClassTree(Server);
		Class.initClassTree(TempoClock);
		this.makeParentEvents;
		
		StartUp.add {
			SynthDef.writeOnce(\default, { arg out=0, freq=440, amp=0.1, pan=0, gate=1;
				var z;
				z = LPF.ar(
						Mix.new(VarSaw.ar(freq + [0, Rand(-0.4,0.0), Rand(0.0,0.4)], 0, 0.3)),
						XLine.kr(Rand(4000,5000), Rand(2500,3200), 1)
					) * Linen.kr(gate, 0.01, 0.7, 0.3, 2);
				OffsetOut.ar(out, Pan2.ar(z, pan, amp));
			}, [\ir]);
		};
		

	}
	
	*makeParentEvents {		
		// define useful event subsets.
		partialEvents = (
			pitchEvent: (
				mtranspose: 0,
				gtranspose: 0.0,
				ctranspose: 0.0,
				
				octave: 5.0,
				root: 0.0,		// root of the scale
				degree: 0,
				scale: #[0, 2, 4, 5, 7, 9, 11], // diatonic major scale
				stepsPerOctave: 12.0,
				detune: 0.0,		// detune in Hertz
				
				note: #{
					(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
				},
				midinote: #{
					((~note.value + ~gtranspose + ~root) / ~stepsPerOctave + ~octave) * 12.0; 
				},
				freq: #{
					(~midinote.value + ~ctranspose).midicps;
				},
				freqToScale: #{ arg self, freq; // conversion from frequency to scale value
					self.use {
						var midinote;
						midinote = (freq.cpsmidi - ~ctranspose);
						midinote / 12.0 - ~octave * ~stepsPerOctave - ~root - ~gtranspose
					}
				}
			),
	
			durEvent: (
				tempo: nil,
				dur: 1.0,
				stretch: 1.0,
				legato: 0.8,
				sustain: #{ ~dur * ~legato * ~stretch },
				lag: 0.0,
				strum: 0.0
			),
			
			ampEvent: (
				amp: #{ ~db.dbamp },
				db: -20.0,
				velocity: 64, 		// MIDI units 0-127
				pan: 0.0, 			// pan center
				trig: 0.5
			),
			
			serverEvent: (
				server: nil,
	
				synthLib: nil,
				instrument: \default,
				
				group: 1,
				out: 0,
				addAction: 0,
				
				hasGate: nil,
				msgFunc: nil,
				defaultMsgFunc: #{|freq = 440, amp = 0.1, pan = 0, out = 0| 
					[\freq, freq, \amp, amp, \pan, pan, \out, out] },
				
				// for \type \set
				args: #[\freq, \amp, \pan, \trig]
			),
			
			bufferEvent: (
				bufnum: 0,
				filename: "",
				frame: 0,
				numframes: 0,
				numchannels: 1,
				gencmd: \sine1,
				genflags: 7,
				genarray: [1],
				bufpos: 0,
				leaveOpen: 0
			),
			midiEvent: (
				midiEventFunctions: (
					noteOn:  #{ arg chan=0, midinote=60, amp=0.1;
							[chan, midinote, asInteger((amp * 127).clip(0, 127)) ] },					noteOff: #{ arg chan=0, midinote=60, amp=0.1;
							[ chan, midinote, asInteger((amp * 127).clip(0, 127)) ] },
					polyTouch: #{ arg chan=0, midinote=60, polyTouch=125;
											[ chan, midinote, polyTouch ] },
					control: #{ arg chan=0, ctlNum, control=125;
											[chan, ctlNum, control ] },
					program:  #{ arg chan=0, progNum=1; [ chan, progNum ] },
					touch:  #{ arg chan=0, val=125; [ chan, val ] },
					bend:  #{ arg chan=0, val=125; [ chan, val ] },
					allNotesOff: #{ arg chan=0; [chan] },
					smpte:	#{ arg frames=0, seconds=0, minutes=0, hours=0, frameRate=25;
											[frames, seconds, minutes, hours, frameRate] },
					songPtr: #{ arg songPtr; [songPtr] },
					sysex: #{ arg uid, array; [uid, array] } // Int8Array
				),
				midicmd: \noteOn
			),
			nodeEvent: (
				delta: 0,
				
				addAction: 0,
				group: 1,
				latency: 0.2,
				instrument: \default,
				hasGate: true,
				
				stop: #{ 
					if (~hasGate == true) 
						{currentEnvironment.set(\gate, 0) } 
						{currentEnvironment.sendOSC([11, ~id]) };
					~isPlaying = false;
				},
				release:	#{ currentEnvironment.set(\gate, 0); ~isPlaying = false; },
				pause:	#{ currentEnvironment.sendOSC([12, ~id, false]); },
				resume:	#{ currentEnvironment.sendOSC([12, ~id, true]);  },	
				map: 	#{ | ev, key, busIndex | ev.sendOSC([14, ev[\id], key, busIndex]) },
			
				before: 	#{ | ev,target | 
					ev.sendOSC([18, ev[\id], target]); 
					ev[\group] = target; ev[\addAction] = 2;
				},
				after: 	#{  | ev, target | 
					ev.sendOSC([19, ev[\id], target]);
					ev[\group] = target; ev[\addAction] = 3;
				},
				headOf: 	#{ | ev, group | 
					ev.sendOSC([22, group, ev[\id]]);   
					ev[\group] = group; ev[\addAction] = 0;
				},
				tailOf: 	#{ |ev,  group | 
					ev.sendOSC([23, group, ev[\id]]);   
					ev[\group] = group; ev[\addAction] = 1;
				},
				isPlaying: #{ |ev| ^(ev[\isPlaying] == true) },
				isPlaying_: #{ | ev, flag | ev[\isPlaying] = flag; },
				nodeID: #{ |ev| ^ev[\id].asArray.last },	
				
				asEventStreamPlayer: #{|ev| ev }
			),

			playerEvent: (
				type: \note,
				play: #{
					var tempo, server;
					
					~finish.value;
					
					server = ~server ?? { Server.default };
					
					tempo = ~tempo;
					if (tempo.notNil) {
						thisThread.clock.tempo = tempo;
					};
					~eventTypes[~type].value(server);
				},
				eventTypes: (
					note_score:#{|server|
						var instrumentName, freqs, lag, dur, strum, sustain, desc, msgFunc;
						var bndl, synthLib, addAction, group, hasGate;
						var score;
						freqs = ~freq = ~freq.value + ~detune;
						score = ~score;
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							addAction = ~addAction;
							group = ~group;
							lag = ~lag;
							strum = ~strum;
							sustain = ~sustain = ~sustain.value;
							instrumentName = ~instrument.asSymbol;
							msgFunc = ~msgFunc;
							if (msgFunc.isNil) {
								synthLib = ~synthLib ?? { SynthDescLib.global };
								desc = synthLib.synthDescs[instrumentName];
								if (desc.notNil) { 
									hasGate = desc.hasGate;
									msgFunc = desc.msgFunc;
								}{
									hasGate = ~hasGate ? true;
									msgFunc = ~defaultMsgFunc;
								};
							}{
								hasGate = ~hasGate ? true;
							};
							bndl = msgFunc.valueEnvir.flop;
							bndl.do {|msgArgs, i|
								var id, latency;
								
								latency = i * strum + lag;
								id = server.nextNodeID;
								//send the note on bundle
								score.add([~absTime,  [\s_new, instrumentName, 
										id, addAction, group] ++ msgArgs]);
								if (hasGate) {
									// send note off bundle.
									score.add([sustain + ~absTime, [\n_set, id, \gate, 0]]); 
									
								};
							}
						};
					},				
					rest: #{},
					note: #{|server|
						var instrumentName, freqs, lag, dur, strum, sustain, desc, msgFunc;
						var bndl, synthLib, addAction, group, hasGate;
						
						freqs = ~freq = ~freq.value + ~detune;
										
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							addAction = ~addAction;
							group = ~group;
							lag = ~lag + server.latency;
							strum = ~strum;
							sustain = ~sustain = ~sustain.value;
							instrumentName = ~instrument.asSymbol;
							msgFunc = ~msgFunc;
							if (msgFunc.isNil) {
								synthLib = ~synthLib ?? { SynthDescLib.global };
								desc = synthLib.synthDescs[instrumentName];
								if (desc.notNil) { 
									hasGate = desc.hasGate;
									msgFunc = desc.msgFunc;
								}{
									hasGate = ~hasGate ? true;
									msgFunc = ~defaultMsgFunc;
								};
							}{
								hasGate = ~hasGate ? true;
							};
						//	~hasGate = hasGate;
							bndl = msgFunc.valueEnvir.flop;
							bndl.do {|msgArgs, i|
								var id, latency;
								
								latency = i * strum + lag;
								id = server.nextNodeID;
								
								//send the note on bundle
								server.sendBundle(latency, [\s_new, instrumentName, id, addAction, group] ++ msgArgs); 
										
								if (hasGate) {
									// send note off bundle.
									thisThread.clock.sched(sustain) { 
										server.sendBundle(latency, [\n_set, id, \gate, 0]); 
									};
								};
							}
						};
					},
					on: #{|server|
						var instrumentName, freqs, lag, dur, strum, desc, msgFunc;
						var bndl, synthLib, addAction, group;
						
						freqs = ~freq = ~freq.value + ~detune;
										
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							addAction = ~addAction;
							group = ~group;
							lag = ~lag + server.latency;
							strum = ~strum;
							~sustain = ~sustain.value;
							synthLib = ~synthLib.value;
							instrumentName = ~instrument.asSymbol;
							msgFunc = ~msgFunc ?? {
								synthLib = ~synthLib ?? { SynthDescLib.global };
								desc = synthLib.synthDescs[instrumentName];
								if (desc.notNil) { 
									desc.msgFunc;
								}{
									~defaultMsgFunc;
								};
							};
							bndl = ([\s_new, instrumentName, ~id, addAction, group] ++ msgFunc.valueEnvir).flop;
							bndl.do {|msgArgs, i|
								var latency;
								
								latency = i * strum + lag;
								
								//send the note on bundle
								server.sendBundle(latency, msgArgs);
							}
						};
					},
					set: #{|server|
						var instrumentName, freqs, lag, dur, strum, bndl;
						freqs = ~freq = ~freq.value + ~detune;
										
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							lag = ~lag + server.latency;
							~sustain = ~sustain.value;
							strum = ~strum;
				
							bndl = ([\n_set, ~id] ++ ~args.envirPairs).flop;
							bndl.do {|msgArgs, i|
								var latency;
								
								latency = i * strum + lag;
								
								server.sendBundle(latency, msgArgs); 
							};
						};
					},
					off: #{|server|
						var lag, dur, strum, hasGate, gate;
										
						lag = ~lag + server.latency;
						strum = ~strum;
						hasGate = ~hasGate ? true;
						gate = min(0.0, ~gate ? 0.0); // accept release times
						~id.asArray.do {|id, i|
							var latency = i * strum + lag;
							if(hasGate) {
								server.sendBundle(latency, [\n_set, id, \gate, gate]); 
							} {
								server.sendBundle(latency, [\n_free, id]);
							}
						};
					},
					
					group: #{|server|
						var group = ~group, addAction = ~addAction;
						var lag = ~lag + server.latency;
						server.listSendBundle(lag,
							~id.asArray.collect {|id, i|
								[\g_new, id, addAction, group]
							};
						);
					},
	

					kill: #{|server|
						var lag, dur, strum;
										
						lag = ~lag + server.latency;
						strum = ~strum;
			
						~id.asArray.do {|id, i|
							var latency;
							
							latency = i * strum + lag;
							
							server.sendBundle(latency, [\n_free, id]); 
						};
					},
	
					bus: #{|server|
						var lag, array;
						lag = ~lag + server.latency;
						array = ~array.asArray;
						server.sendBundle(lag, [\c_setn, ~out, array.size] ++ array);
					},
					
					gen: #{|server|
						var lag, genarray;
						lag = ~lag + server.latency;
						genarray = ~genarray;
						server.sendBundle(lag, [\b_gen, ~bufnum, ~gencmd, ~genflags]
							 ++ genarray);
					},
					load: #{|server|
						var lag;
						lag = ~lag + server.latency;
						server.sendBundle(lag, 
							[\b_allocRead, ~bufnum, ~filename, ~frame, ~numframes]);
					},
					read: #{|server|
						var lag;
						lag = ~lag + server.latency;
						server.sendBundle(lag, 
						[\b_read, ~bufnum, ~filename, ~frame, ~numframes, ~bufpos, ~leaveOpen]);
					},
					alloc: #{|server|
						var lag;
						lag = ~lag + server.latency;
						server.sendBundle(lag, [\b_alloc, ~bufnum, ~numframes, ~numchannels]);
					},
					free: #{|server|
						var lag;
						lag = ~lag + server.latency;
						server.sendBundle(lag, [\b_free, ~bufnum]);
					},
					
					midi: #{|server|
						var freqs, lag, dur, sustain, strum;
						var bndl, midiout, hasHate, midicmd;
						
						freqs = ~freq = ~freq.value + ~detune;
												
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							~midinote = freqs.cpsmidi;
							strum = ~strum;
							lag = ~lag + server.latency;
							sustain = ~sustain = ~sustain.value;
							midiout = ~midiout.value;
							hasHate = ~hasGate ? true;
							midicmd = ~midicmd;
							bndl = ~midiEventFunctions[midicmd].valueEnvir.asCollection;
							
							bndl = bndl.flop;
							
							bndl.do {|msgArgs, i|
									var latency;
									
									latency = i * strum + lag;
									
									if(latency == 0.0) {
										midiout.performList(midicmd, msgArgs)
									} {
										thisThread.clock.sched(latency, {
											midiout.performList(midicmd, msgArgs);
										})
									};
									if(hasHate and: { midicmd === \noteOn }) {
										thisThread.clock.sched(sustain + latency, { 
											midiout.noteOff(*msgArgs)
										});
									};
							};
						}
					},
					monoOff:  #{|server|
						var lag = ~lag + server.latency;
			
						if(~hasGate == false) {
							server.sendBundle(lag, [\n_free] ++ ~id);
						} {
							server.sendBundle(lag, *[\n_set, ~id, \gate, 0].flop); 
						};
						
					},
					
					monoSet: #{|server|
						var freqs, lag, bndl;
						
						freqs = ~freq = ~freq.value + ~detune;
										
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							lag = ~lag + server.latency;
							~sustain = ~sustain.value;
				
							bndl = ([\n_set, ~id] ++ ~msgFunc.valueEnvir).flop;
							server.sendBundle(server.latency, *bndl);
						};
					},
			
					monoNote:	#{ |server|	
						var bndl, id, ids, addAction, f;
						addAction = ~addAction;
						~freq = ~freq.value + ~detune;
						f = ~freq;
						~amp = ~amp.value;
						
						bndl = ( [\s_new, ~instrument, ids, addAction, ~group] 
							++ ~msgFunc.valueEnvir).flop;
						bndl.do { | b |
							id = server.nextNodeID;
							ids = ids.add(id);
							b[2] = id;
						};
				
						if ((addAction == 0) || (addAction == 3)) {
							bndl = bndl.reverse;
						};
						server.sendBundle(server.latency, *bndl);
						~updatePmono.value(ids, server);
					},
					
					Synth: #{ |server|	
						var instrumentName, desc, msgFunc;
						var bndl, synthLib, addAction, group, latency, ids, id, groupControls;
						~server = server;
						group = ~group;
						addAction = ~addAction;
						~freq = ~freq.value + ~detune;
						~amp = ~amp.value;
						ids = ~id;					
						synthLib = ~synthLib ?? { SynthDescLib.global };
						~defName = instrumentName = ~instrument.asSymbol;
						desc = synthLib.synthDescs[instrumentName];						if (desc.notNil) { 
							msgFunc = desc.msgFunc;
							~hasGate = desc.hasGate;
						}{
							msgFunc = ~defaultMsgFunc;
						};
					
						bndl = ( [\s_new, instrumentName, ids, addAction, group] 
						++ msgFunc.valueEnvir).flop;
						if (ids.isNil ) {
							bndl.do { | b |
								id = server.nextNodeID;
								ids = ids.add(id);
								b[2] = id;
							};
						};					
				
						if ((addAction == 0) || (addAction == 3)) {
							bndl = bndl.reverse;
						};
						server.sendBundle(server.latency, *bndl);
						~id = ids;
						~isPlaying = true;	 
						NodeWatcher.register(currentEnvironment);
					},
					
					Group: #{|server|
						var ids, group, addAction, bundle;
						ids = ~id = (~id ?? { server.nextNodeID }).asArray;
						group = ~group;
						addAction = ~addAction;
						~server = server;
						if ((addAction == 0) || (addAction == 3) ) {
							ids = ids.reverse;
						};
						bundle = ids.collect {|id, i|
							[\g_new, id, addAction, group]; 
						};
						server.sendBundle(server.latency, *bundle);
						~isPlaying = true;
						NodeWatcher.register(currentEnvironment);
					},
					
					tree: #{ |server|
						var doTree = { |tree, currentNode, addAction=1|
							if(tree.isKindOf(Association)) {
								~bundle = ~bundle.add(["/g_new", tree.key, addAction, currentNode]);
								currentNode = tree.key;
								tree = tree.value;
							};
							if(tree.isSequenceableCollection) {
								tree.do { |x, i|
									x ?? { tree[i] = x = server.nextNodeID };
									doTree.(x, currentNode)
								};
							} {
								~bundle = ~bundle.add(["/g_new", tree, addAction, currentNode]);
							};
							
						};
						~bundle = nil;
						~treeGroups = ~treeGroups ?? { ~tree.deepCopy };
						~treeGroups !? { 
							doTree.(~treeGroups, ~group, ~addAction);
							CmdPeriod.doOnce { ~treeGroups = nil };
						};
						~bundle !? {
							server.sendBundle(server.latency, *~bundle);
						};
						~bundle = nil;
					}

					
				)
			)
		);
		
		parentEvents = (
			default: ().putAll(
				partialEvents.pitchEvent, 
				partialEvents.ampEvent, 
				partialEvents.durEvent, 
				partialEvents.bufferEvent,
				partialEvents.serverEvent,
				partialEvents.playerEvent,
				partialEvents.midiEvent
			),
			groupEvent:	(
				play: #{
					var server, group, addAction, ids, bundle;
					ids = Event.checkIDs(~id);
					group = ~group;
					addAction = ~addAction;
					~server = server= ~server ?? {Server.default};
					if ((addAction == 0) || (addAction == 3) ) {
						ids = ids.reverse;
					};
					bundle = ids.collect {|id, i|
						[\g_new, id, addAction, group]; 
					};
					server.sendBundle(server.latency, *bundle);
					~isPlaying = true;
					~isRunning = true;
					NodeWatcher.register(currentEnvironment);
				}
			).putAll(partialEvents.nodeEvent),
			synthEvent:	(
				play: #{ 
					var server, latency, group, addAction;
					var instrumentName, synthLib, desc, msgFunc;
					var bndl, ids, id;
					~server = server = ~server ?? { Server.default };
					group = ~group;
					addAction = ~addAction;
					synthLib = ~synthLib ?? { SynthDescLib.global };
					instrumentName = ~instrument.asSymbol;
					desc = synthLib.synthDescs[instrumentName];					if (desc.notNil) { 
						msgFunc = desc.msgFunc;
						~hasGate = desc.hasGate;
					}{
						msgFunc = ~defaultMsgFunc;
					};
				
					bndl = ( [\s_new, instrumentName, ids, addAction, group]
						 ++ msgFunc.valueEnvir).flop;
					bndl.postln;
					ids = Event.checkIDs(~id);
					if (ids.isNil ) {
						bndl.do { | b |
							id = server.nextNodeID;
							ids = ids.add(id);
							b[2] = id;
						};
					};					
			
					if ((addAction == 0) || (addAction == 3)) {
						bndl = bndl.reverse;
					};
					server.sendBundle(server.latency, *bndl);
					~id = ids;
					~isPlaying = true;	 
					~isRunning = true;	 
					NodeWatcher.register(currentEnvironment);
				},
				defaultMsgFunc: #{|freq = 440, amp = 0.1, pan = 0, out = 0| 
					[\freq, freq, \amp, amp, \pan, pan, \out, out] }
			).putAll(partialEvents.nodeEvent)
			

		);
		
		defaultParentEvent = parentEvents.default;
	}
}

