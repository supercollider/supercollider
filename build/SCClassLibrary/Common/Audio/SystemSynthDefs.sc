
// synthdefs needed by classes

+ SynthDef {

	*initClass {
		StartUp.add {
		
			2.do { arg i; i = i + 1;
				SynthDef("system_link_audio_" ++ i, 
				{ arg out=0, in=16, vol=1, doneAction=2;
					var env;
					env = EnvGate( doneAction:doneAction, curve:'sin') * Lag.kr(vol, 0.05);
					Out.ar(out, InFeedback.ar(in, i) * env) 
				}, [\kr, \ir, \kr, \ir]).writeDefFile;
			};
			2.do { arg i; i = i + 1;
				SynthDef.writeOnce("system_link_control_" ++ i, 
				{ arg out=0, in=16, doneAction=2;
					var env;
					env = EnvGate( doneAction:doneAction, curve:'lin');
					Out.kr(out, In.kr(in, i) * env) 
				}, [\kr, \ir, \kr, \ir]).writeDefFile;
			};
			
			for(1,8,{ arg i;
				SynthDef.writeOnce("system_diskout_" ++ i.asString, { arg i_in, i_bufNum=0;
					DiskOut.ar(i_bufNum, InFeedback.ar(i_in, i));
				});
			});		
	
		}
	}


}