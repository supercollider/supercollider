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
	
	
	env { arg env, fadeTime=0.1, target;
		var bundle, size;
		bundle = List.new;
		size = binaryValue(env.levels.size > 4);
		env.levels = env.levels.extend(8, env.levels.last);
		env = env.asArray;
		numChannels.do({ arg i;
			bundle = bundle.add(
				[9, "system_env_" ++ size ++ "_" ++ rate, -1, 0, target.asNodeID, 
					\i_bus, index+i 
				]
			);
			bundle = bundle.add([\n_setn, -1, \env, env.size] ++ env);
		});
		server.listSendBundle(nil, bundle);
				
	}
		
	toServer { arg target, defName, level, dur;
			var bundle;
			level = level.asArray;
			dur = dur.asArray;
			bundle = List.new;
			numChannels.do({ arg i;
				this.gateBusToBundle(bundle, target, defName, 
				[\i_level, level.wrapAt(i), \i_dur, dur.wrapAt(i)], index+i);
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



