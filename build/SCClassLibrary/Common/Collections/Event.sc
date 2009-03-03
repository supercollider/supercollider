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
	*addEventType { arg type, func;
		var types = partialEvents.playerEvent.eventTypes;
		types.put(type, func)
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
//		^this.delta
	}
	
	// node watcher interface
	
	isPlaying_ { arg val;
		this.put(\isPlaying, val);
	}
	
	isRunning_ { arg val;
		this.put(\isRunning, val);
	}
	
	// this method is called by EventStreamPlayer so it can schedule Routines as well
	playAndDelta { | cleanup, mute |
		if (mute) { this.put(\freq, \rest) };
		cleanup.update(this);
		this.play;
		^this.delta;
	}

	// A Quant specifies the quant and phase at which a TempoClock starts an EventStreamPlayer
	// Its offset specifies how far ahead of time events are actually computed by the EventStream.
	// offset allows ~strum to be negative, so strummed chords complete on the beat
	// it also makes it possible for one pattern to run a little ahead of another to set values
	// This method keeps ~timingOffset and Quant.offset the same.
	
	synchWithQuant { | quant |
		if(quant.timingOffset.notNil) {
			^this.copy.put(\timingOffset, quant.timingOffset)
		} {
			quant.timingOffset = this[\timingOffset];
			^this
		};
	}

	// This enables events to represent the server resources they created in an Event
	// So, ~bufnum = (type: \sine1, amps: 1/(1..10)) is possible
	asControlInput {		
		^this[ EventTypesWithCleanup.ugenInputTypes[this[\type] ] ] ;
	}
	asUGenInput { ^this.asControlInput }

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
			Event.makeDefaultSynthDef;
		};
		

	}
	
	*makeDefaultSynthDef {
		SynthDef(\default, { arg out=0, freq=440, amp=0.1, pan=0, gate=1;
				var z;
				z = LPF.ar(
						Mix.new(VarSaw.ar(freq + [0, Rand(-0.4,0.0), Rand(0.0,0.4)], 0, 0.3)),
						XLine.kr(Rand(4000,5000), Rand(2500,3200), 1)
					) * Linen.kr(gate, 0.01, 0.7, 0.3, 2);
				OffsetOut.ar(out, Pan2.ar(z, pan, amp));
			}, [\ir]).storeOnce;
	}
	
	*makeParentEvents {		
		// define useful event subsets.
		partialEvents = (
			pitchEvent: (
				mtranspose: 0,
				gtranspose: 0.0,
				ctranspose: 0.0,
				
				octave: 5.0,
				root: 0.0,					// root of the scale
				degree: 0,
				scale: #[0, 2, 4, 5, 7, 9, 11], 	// diatonic major scale
				stepsPerOctave: 12.0,
				detune: 0.0,					// detune in Hertz
				harmonic: 1.0,				// harmonic ratio
				octaveRatio: 2.0,
				
				note: #{
					(~degree + ~mtranspose).degreeToKey(
						~scale, 
						~scale.respondsTo(\stepsPerOctave).if(
							{ ~scale.stepsPerOctave },
							~stepsPerOctave
						)
					);
				},
				midinote: #{
					(((~note.value + ~gtranspose + ~root) / 
						~scale.respondsTo(\stepsPerOctave).if(
							{ ~scale.stepsPerOctave },							~stepsPerOctave) + ~octave - 5.0) * 						(12.0 * ~scale.respondsTo(\octaveRatio).if						({ ~scale.octaveRatio }, ~octaveRatio).log2) + 60.0);				},
				detunedFreq: #{
					~freq.value + ~detune
				},
				freq: #{
					(~midinote.value + ~ctranspose).midicps * ~harmonic;
				},
				freqToNote: #{ arg self, freq; // conversion from frequency to note value
					self.use {
						var midinote;
						var steps = ~scale.respondsTo(\stepsPerOctave).if(
							{ ~scale.stepsPerOctave }, ~stepsPerOctave
						);
						midinote = cpsmidi((freq / ~harmonic) - ~ctranspose);
						midinote / 12.0 - ~octave * steps - ~root - ~gtranspose
					}
				},
				freqToScale: #{ arg self, freq; 
					// conversion from frequency to scale value. 
					self.use {
						var steps = ~scale.respondsTo(\stepsPerOctave).if(
							{ ~scale.stepsPerOctave }, ~stepsPerOctave
						);
						var degree = self.freqToNote(freq).keyToDegree(~scale, steps) - ~mtranspose;
						degree.asArray.collect {|x, i|
							x = x.round(0.01);
							if(x.floor != x) { 
								"could not translate: %\n".postf(freq[i]); 
								nil 
							} { x }
						}.unbubble;
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
				strum: 0.0,
				strumEndsTogether: false
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
				
				group: 1,
				out: 0,
				addAction: 0,
				
				instrument: \default,
				variant: nil,
					// this function should return a msgFunc
					// (i.e. a Function that assembles a synth control list from event values)
				getMsgFunc: { |instrument|
					var	synthLib, desc;
						// if user specifies a msgFunc, prefer user's choice
					if(~msgFunc.isNil) {
						instrument = ~instrument = instrument.asSymbol;
						synthLib = ~synthLib ?? { SynthDescLib.global };
						~synthDesc = desc = synthLib.at(instrument);
						if (desc.notNil) { 
							~hasGate = desc.hasGate;
							~msgFunc = desc.msgFunc;
						}{
							~msgFunc = ~defaultMsgFunc;
						};
					} { ~msgFunc };
				},	
				synthDefName: { |instrument, variant, synthDesc|
						// allow `nil to cancel a variant in a pattern
					variant = variant.dereference;
					if(variant.notNil and: { synthDesc.notNil and: { synthDesc.hasVariants } })
						{ (instrument ++ "." ++ variant).asSymbol }
						{ instrument.asSymbol };
				},
				hasGate: true,		// assume SynthDef has gate
				sendGate: nil,		// false => event does not specify note release
				
				defaultMsgFunc: #{|freq = 440, amp = 0.1, pan = 0, out = 0| 
					[\freq, freq, \amp, amp, \pan, pan, \out, out] },
				
				// for \type \set
				args: #[\freq, \amp, \pan, \trig],
				
				timingOffset: 0 ,
	//			
