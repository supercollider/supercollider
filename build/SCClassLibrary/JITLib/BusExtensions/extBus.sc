+ Bus {

	gate { arg level=1.0, dur=0, target;
		^this.toServer(target.asTarget, "system_gate_"++rate, level, dur);
	}
	
	line { arg level=1.0, dur=1.0, target;
		^this.toServer(target.asTarget, "system_line_"++rate, level, dur);
	}
	
	xline { arg level=1.0, dur=1.0, target;
		^this.toServer(target.asTarget, "system_xline_"++rate, level, dur);
	}
	
	//starts/ends to original value in fadeTime
	env { arg env, fadeTime=0.1, target;
		var bundle, size;
		bundle = List.new;
		size = binaryValue(env.levels.size > 4);
		env = env.asArray;
		
		numChannels.do({ arg i; //do it within a group later, only when numChannels > 1
			bundle = bundle.add(
				[9, "system_env_"++size++"_"++rate, -1, 0, 0, 
					\i_bus, index+i 
					//\i_fadeTime, fadeTime
				]
			);
			bundle = bundle.add([\n_setn, -1, \env, env.size] ++ env);
		});
		server.listSendBundle(nil, bundle);
				
	}
		
	toServer { arg target, defName, level, dur;
			var bundle, args;
			args = [\i_level, level, \i_dur, dur];
			bundle = List.new;
			numChannels.do({ arg i;
				this.gateBusToBundle(bundle, target, defName, args, index+i);
			});
			server.listSendBundle(nil, bundle);
						
	}
	
	gateBusToBundle { arg bundle, target, defName, args, index;
			bundle.add([9, defName, -1, 0, target.asNodeID, \i_bus, index]++args);
			if(rate === 'control', {
				bundle.add(["/n_map", -1, \i_start, index]) 
			});
	} 
	

}



