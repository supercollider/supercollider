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



	
+ NodeProxy {

	gate { arg level=1.0, dur=0;
		if(bus.notNil && this.isPlaying, { bus.gate(level, dur, group) }) 
		{ "not playing".inform };
	}
	
	line { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.line(level, dur, group) })
		{ "not playing".inform };
	}
	
	xline { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.xline(level, dur, group) })
		{ "not playing".inform };
	}
	
	env { arg env;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.env(env) })
		{ "not playing".inform };
	}
		
	gateAt { arg key, level=1.0, dur=1.0;
		this.group.set(key, level); 
		SystemClock.sched(dur, { this.group.set(key, nodeMap[key].value); nil });
	}
	
	lineAt { arg key, level=1.0, dur;
		this.performAtControl(\line, key, level, dur);
	}
	
	xlineAt { arg key, level=1.0, dur;
		this.performAtControl(\xline, key, level, dur);
	}
	
	// private
	performAtControl { arg action, keys, levels=1.0, durs;
		var ctlBus, bundle, id, setArgs, setBundle, ctlIndex, missing;
		if(this.isPlaying) {
			durs = durs ? this.fadeTime;
			id = group.nodeID;
			keys = keys.asArray; levels = levels.asArray; durs = durs.asArray;

			ctlBus = Bus.alloc('control', server, keys.size);
			ctlIndex = ctlBus.index;
			bundle = ["/n_map", id];
			keys.do { arg key, i; bundle = bundle.addAll([key, ctlIndex + i]) };
			missing = keys.select { arg key; nodeMap[key].isNil };
			if(missing.notEmpty) { 
				this.supplementNodeMap(missing); 
				nodeMap.addToBundle(bundle, group) 
			};
			server.sendBundle(server.latency, bundle);
				
			ctlBus.perform(action, levels, durs);
			ctlBus.free;
				
			setArgs = [keys, levels].flop.flat;
				// set the node map
			nodeMap.set(*setArgs);
				// finally set it to that vealue
			server.sendBundle(server.latency + durs.maxItem, 
					 ["/n_map", id] ++ [keys, -1].flop.flat,
					 ["/n_set", id] ++ setArgs
				);
		} { "not playing".inform };
		
	}
}
