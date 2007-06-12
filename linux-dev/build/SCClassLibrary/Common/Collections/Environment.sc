
Environment : IdentityDictionary {

	*new { arg size=8; ^super.new(size) }
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
}

// Events are returned by Pattern Streams

Event : Environment {
	classvar <defaultProtoEvent;
	
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
		^this.delta / this.at(\tempo)
	}
	
	*protoEvent { ^defaultProtoEvent.copy }
	*initClass {		
		defaultProtoEvent = this.make({		
			// fill prototype event with default property values
			
			~reverse = false;
			
			~tempo = 1;	// beats per second
			~dur = 1.0;
			~stretch = 1.0;
			~legato = 0.8;
			~sustain = { ~dur * ~legato * ~stretch };
			~lag = 0.0;
			~strum = 0.0;
			~fxIndex = 0;
			~monoIndex = 0;
			
			~amp = { ~db.dbamp };
			~db = -20.0;
			~velocity = 64; 		// MIDI units 0-127
			~pan = 0.0; 			// pan center
			~channels = 2;
			
			~mtranspose = 0;
			~gtranspose = 0.0;
			~ctranspose = 0.0;
			
			~octave = 5.0;
			~root = 0.0;		// root of the scale
			~degree = 0;
			~scale = #[0, 2, 4, 5, 7, 9, 11]; // diatonic major scale
			~stepsPerOctave = 12.0;
			~detune   = 0.0;		// detune in Hertz
			
			~note = {
				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);
			};
			~midinote = {
				var divs;	
				divs = ~stepsPerOctave;
				(~note.value + ~gtranspose + (~octave * divs) + ~root)
					 * 12.0 / divs; 
			};
			~freq = {
				(~midinote.value + ~ctranspose).midicps;
			};
			
			~env = Env.asr(0.01, 1.0, 0.5);
			~envgen = { EnvGen.kr(~env, 1, ~amp) };

			~chanOffset = 0;
			~instrument = \default;
			~wavetable = Wavetable.sineFill(1024, 1/[1,2,3,4,5,6]);
			~ugenFunc = { 
				Library.at(\instruments, ~instrument).ugenFunc.valueEnvir; 
			};
			
			~argNames = #[\freq, \amp, \pan];
			//~group = 0;
			
			~finish = {
				// do final calculations
				~freq = ~freq.value + ~detune;
				~amp = ~amp.value;
				~sustain = ~sustain.value;
			};
						
			~player = NotePlayer.new;
			~latency = 0.05;
		});
	}
}