//				schedBundle: #{ |lag, offset, server ...bundle |
//					thisThread.clock.sched ( offset, {
//							// the test is to prevent piles of "late" messages
//							// if you're using nil server latency
//						if(server.latency.isNil and: { lag == 0 or: { lag.isNil } }) {
//							server.sendBundle(nil, *bundle)
//						} {
//							server.sendBundle((server.latency ? 0) + (lag ? 0), *bundle);
//						}
//					})
//				},
//
//				schedBundleArray: #{ | lag, offset, server, bundleArray |
//					thisThread.clock.sched ( offset, {
//						if(server.latency.isNil and: { lag == 0 or: { lag.isNil } }) {
//							server.sendBundle(nil, *bundleArray)
//						} {
//							server.sendBundle((server.latency ? 0) + (lag ? 0), *bundleArray);
//						}
//					})
//				},
//								
				schedBundle: #{ |lag, offset, server ...bundle |
				thisThread.clock.sched ( offset, {
					if (lag !=0 ) {
						SystemClock.sched( lag, { server.sendBundle(server.latency, *bundle) })
					} {
						server.sendBundle(server.latency, *bundle)
					}
				})
			},

			schedBundleArray: #{ | lag, offset, server, bundleArray |
				thisThread.clock.sched ( offset, {
					if (lag !=0 ) {
						SystemClock.sched(lag, { server.sendBundle(server.latency, *bundleArray) })
					} {
						server.sendBundle(server.latency, *bundleArray)
					}
				})
			},

				schedStrummedNote: {| lag, strumTime, sustain, server, msg, sendGate |
					var dur, schedBundle = ~schedBundle;
					schedBundle.value(lag, strumTime + ~timingOffset, server, msg);
					if(sendGate) { 
						if (~strumEndsTogether) { 
							dur = sustain ;
						} { 
							dur = sustain + strumTime 
						};
						schedBundle.value(lag, dur + ~timingOffset, server, [\n_set, msg[2], \gate, 0])
					}
				}				
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
							[chan, midinote, asInteger((amp * 127).clip(0, 127)) ] },
					noteOff: #{ arg chan=0, midinote=60, amp=0.1;
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
					rest: #{},
			
					note: #{|server|
						var freqs, lag, strum, strumTime, sustain;
						var bndl, addAction, group, sendGate, ids;
						var msgFunc, desc, synthLib, bundle, instrumentName, schedBundleArray, offset;
						
						freqs = ~detunedFreq.value;
										
						if (freqs.isKindOf(Symbol).not) {
							// determine msgFunc - it gets the synth's control values from the Event
							msgFunc = ~getMsgFunc.valueEnvir;
							instrumentName = ~synthDefName.valueEnvir;
						
							// now update values in the Event that may be determined by functions
							~freq = freqs;
							~amp = ~amp.value;
							~sustain = sustain = ~sustain.value;
			
							// compute the control values and generate OSC commands
//							bndl = msgFunc.valueEnvir.asControlInput;							bndl = [\s_new, instrumentName, ids, Node.actionNumberFor(~addAction), ~group.asControlInput] ++ bndl; 
//							bndl = bndl.flop;
//							ids = Array.fill(bndl.size, {server.nextNodeID });
//							bndl.do { | msg, i | msg[2] = ids[i]  };


							bndl = msgFunc.valueEnvir;
							bndl = [\s_new, instrumentName, ids, Node.actionNumberFor(~addAction), ~group] ++ bndl; 
							bndl = bndl.flop;
							ids = Array.fill(bndl.size, {server.nextNodeID });
							bndl = bndl.collect { | msg, i | msg[2] = ids[i]; msg.asOSCArgArray  };
							
							// determine how to send those commands
							lag = ~lag;
							offset = ~timingOffset;
							sendGate = ~sendGate ? ~hasGate;         // sendGate == false turns off releases
							schedBundleArray = ~schedBundleArray;
							if (	 (strum = ~strum) == 0 ) {
								schedBundleArray.value(lag, offset, server, bndl);
								if (sendGate) { 
									~schedBundleArray.value(lag, sustain + offset, server, [\n_set, ids, \gate, 0].flop)
								}
							} {	
								if (strum < 0) { bndl = bndl.reverse };
								strumTime = 0;
								bndl.do { | msg, i | 
									~schedStrummedNote.value( lag, strumTime, sustain, server, msg, sendGate); 
									strumTime = strumTime + strum;
								}
							}
						}
					},
			
					on: #{|server|
						var freqs;
						var bndl, sendGate, ids;
						var msgFunc, desc, synthLib, bundle, instrumentName;
						
						freqs = ~detunedFreq.value;
									
						if (freqs.isKindOf(Symbol).not) {
							~freq = freqs;
							~amp = ~amp.value;
							msgFunc = ~getMsgFunc.valueEnvir;
							instrumentName = ~synthDefName.valueEnvir;
//							bndl = msgFunc.valueEnvir.asControlInput;
//							bndl = [\s_new, instrumentName, ~id, Node.actionNumberFor(~addAction), ~group.asControlInput] ++ bndl; 
//							bndl = bndl.flop;
//							if ( (ids = ~id).isNil ) {
//								ids = Array.fill(bndl.size, {server.nextNodeID });
//								bndl.do { | msg, i | msg[2] = ids[i]  };
//							};
//							~schedBundleArray.value(~lag, ~timingOffset, server, bndl);
							bndl = msgFunc.valueEnvir;
							bndl = [\s_new, instrumentName, ~id, Node.actionNumberFor(~addAction), ~group] ++ bndl; 
							bndl = bndl.flop;
							if ( (ids = ~id).isNil ) {
								ids = Array.fill(bndl.size, {server.nextNodeID });
								bndl = bndl.collect { | msg, i | msg[2] = ids[i]; msg.asOSCArgArray  };
							} {
								bndl = bndl.asOSCArgBundle;
							
							};
							~schedBundleArray.value(~lag, ~timingOffset, server, bndl);
						};
						
						~server = server;
						~id = ids;
						~callback.value(currentEnvironment)
					},
					
					set: #{|server|
						var freqs, lag, dur, strum, bndl, msgFunc;
						freqs = ~freq = ~detunedFreq.value;
										
						if (freqs.isKindOf(Symbol).not) {
							~server = server;
							freqs = ~freq;
							~amp = ~amp.value;
							if ( (msgFunc = ~getMsgFunc.valueEnvir).notNil) {
								bndl = msgFunc.valueEnvir;
							} {	
								bndl = ~args.envirPairs;
							};
//							bndl = ([\n_set, ~id.asControlInput] ++  bndl).asControlInput.flop;

							bndl = ([\n_set, ~id] ++  bndl).flop.asOSCArgBundle;
							~schedBundleArray.value(~lag, ~timingOffset, server, bndl);
						};
					},
			
					off: #{|server|
						var gate;
						if (~hasGate) { 
							gate = min(0.0, ~gate ? 0.0); // accept release times
							~schedBundleArray.value(~lag, ~timingOffset, server,[\n_set, ~id.asControlInput, \gate, gate].flop) 
						} {
							~schedBundleArray.value(~lag, ~timingOffset, server, [\n_free, ~id.asControlInput].flop)
						}						
					},
					
					kill: #{|server|
						~schedBundleArray.value(~lag, ~timingOffset, server, [\n_free, ~id.asControlInput].flop)
					},
			
					group: #{|server|
						var bundle;
						if (~id.isNil) { ~id = server.nextNodeID };
						bundle = [\g_new, ~id.asArray, Node.actionNumberFor(~addAction), ~group.asControlInput].flop;
						~schedBundleArray.value(~lag, ~timingOffset, server, bundle);
					},
			
			
					bus: #{|server|
						var array;
						array = ~array.asArray;
						~schedBundle.value(~lag, ~timingOffset, server, [\c_setn, ~out.asControlInput, array.size] ++ array);
					},
					
					gen: #{|server|
						~schedBundle.value(~lag, ~timingOffset, server, [\b_gen, ~bufnum.asControlInput, ~gencmd, ~genflags] ++ ~genarray);
					},
					
					load: #{|server|
						~schedBundle.value(~lag, ~timingOffset, server, [\b_allocRead, ~bufnum.asControlInput, ~filename, ~frame, ~numframes]);
					},
					read: #{|server|
						~schedBundle.value(~lag, ~timingOffset, server, [\b_read, ~bufnum.asControlInput, ~filename, ~frame, ~numframes, ~bufpos, ~leaveOpen]);
					},
					alloc: #{|server|
						~schedBundle.value(~lag, ~timingOffset, server, [\b_alloc, ~bufnum.asControlInput, ~numframes, ~numchannels]);
					},
					free: #{|server|
						~schedBundle.value(~lag, ~timingOffset, server, [\b_free, ~bufnum.asControlInput]);
					},
					
					midi: #{|server|
						var freqs, lag, dur, sustain, strum;
						var bndl, midiout, hasGate, midicmd;
						
						freqs = ~freq = ~detunedFreq.value;
												
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							~midinote = (freqs.cpsmidi).round(1).asInteger;
							strum = ~strum;
							lag = ~lag;
							sustain = ~sustain = ~sustain.value;
							midiout = ~midiout.value;
							~uid ?? { ~uid = midiout.uid };  // mainly for sysex cmd
							hasGate = ~hasGate ? true;
							midicmd = ~midicmd;
							bndl = ~midiEventFunctions[midicmd].valueEnvir.asCollection;
							
							bndl = bndl.asControlInput.flop;
							
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
									if(hasGate and: { midicmd === \noteOn }) {
										thisThread.clock.sched(sustain + latency, { 
											midiout.noteOff(*msgArgs)
										});
									};
							};
						}
					},
					setProperties:  {
						var receiver = ~receiver,
							go = {
								~args.do { |each|	
										var selector, value = each.envirGet;
										if(value.notNil) {
											selector = each.asSetter;
											// ("%.%_(%)\n").postf(receiver, selector, value);
										 	receiver.perform(selector.asSetter, value)
										 };
								}
							};
						if(~defer ? true) {
								// inEnvir is needed
								// because we'll no longer be in this Event
								// when defer wakes up
							go.inEnvir.defer
						} {
							go.value
						};
					},
					monoOff:  #{|server|
			
						if(~hasGate == false) {
							~schedBundle.value(~lag, ~timingOffset, server, [\n_free] ++ ~id.asControlInput);
						} {
							~schedBundle.value(~lag, ~timingOffset, server, *([\n_set, ~id.asControlInput, \gate, 0].flop) ); 
						};
						
					},
					
					monoSet: #{|server|
						var freqs, lag, bndl;
						
						freqs = ~freq = ~detunedFreq.value;
										
						if (freqs.isKindOf(Symbol).not) {
							~amp = ~amp.value;
							~sustain = ~sustain.value;
				
							bndl = ([\n_set, ~id.asControlInput] ++ ~msgFunc.valueEnvir).flop;
							bndl = bndl.collect(_.asOSCArgArray);
							~schedBundle.value(~lag, ~timingOffset, server, *bndl);
						};
					},
			
					monoNote:	#{ |server|	
						var bndl, id, ids, addAction, f;
						addAction = ~addAction;
						~freq = ~detunedFreq.value;
						f = ~freq;
						~amp = ~amp.value;
						
						bndl = ( [\s_new, ~instrument, ids, addAction, ~group.asControlInput] 
							++ ~msgFunc.valueEnvir).flop;
						bndl.do { | b |
							id = server.nextNodeID;
							ids = ids.add(id);
							b[2] = id;
						};
				
						if ((addAction == 0) || (addAction == 3)) {
							bndl = bndl.reverse;
						};
						bndl = bndl.collect(_.asOSCArgArray);
						~schedBundle.value(~lag, ~timingOffset, server, *bndl);
						~updatePmono.value(ids, server);
					},
					
					Synth: #{ |server|	
						var instrumentName, desc, msgFunc;
						var bndl, synthLib, addAction, group, latency, ids, id, groupControls;
						~server = server;
						addAction = Node.actionNumberFor(~addAction);
						group = ~group.asControlInput;
						~freq = ~detunedFreq.value;
						~amp = ~amp.value;
						ids = ~id;
						msgFunc = ~getMsgFunc.valueEnvir;
						instrumentName =~synthDefName.valueEnvir;
					
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
						
						bndl = bndl.collect(_.asOSCArgArray);
						
						server.sendBundle(server.latency, *bndl);
						~id = ids;
						~isPlaying = true;	 
						NodeWatcher.register(currentEnvironment);
					},
					
					Group: #{|server|
						var ids, group, addAction, bundle;
						ids = ~id = (~id ?? { server.nextNodeID }).asArray;
						addAction = Node.actionNumberFor(~addAction);
						group = ~group.asControlInput;
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
								~bundle = ~bundle.add(["/g_new",
									tree.key.asControlInput, Node.actionNumberFor(addAction),
									currentNode.asControlInput]);
								currentNode = tree.key;
								tree = tree.value;
							};
							if(tree.isSequenceableCollection) {
								tree.do { |x, i|
									x ?? { tree[i] = x = server.nextNodeID };
									doTree.(x, currentNode)
								};
							} {
								~bundle = ~bundle.add(["/g_new",
									tree.asControlInput, Node.actionNumberFor(addAction),
									currentNode.asControlInput]);
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
				lag: 0,
				play: #{
					var server, group, addAction, ids, bundle;
					~finish.value;
					group = ~group.asControlInput;
					addAction = Node.actionNumberFor(~addAction);
					~server = server= ~server ?? {Server.default};
					ids = Event.checkIDs(~id);
					if (ids.isNil) { ids = ~id = server.nextNodeID };
					if ((addAction == 0) || (addAction == 3) ) {
						ids = ids.asArray.reverse;
					};
					bundle = ids.collect {|id, i|
						[\g_new, id, addAction, group]; 
					};
					server.sendBundle(server.latency, *bundle);
					~isPlaying = true;
					~isRunning = true;
					NodeWatcher.register(currentEnvironment);
				}).putAll(partialEvents.nodeEvent),
			synthEvent:	(
				lag: 0,
				play: #{
				 
				var server, latency, group, addAction;
				var instrumentName, synthLib, desc, msgFunc;
				var msgs, cvs;
				var bndl, ids;
				~finish.value;
				~server = server = ~server ?? { Server.default };
				group = ~group.asControlInput;
				addAction = Node.actionNumberFor(~addAction);
				synthLib = ~synthLib ?? { SynthDescLib.global };
				instrumentName = ~instrument.asSymbol;
				desc = synthLib.synthDescs[instrumentName];
				if (desc.notNil) { 
					msgFunc = desc.msgFunc;
					~hasGate = desc.hasGate;
				}{
					msgFunc = ~defaultMsgFunc;
				};
			
				msgs = msgFunc.valueEnvir.flop;
				ids = Event.checkIDs(~id);
				if (ids.isNil ) { ids = msgs.collect { server.nextNodeID } };
				bndl = ids.collect { |id, i|
					[\s_new, instrumentName, id, addAction, group]
					 ++ msgs[i]
				};
			
				if ((addAction == 0) || (addAction == 3)) {
					bndl = bndl.reverse;
				};
				bndl = bndl.asOSCArgBundle;
				if (~lag !=0) {
					server.sendBundle(server.latency ? 0 + ~lag, *bndl);
				} {
					server.sendBundle(server.latency, *bndl);
				
				};
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

