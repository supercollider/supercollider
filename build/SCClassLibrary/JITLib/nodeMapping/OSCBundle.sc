OSCBundle {
	var <target, <msgs;
	
	*new { arg target;
		^super.new.tinit(target.asTarget);
	}
	
	tinit { arg targ;
		target = targ;
		msgs = List.new;
	}
	
	send { arg latency;
		target.server.sendMsgList(msgs,latency);
	}
	
	addMsg { arg cmd, argList;
		target.addMsg(msgs, cmd, argList);
	}
	
	server { ^target.server }
	
	*newSynth { arg defName, args, target, addAction;
		var synth, bundle;
		synth = Synth.prNew(defName);
		bundle = super.new.tinit(synth);
		bundle.msgs.add(synth.newMsg(target, addAction, args));
		^bundle
	}
	
	*newGroup { arg target, addAction;
		var group, bundle;
		group = Group.prNew;
		bundle = super.new.tinit(group);
		bundle.msgs.add(group.newMsg(target, addAction));
		^bundle
	}


}

