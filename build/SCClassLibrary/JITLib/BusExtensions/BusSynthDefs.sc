BusSynthDefs {

*initClass {


		//////// control rate defs ///////////


		SynthDef.writeOnce("system_gate_control", { arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
			var env;
			env = EnvGen.kr(Env([0, 1, 1, 0],[0, 1, 0]), 1.0, i_level-i_start, i_start, i_dur, doneAction:2);
			ReplaceOut.kr(i_bus, env)
		});

		SynthDef.writeOnce("system_line_control", { arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
			ReplaceOut.kr(i_bus, Line.kr(i_start, i_level, i_dur, doneAction:2))
		});

		SynthDef.writeOnce("system_xline_control", { arg i_bus=0, i_start=0.001, i_level=1.0, i_dur=1.0;
			ReplaceOut.kr(i_bus, XLine.kr(i_start, i_level, i_dur, doneAction:2))
		});
		
		//to do: fade in and out.
		//two types, n > 4 and n <= 4
		SynthDef("system_env_0_control", { arg i_bus=0; // i_fadeTime=0.3;
			var env, envctl, in;
			//in = In.kr(i_bus,1);
			env = Env.newClear(4);
			envctl = Control.names([\env]).kr( env.asArray );
			//envctl = envctl.addAll([in, i_fadeTime, 1, 0]);
			Out.kr(i_bus, EnvGen.kr(envctl, doneAction: 2));
		}).writeDefFile;
		
		SynthDef("system_env_1_control", { arg i_bus=0; //, i_fadeTime=0.3;
			var env, envctl, in;
			//in = In.kr(i_bus,1);
			env = Env.newClear(8);
			envctl = Control.names([\env]).kr( env.asArray );
			//envctl = envctl.addAll([in, i_fadeTime, 1, 0]);
			Out.kr(i_bus, EnvGen.kr(envctl, doneAction: 2));
		}).writeDefFile;
		
		//////// audio rate defs ///////////
		
		SynthDef.writeOnce("system_gate_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			var env, start;
			start = InFeedback.ar(i_bus);
			env = EnvGen.ar(Env([0, 1, 1, 0],[0, 1, 0]), 1.0, i_level-start, start, i_dur, doneAction:2);
			ReplaceOut.ar(i_bus, env)
		});

		SynthDef.writeOnce("system_line_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			ReplaceOut.ar(i_bus, Line.ar(InFeedback.ar(i_bus), i_level, i_dur, doneAction:2))
		});

		SynthDef.writeOnce("system_xline_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			ReplaceOut.ar(i_bus, XLine.ar(InFeedback.ar(i_bus).max(0.01), i_level, i_dur, doneAction:2))
		});
		
		
		SynthDef.writeOnce("system_env_1_audio", { arg i_bus=0;
			var env, envctl;
			env = Env.newClear(8);
			envctl = Control.names([\env]).kr( env.asArray );
				Out.ar(i_bus, EnvGen.ar(envctl, doneAction: 2));
		});
		SynthDef.writeOnce("system_env_0_audio", { arg i_bus=0;
			var env, envctl;
			env = Env.newClear(4);
			envctl = Control.names([\env]).kr( env.asArray );
				Out.ar(i_bus, EnvGen.ar(envctl, doneAction: 2));
		});
		
		
	}

}