OSCBundle {
	var <target, <messages;
	
	*new { arg target;
		^super.new.tinit(target.asTarget);
	}
	
	tinit { arg targ;
		target = targ;
		messages = List.new;
	}
	
	send { arg latency;
		//target.server.sendMsgList(messages,latency);
		target.server.listSendBundle(latency, messages);
	}
	
	addMsg { arg cmd, argList;
		target.addMsg(messages, cmd, argList);
	}
	
	server { ^target.server }
	
	*newSynth { arg defName, args, target, addAction;
		var synth, bundle;
		synth = Synth.prNew(defName);
		bundle = super.new.tinit(synth);
		bundle.messages.add(synth.newMsg(target, addAction, args));
		^bundle
	}
	
	*newGroup { arg target, addAction;
		var group, bundle;
		group = Group.prNew;
		bundle = super.new.tinit(group);
		bundle.messages.add(group.newMsg(target, addAction));
		^bundle
	}


}

