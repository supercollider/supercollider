/* 
OSCpathDispatcher dispatches OSC commands to OSCpathResponder 's.
It is also an OSCMultiResponder, so it distributes a command to
any OSCresponderNode's assigned to that command.

path: an array of the indices in the OSC message that are the path values

*/
OSCpathDispatcher : OSCMultiResponder {

	classvar <>cmdPathIndices;

	var <>pathResponders;
	var <>pathIndices;
	
	*initClass {
		cmdPathIndices = IdentityDictionary.new;		
		cmdPathIndices.put('/b_set',	#[1,2]);
		cmdPathIndices.put('/b_setn',	#[1,2]);
		cmdPathIndices.put('/c_set',	#[1]);
		cmdPathIndices.put('/c_setn',	#[1]);
		cmdPathIndices.put('/n_set',	#[1,2]);
		cmdPathIndices.put('/n_setn',	#[1,2]);
		cmdPathIndices.put('/tr',		#[1,2]);
		cmdPathIndices.put('/n_end',	#[1]);
		cmdPathIndices.put('/c_end',	#[1]);		// dummy OSC command
	}	
	*new {  arg addr, cmdName, action, pathIndices;
		^super.new(addr, cmdName, action).init(pathIndices);
	}
	init { arg argPathIndices = 1;
		pathIndices = argPathIndices ? cmdPathIndices.at(cmdName);
		pathResponders = Set.new;
	}
	value { arg time, msg; 
		var cmdPath, match, responder;
		super.value(time, msg);
		if (pathIndices.notNil, {
			cmdPath = [cmdName] ++ pathIndices.collect({ arg i; msg.at(i) });
			responder = OSCpathResponder(addr, cmdPath);
			match = pathResponders.findMatch(responder);
			if (match.notNil, { 
				match.value(time, msg); 
			});
			pathIndices.size.do({ arg i;
				responder.path.put(i,nil);
				match = pathResponders.findMatch(responder);
				if (match.notNil, { 
					match.value(time, msg); 
				});
			});
		});
	}	
	addChild { arg responder;
		var old;
		old = pathResponders.findMatch(responder);
		if(old.notNil,{ pathResponders.remove(old) });
		pathResponders.add(responder);
	}
	removeChild { arg responder;
		 pathResponders.remove(responder);
		 // if(this.isEmpty) { this.remove };
	}
	
	isEmpty { ^(nodes.size + pathResponders.size) == 0 }
}

OSCpathResponder : OSCresponder {
	var <>path;
	var <>dispatcher;
	
	*new { arg addr, cmdPath, action; 
		var cmdName, path;
		#cmdName ...path = cmdPath;
		^super.newCopyArgs(addr, cmdName, action, path);
	}
	
	findDispatcher {
		var responder, match;
		responder = OSCpathDispatcher(addr, cmdName);
		match = OSCresponder.all.findMatch(responder);
		if(match.isNil, { ^responder.add });
		if (match.class === OSCresponder,  {
			match.remove;
			responder.nodes_([match]);
			^responder.add
		});
		if (match.class === OSCMultiResponder, {
			match.remove;
			responder.nodes_(match.nodes);
			^responder.add;
		});
		^match;
	}		
	add { 
		dispatcher = this.findDispatcher;
		dispatcher.addChild(this);
	}
	
	remove { 
		dispatcher.removeChild(this);
	}
		
	== { arg that;
		^that respondsTo: \path and: { path == that.path }
	}
	hash { ^path.hash }
}		

/*
(
	var s, commandpath, response, aSynth, nodeID, triggerID;
	s = Server.local;
	s.boot;
	triggerID = 1;
	aSynth = {arg freq = 1, triggerID = 1; SendTrig.kr(SinOsc.kr(freq), triggerID, 666); }.play;
	nodeID = aSynth.nodeID;
	commandpath = ['/tr', nodeID, triggerID];
	response = { arg time, responder, message; message.postln };

	o = OSCpathResponder(s.addr, commandpath, response);
	o.add;

)

o.remove

(
	var s, commandpath, response, aSynth, nodeID, triggerID;
	s = Server.local;
	commandpath = ['/tr', nil, triggerID];
	response = { arg time, responder, message; message.postln };

	o = OSCpathResponder(s.addr, commandpath, response);
	o.add;
)

*/

