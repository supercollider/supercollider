+ Bus {

	gate { arg target, start=0.0, level=1.0, dur=0;
		^this.toServer(target, "system_gate_"++rate, [\i_start, start, \i_level,level, \i_dur, dur]);
	}
	
		
	line { arg target, start=0.0, end=1.0, dur=1.0;
		^this.toServer(target, "system_line_"++rate, [ \i_start, start, \i_end, end, \i_dur, dur ]);
	}
	
	xline { arg target, start=0.001, end=1.0, dur=1.0;
		^this.toServer(target, "system_xline_"++rate, [ \i_start, start, \i_end, end, \i_dur, dur ]);
	}
	/*
	triangle { arg target, start=0.0, level=1.0, dur=1.0;
		^this.addNode(target, "system_triangle_"++rate, [ \i_start, start, \i_end, level, \i_dur, dur ]);
	}
	*/
	
	toServer { arg target, defName, args, expand=true;
			var bundle;
			bundle = List.new;
			this.serverMsg(bundle, target, defName, args, expand);
			server.listSendBundle(nil, bundle);
	}
	serverMsg { arg bundle, target, defName, args;
			numChannels.do({ arg i;
				Synth.newMsg(bundle, defName, [\bus, index+i]++(args??{#[]}), 
				target.asTarget, \addBefore)
			});

	} 
	
	gateMsg { arg bundle, target, start=0.0, level=1.0, dur=0;
		^this.serverMsg(bundle, target, "system_gate_"++rate, [\i_start, start, \i_level,level, \i_dur, dur]);
	}
	
	lineMsg { arg bundle, start=0.0, end=1.0, dur=1.0, target;
		^this.serverMsg(bundle, target, "system_line_"++rate, [ \i_start, start, \i_end, end, \i_dur, dur ]);
	}
	
	xlineMsg { arg bundle, start=0.001, end=1.0, dur=1.0, target;
		^this.serverMsg(bundle, target, "system_line_"++rate, [ \i_start, start, \i_end, end, \i_dur, dur ]);

	}


}



