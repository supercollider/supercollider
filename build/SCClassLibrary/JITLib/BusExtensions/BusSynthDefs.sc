SystemSynthDefs {

*initClass {
	var defs;
	defs = [
		SynthDef("system_gate_control", { arg ibus=0, i_start=0.0, i_level=1.0, i_dur=1.0;
			var env;
			env = EnvGen.kr(Env([0, 1, 1, 0],[0, 1, 0]), 1.0, i_level-i_start, i_start, i_dur, doneAction:2);
			ReplaceOut.kr(ibus, env)
		}),

		SynthDef("system_line_control", { arg ibus=0, i_start=0.0, i_end=1.0, i_dur=1.0;
			ReplaceOut.kr(ibus, Line.kr(i_start, i_end, i_dur, doneAction:2))
		}),

		SynthDef("system_xline_control", { arg ibus=0, i_start=0.001, i_end=1.0, i_dur=1.0;
			ReplaceOut.kr(ibus, XLine.kr(i_start, i_end, i_dur, doneAction:2))
		}),
		SynthDef("system_triangle_control", { arg ibus=0, i_start=0.0, i_level=1.0, i_dur=1.0;			ReplaceOut.kr(ibus, EnvGen.kr(Env.triangle, 1.0, i_level-i_start, i_start, i_dur, 2))
		})
	];
	
	defs.do({ arg item; item.writeDefFile });
}

}