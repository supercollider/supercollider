/*
OSCpathDispatcher dispatches OSC commands to OSCpathResponder 's.
It is also an OSCMultiResponder, so it distributes a command to
any OSCresponderNode's assigned to that command.

path: an array of the indices in the OSC message that are the path values

*/
OSCpathDispatcher : OSCMultiResponder {

	classvar <>cmdPathIndices;

	var <>pathResponders;
	var <>maxPathSize = 0;

	*new {  arg addr, cmdName, action, pathSize;
		^super.new(addr, cmdName, action).initPathSize(pathSize);
	}
	initPathSize { arg pathSize;
		maxPathSize = pathSize;
		pathResponders = Set.new;
	}
	value { arg time, msg;
		var cmdPath, match, responder;
		super.value(time, msg);
		if (maxPathSize.notNil, {
			cmdPath = [cmdName] ++ msg[1..maxPathSize];
			responder = OSCpathResponder(addr, cmdPath);
			match = pathResponders.findMatch(responder);
			if (match.notNil, {
				match.value(time, msg);
			});
			(maxPathSize - 1).do({ arg i;
				responder.path.removeAt(responder.path.size - 1);
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
		if(responder.path.size > maxPathSize) { maxPathSize = responder.path.size };
	}
	removeChild { arg responder;
		 pathResponders.remove(responder);
		 if(responder.path.size == maxPathSize) {
		 	maxPathSize = pathResponders.maxValue({ |resp| resp.path.size }) ? 0;
		 };
		 if(this.isEmpty) { this.remove };
	}

	isEmpty { ^(nodes.size + pathResponders.size) == 0 }
}

OSCpathResponder : OSCresponder {
	var <>path;
	var <>dispatcher;

	*new { arg addr, cmdPath, action;
		var cmdName, path;
		#cmdName ...path = cmdPath;
		^super.new(addr, cmdName, action).path_(path);
	}

	findDispatcher {
		var responder, match, pathIndices;
		responder = OSCpathDispatcher(addr, cmdName, nil, path.size);
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
