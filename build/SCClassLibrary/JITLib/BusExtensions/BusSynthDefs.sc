BusSynthDefs {

*initClass {
		StartUp.add {

			//////// control rate defs ///////////
		
		
			SynthDef.writeOnce("system_gate_control", 
				{ arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
				var env;
				env = EnvGen.kr(Env([0, 1, 1, 0],[0, 1, 0]), 
					1.0, i_level-i_start, i_start, i_dur, doneAction:2);
				ReplaceOut.kr(i_bus, env)
			});
	
			SynthDef.writeOnce("system_line_control", 
				{ arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
				ReplaceOut.kr(i_bus, Line.kr(i_start, i_level, i_dur, doneAction:2))
			});
	
			SynthDef.writeOnce("system_xline_control", 
				{ arg i_bus=0, i_start=0.001, i_level=1.0, i_dur=1.0;
				ReplaceOut.kr(i_bus, XLine.kr(i_start, i_level, i_dur, doneAction:2))
			});
			
			//to do: fade in and out.
			//two types, n > 4 and n <= 4
			SynthDef.writeOnce("system_env_0_control", { arg i_bus=0;
				var env, envctl, in;
				env = Env.newClear(4);
				envctl = Control.names([\env]).kr( env.asArray );
				Out.kr(i_bus, EnvGen.kr(envctl, doneAction: 2));
			});
			
			SynthDef.writeOnce("system_env_1_control", { arg i_bus=0;
				var env, envctl, in;
				env = Env.newClear(8);
				envctl = Control.names([\env]).kr( env.asArray );
				Out.kr(i_bus, EnvGen.kr(envctl, doneAction: 2));
			});
			
			//////// audio rate defs ///////////
			
			SynthDef.writeOnce("system_gate_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
				var env, start;
				start = InFeedback.ar(i_bus);
				env = EnvGen.ar(Env([0, 1, 1, 0],[0, 1, 0]), 
					K2A.ar(1.0), i_level-start, start, i_dur, doneAction:2);
				ReplaceOut.ar(i_bus, env)
			});
	
			SynthDef.writeOnce("system_line_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
				ReplaceOut.ar(i_bus, Line.ar(InFeedback.ar(i_bus), i_level, i_dur, doneAction:2))
			});
	
			SynthDef.writeOnce("system_xline_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
				ReplaceOut.ar(i_bus, 
					XLine.ar(InFeedback.ar(i_bus).max(0.01), i_level, i_dur, doneAction:2)
				)
			});
			
			
			SynthDef.writeOnce("system_env_1_audio", { arg i_bus=0;
				var env, envctl;
				env = Env.newClear(8);
				envctl = Control.names([\env]).kr( env.asArray );
					ReplaceOut.ar(i_bus, EnvGen.ar(envctl, doneAction: 2));
			});
			SynthDef.writeOnce("system_env_0_audio", { arg i_bus=0;
				var env, envctl;
				env = Env.newClear(4);
				envctl = Control.names([\env]).kr( env.asArray );
					ReplaceOut.ar(i_bus, EnvGen.ar(envctl, doneAction: 2));
			});
	
	}
	}

}
