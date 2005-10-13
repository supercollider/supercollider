
// synthdefs needed by classes

SystemSynthDefs {
	classvar <>numChannels = 2;
	classvar <>tempNamePrefix = "temp__";
	
		
	*initClass {
		StartUp.add {
			// clean up any written synthdefs starting with "temp__"
			var path = SynthDef.synthDefDir ++ tempNamePrefix ++ "*";
			if(pathMatch(path).notEmpty) { unixCmd("rm" + path) };
		
			(1..numChannels).do { arg i;
				SynthDef("system_link_audio_" ++ i, 
				{ arg out=0, in=16, vol=1, doneAction=2;
					var env;
					env = EnvGate( doneAction:doneAction, curve:'sin') * Lag.kr(vol, 0.05);
					Out.ar(out, InFeedback.ar(in, i) * env) 
				}, [\kr, \ir, \kr, \ir]).writeDefFile;
			};
			(1..numChannels).do { arg i;
				SynthDef.writeOnce("system_link_control_" ++ i, 
				{ arg out=0, in=16, doneAction=2;
					var env;
					env = EnvGate( doneAction:doneAction, curve:'lin');
					Out.kr(out, In.kr(in, i) * env) 
				}, [\kr, \ir, \kr, \ir]).writeDefFile;
			};
			
			(1..numChannels).do { arg i;
				SynthDef.writeOnce("system_diskout_" ++ i.asString, { arg i_in, i_bufNum=0;
					DiskOut.ar(i_bufNum, InFeedback.ar(i_in, i));
				});
			};		
	
		}
	}


}

