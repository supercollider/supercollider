NodeBundle {
	var <>server, <>nodeID, <>messages;
	
	*new { arg server, nodeID;
		^super.newCopyArgs(server,nodeID).minit;
	}
	
	minit { arg targ;
		messages = List.new;
	}
	
	send { arg latency;
		server.listSendBundle(latency, messages);
	}
	
	addMsg { arg cmd, argList;
		messages.add([cmd, nodeID]++argList);
	}
	
	server { ^target.server }
	
}
