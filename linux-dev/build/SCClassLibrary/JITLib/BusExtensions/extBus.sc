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
		
	toServer { arg target, defName, level, dur;
			var bundle, args;
			args = [\i_level, level, \i_dur, dur];
			bundle = List.new;
			numChannels.do({ arg i;
				this.gateSynthMsg(bundle, target, defName, args, index+i);
			});
			server.listSendBundle(nil, bundle);
						
	}
	
	gateSynthMsg { arg bundle, target, defName, args, index;
			var synth;
			synth = Synth.newToBundle(bundle, defName, [\i_bus, index]++args, 
				target.asTarget.group, \addToHead);
			if(rate === 'control', {
				synth.msgToBundle(bundle, "/n_map", [\i_start, index]) 
			});
	} 
	

}



