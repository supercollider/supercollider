// synthdefs needed by classes

SystemSynthDefs {
	classvar <>numChannels = 16;
	classvar <>tempNamePrefix = "temp__";
	classvar tempDefCount = 0;
	classvar <>maxTempDefNames = 512;

	*generateTempName {
		var name = tempNamePrefix ++ tempDefCount;
		tempDefCount = tempDefCount + 1 % maxTempDefNames;
		^name
	}

	*initClass {

		StartUp.add {

			// clean up any written synthdefs starting with "temp__"
			var path = SynthDef.synthDefDir ++ tempNamePrefix ++ "*";
			var match = pathMatch(path);
			if(match.notEmpty) {
				"Cleaning up % temporary SynthDef%...\n".postf(
					match.size,
					if(match.size == 1, { "" }, { "s" })
				);
				match.do { |file| File.delete(file) };
			};

			// add system synth defs
			(1..numChannels).do { arg i;
				SynthDef("system_link_audio_" ++ i,
					{ arg out=0, in=16, vol=1, level=1, lag=0.05, doneAction=2;
						var env = EnvGate(doneAction:doneAction, curve:'sin') * Lag.kr(vol * level, lag);
						Out.ar(out, InFeedback.ar(in, i) * env)
					}, [\kr, \kr, \kr, \kr, \kr, \ir]).add;

				SynthDef("system_link_control_" ++ i,
					{ arg out=0, in=16, doneAction=2;
						var env = EnvGate(doneAction:doneAction, curve:'lin');
						Out.kr(out, In.kr(in, i) * env)
					}, [\kr, \kr, \ir]).add;

				SynthDef("system_diskout_" ++ i, { arg i_in, i_bufNum=0;
					DiskOut.ar(i_bufNum, InFeedback.ar(i_in, i));
				}).add;

				SynthDef("system_setbus_hold_audio_" ++ i, { arg out = 0, fadeTime = 0, curve = 0, gate = 1;
					var values = NamedControl.ir(\values, 0 ! i);
					var env = Env([In.ar(out, i), values, values], [1, 0], curve, 1);
					var sig = EnvGen.ar(env, gate + Impulse.kr(0), timeScale: fadeTime, doneAction: 2);
					ReplaceOut.ar(out, sig);
				}, [\ir, \kr, \ir, \kr]).add;

				SynthDef("system_setbus_audio_" ++ i, { arg out = 0, fadeTime = 0, curve = 0, gate = 1;
					var values = NamedControl.ir(\values, 0 ! i);
					var env = Env([-1, 1, 1, -1], [1, 0, 1], curve, 1);
					var envgen = EnvGen.kr(env, gate + Impulse.kr(0), timeScale: fadeTime, doneAction: 2);
					var sig = LinXFade2.ar(In.ar(out, i), DC.ar(values), envgen);
					ReplaceOut.ar(out, sig);
				}, [\ir, \kr, \ir, \kr]).add;

				SynthDef("system_setbus_control_" ++ i, { arg out = 0, fadeTime = 0, curve = 0;
					var values = NamedControl.ir(\values, 0 ! i);
					var env = Env([In.kr(out, i), values], [1], curve);
					var sig = EnvGen.kr(env, timeScale: fadeTime, doneAction: 2);
					ReplaceOut.kr(out, sig);
				}, [\ir, \kr, \ir]).add;
			};
		};
	}
}
