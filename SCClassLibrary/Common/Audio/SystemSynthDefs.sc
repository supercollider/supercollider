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
		// clean up any written synthdefs starting with "temp__"
		StartUp.add {
			var path = SynthDef.synthDefDir ++ tempNamePrefix ++ "*";
			if(pathMatch(path).notEmpty) { unixCmd("rm -f" + path.quote) };

			(1..numChannels).do { arg i;
				SynthDef("system_link_audio_" ++ i,
					{ arg out=0, in=16, vol=1, doneAction=2;
						var env;
						env = EnvGate( doneAction:doneAction, curve:'sin') * Lag.kr(vol, 0.05);
						Out.ar(out, InFeedback.ar(in, i) * env)
					}, [\kr, \ir, \kr, \ir]).add;

				SynthDef("system_link_control_" ++ i,
					{ arg out=0, in=16, doneAction=2;
						var env;
						env = EnvGate( doneAction:doneAction, curve:'lin');
						Out.kr(out, In.kr(in, i) * env)
					}, [\kr, \ir, \kr, \ir]).add;

				SynthDef("system_diskout_" ++ i, { arg i_in, i_bufNum=0;
					DiskOut.ar(i_bufNum, InFeedback.ar(i_in, i));
				}).add;
			};
		};
	}
}
