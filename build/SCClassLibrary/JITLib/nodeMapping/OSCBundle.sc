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
	
}



SynthBundle : OSCBundle {
	*new { arg defName, args, target, addAction;
		var synth, bundle;
		target = target.asTarget;
		synth = Synth.prNew(defName, target.server);
		bundle = super.new(synth);
		bundle.messages.add(synth.newMsg(target, addAction, args));
		^bundle
	}
}

GroupBundle : OSCBundle {
	*new { arg target, addAction;
		var group, bundle;
		target = target.asTarget;
		group = Group.prNew(target.server);
		bundle = super.new(group);
		bundle.messages.add(group.newMsg(target, addAction));
		^bundle
	}
}
