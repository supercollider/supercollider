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
			"Cleaning up temp synthdefs...".inform;
			if(pathMatch(path).notEmpty) { unixCmdGetStdOut(("rm -f" + "'" ++ path ++ "'") )};

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
			};
		};
	}
}
