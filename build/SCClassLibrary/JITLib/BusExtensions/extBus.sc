+ Bus {

	gate { arg level=1.0, dur=0, target, latency;
		^this.toServer(target.asTarget, "system_gate_"++rate, [\i_level,level, \i_dur, dur], latency);
	}
	
		
	line { arg level=1.0, dur=1.0, target, latency;
		^this.toServer(target.asTarget, "system_line_"++rate, [ \i_end, level, \i_dur, dur ], latency);
	}
	
	xline { arg level=1.0, dur=1.0, target, latency;
		^this.toServer(target.asTarget, "system_xline_"++rate, [ \i_end, level, \i_dur, dur ], latency);
	}
		
	toServer { arg target, defName, args, latency;
			var bundle, resp, time;
			bundle = List.new;
			latency = latency ? server.latency;
			time = thisThread.time;
			resp = OSCresponder(target.server.addr, "/c_set", { arg time, resp, msg;
				msg.removeAt(0);
				numChannels.do({ arg i;
					var cArgs;
					cArgs = args.add(\i_start);
					cArgs = cArgs.add(msg.at(2*i+1));
					this.gateSynthMsg(bundle, target, defName, cArgs, index+i);
				});
				time = (latency - time + thisThread.time).max(0);
				bundle.asCompileString.postln;
				server.listSendBundle(time, bundle);
				resp.remove;
				
			}).add;
			
			server.listSendMsg(["/c_get"]++Array.series(numChannels, index, 1));
			
	}
	gateSynthMsg { arg bundle, target, defName, args, index;
			Synth.newMsg(bundle, defName, [\i_bus, index]++args, 
				target.asTarget.group, \addToHead)
	} 
	

}



