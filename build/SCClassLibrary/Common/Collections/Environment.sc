
Environment : IdentityDictionary {
	classvar <>stack;
	
	
	*make { arg function;
		^this.new.make(function)
	}
	*use { arg function;
		^this.new.use(function)
	}

	make { arg function;
		// pushes the Envir, executes function, returns the Envir
		// usually used to create an environment by adding new variables to it.
		var result, saveEnvir;
		
		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		function.value(this);
		currentEnvironment = saveEnvir;
	}
	use { arg function;
		// temporarily replaces the currentEnvironment with this, 
		// executes function, returns the result of the function
		var result, saveEnvir;
		
		saveEnvir = currentEnvironment;
		currentEnvironment = this;
		result = function.value(this);
		currentEnvironment = saveEnvir;
		^result
	}
	
	eventAt { arg key; ^this.at(key) }
	
	*pop {
		if(stack.notEmpty) { currentEnvironment = stack.pop.debug };
		\pop.debug;
	}
	
	*push { arg envir;
		stack = stack.add(currentEnvironment);
		currentEnvironment = envir;
	}
	
	pop { ^this.class.pop }
	push { this.class.push(this) }
	
}

// Events are returned by Pattern Streams

Event : Environment {
	classvar default;
	
	*default {
		^Event.new(8, default);
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
		this.at(\player).playEvent(this);
		^this.delta
	}
	
	*silent { arg time;
		var res;
		res = this.default;
		res.use({ ~freq = \rest; ~delta = time });
		^res
	}
	
	*initClass {
		Class.initClassTree(Server);
		Class.initClassTree(TempoClock);
		
		default = this.make({		
			// fill prototype event with default property values
			
			~reverse = false;
			
			~tempoclock = TempoClock.default;
			~tempo = nil;
			
			~dur = 1.0;
			~stretch = 1.0;
			~legato = 0.8;
			~sustain = #{ ~dur * ~legato * ~stretch };
			~lag = 0.0;
			~strum = 0.0;
			
			~amp = #{ ~db.dbamp };
			~db = -20.0;
			~velocity = 64; 		// MIDI units 0-127
			~pan = 0.0; 			// pan center
			
			~mtranspose = 0;
			~gtranspose = 0.0;
			~ctranspose = 0.0;
			
			~octave = 5.0;
			~root = 0.0;		// root of the scale
			~degree = 0;
			~scale = #[0, 2, 4, 5, 7, 9, 11]; // diatonic major scale
			~stepsPerOctave = 12.0;
			~detune   = 0.0;		// detune in Hertz
			
			~note = #{
				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
			};
			~midinote = #{
				var divs;	
				divs = ~stepsPerOctave;
				(~note.value + ~gtranspose + (~octave * divs) + ~root)
					 * 12.0 / divs; 
			};
			~freq = #{
				(~midinote.value + ~ctranspose).midicps;
			};

			~instrument = \default;
			
			~group = 0;
			~out = 0;
			~addAction = 0;
			
			~finish = #{
				// do final calculations
				~freq = ~freq.value + ~detune;
				~amp = ~amp.value;
				~sustain = ~sustain.value;
			};
									
			~player = NotePlayer.new;
			~synthLib = SynthDescLib.global;

			~server = Server.default;
		});
		
		SynthDef(\default, { arg out=0, freq=440, amp=0.1, pan=0, gate=1;
			var z;
			z = LPF.ar(
				Mix.new(VarSaw.ar(freq + [0, Rand(-0.4,0.0), Rand(0.0,0.4)], 0, 0.3)),
				XLine.kr(Rand(4000,5000), Rand(2500,3200), 1)) * Linen.kr(gate, 0.01, amp * 0.7, 0.3, 2);
			Out.ar(out, Pan2.ar(z, pan));
		}, [\ir]).writeDefFile;

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
}

