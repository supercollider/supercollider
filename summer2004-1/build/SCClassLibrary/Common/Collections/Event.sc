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
		^parentEvents.silentEvent.copy.put(\dur, dur)
	}
	
	next { ^this.copy }
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
	
	*initClass {
		Class.initClassTree(Server);
		Class.initClassTree(TempoClock);
		this.makeParentEvents;
				
		SynthDef(\default, { arg out=0, freq=440, amp=0.1, pan=0, gate=1;
			var z;
			z = LPF.ar(
					Mix.new(VarSaw.ar(freq + [0, Rand(-0.4,0.0), Rand(0.0,0.4)], 0, 0.3)),
					XLine.kr(Rand(4000,5000), Rand(2500,3200), 1)
				) * Linen.kr(gate, 0.01, amp * 0.7, 0.3, 2);
			Out.ar(out, Pan2.ar(z, pan));
		}, [\ir]).writeDefFile;
		

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
				pan: 0.0 			// pan center
			),
			
			serverEvent: (
				instrument: \default,
				
				group: 0,
				out: 0,
				addAction: 0,
										
				synthLib: SynthDescLib.global,
	
				server: #{ Server.default }
			)
		);
		
		// parents
		parentEvents = (
			
			silentEvent: ( play: nil ).putAll( partialEvents.durEvent ),
			
			tempoEvent: (
				play: #{
					var tempo;
					tempo = ~tempo;
					if (tempo.notNil) {
						thisThread.clock.tempo = tempo;
					};
				}
			).putAll(
				partialEvents.durEvent
			),
			
			noteEvent: (
				play: #{
					var freqs, lag, dur, strum, sustain, desc;
					var bndl, server, addAction, group, tempo;
					
					freqs = ~freq = ~freq.value + ~detune;
					
					tempo = ~tempo;
					if (tempo.notNil) {
						thisThread.clock.tempo = tempo;
					};
									
					if (freqs.isKindOf(Symbol).not) {
						~finish.value;
						~amp = ~amp.value;
						server = ~server = ~server.value;
						addAction = ~addAction;
						group = ~group;
						lag = ~lag + server.latency;
						strum = ~strum;
						sustain = ~sustain = ~sustain.value;
			
						desc = ~synthLib.synthDescs[~instrument.asSymbol];
						if (desc.isNil) { 
							error("instrument " ++ ~instrument ++ " not found."); 
							~delta = ~dur = nil; // force stop
						}{
							bndl = desc.msgFunc.valueEnvir.flop;
							bndl.do {|msgArgs, i|
								var id, latency;
								
								latency = i * strum + lag;
								id = server.nextNodeID;
								
								//send the note on bundle
								server.sendBundle(lag, [9, desc.name, id, addAction, group] ++ msgArgs); 
										
								if (desc.hasGate) {
									// send note off bundle.
									thisThread.clock.sched(sustain) { 
										server.sendBundle(lag, [15, id, \gate, 0]); //15 == n_set
									};
								};
							}
						}
					};
				}
			).putAll(
				partialEvents.pitchEvent, 
				partialEvents.ampEvent, 
				partialEvents.durEvent, 
				partialEvents.serverEvent
			),
			
			
			nodeSetEvent: (
				play: #{
					var freqs, lag, dur, strum, sustain, desc, bndl, server, addAction, group, tempo;
					freqs = ~freq.value + ~detune;
					
					tempo = ~tempo;
					if (tempo.notNil) {
						thisThread.clock.tempo = tempo;
					};
									
					if (freqs.isKindOf(Symbol).not) {
						server = ~server.value;
						~amp = ~amp.value;
						addAction = ~addAction;
						group = ~group;
						lag = ~lag + server.latency;
						strum = ~strum;
			
						desc = ~synthLib.synthDescs[~instrument.asSymbol];
						if (desc.isNil) { 
							error("instrument " ++ ~instrument ++ " not found."); 
							~delta = ~dur = nil; // force stop
						}{
							bndl = ([15, ~id] ++ desc.msgFunc.valueEnvir).flop;
							bndl.do {|msgArgs, i|
								var latency;
								
								latency = i * strum + lag;
								
								server.sendBundle(lag, msgArgs); 
							};
						};
					};
				}
			).putAll(
				partialEvents.pitchEvent, 
				partialEvents.ampEvent, 
				partialEvents.durEvent, 
				partialEvents.serverEvent
			),
			
			controlBusEvent: (
				play: #{
					var server, lag, array;
					server = ~server.value;
					lag = ~lag + server.latency;
					array = ~array;
					server.sendBundle(lag, [\c_setn, ~out, array.size] ++ array);
				}
			).putAll(
				partialEvents.durEvent, 
				partialEvents.serverEvent
			)
		);
		
		defaultParentEvent = parentEvents.noteEvent;
	}
}

