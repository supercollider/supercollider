SystemSynthDefs {

*initClass {
	var defs;
	defs = [
		SynthDef("system_gate_control", { arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
			var env;
			env = EnvGen.kr(Env([0, 1, 1, 0],[0, 1, 0]), 1.0, i_level-i_start, i_start, i_dur, doneAction:2);
			ReplaceOut.kr(i_bus, env)
		}),

		SynthDef("system_line_control", { arg i_bus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
			ReplaceOut.kr(i_bus, Line.kr(i_start, i_level, i_dur, doneAction:2))
		}),

		SynthDef("system_xline_control", { arg i_bus=0, i_start=0.001, i_level=1.0, i_dur=1.0;
			ReplaceOut.kr(i_bus, XLine.kr(i_start, i_level, i_dur, doneAction:2))
		}),
		
		SynthDef("system_gate_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			var env, start;
			start = InFeedback.ar(i_bus);
			env = EnvGen.ar(Env([0, 1, 1, 0],[0, 1, 0]), 1.0, i_level-start, start, i_dur, doneAction:2);
			ReplaceOut.ar(i_bus, env)
		}),

		SynthDef("system_line_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			ReplaceOut.ar(i_bus, Line.ar(InFeedback.ar(i_bus), i_level, i_dur, doneAction:2))
		}),

		SynthDef("system_xline_audio", { arg i_bus=0,  i_level=1.0, i_dur=1.0;
			ReplaceOut.ar(i_bus, XLine.ar(InFeedback.ar(i_bus).max(0.01), i_level, i_dur, doneAction:2))
		})
	];
	
	defs.do({ arg item; item.writeDefFile });
}

}