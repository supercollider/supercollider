Pfx : FilterPattern {
	var <>fxname, <>pairs;
	*new { arg pattern, fxname ... pairs;
		if (pairs.size.odd, { Error("Pfx should have odd number of args.\n").throw });
		^super.new(pattern).fxname_(fxname).pairs_(pairs)
	}
	storeArgs { ^[pattern, fxname] ++ pairs }
	embedInStream { arg inevent;	
		var stream;
		var server = inevent[\server] ?? { Server.default };
		var id = server.nextNodeID;
		var event = inevent.copy;
		
		event ?? { ^nil.yield };
		
		pairs.pairsDo {|name, value|
			event[name] = value;
		};
		event[\addAction] = 1; // add to tail of group.
		event[\instrument] = fxname;
		event[\type] = \on;
		event[\id] = id;
		event[\delta] = 0;
		inevent = event.yield;
		
		stream = pattern.asStream;
		
		loop {
			event = stream.next(inevent);
			if (event.isNil) { 
				event = inevent.copy ?? { Event.default };
				event[\type] = \off;
				event[\id] = id;
				event[\delta] = 0;
				event.play;
				^if(inevent.isNil) { nil.yield } { inevent };
			};
			inevent = event.yield;
		};
	}
}



Pgroup : FilterPattern {
	
	embedInStream { arg inevent;
	
		var server, groupID, event, ingroup;
		
		if(inevent.isNil) { ^nil.yield };		
		
		server = inevent[\server] ?? { Server.default };
		ingroup = inevent[\group];		
		groupID = server.nextNodeID;
		
		event = inevent.copy;
		event[\addAction] = 1;
		event[\type] = \group;
		event[\delta] = 1e-9; // no other sync choice for now. (~ 1 / 20000 sample delay)
		event[\id] = groupID;
		event[\group] = ingroup;
		
		inevent = event.yield;
		
		inevent !? { inevent = inevent.copy; inevent[\group] = ingroup };
		^this.class.embedLoop(inevent, pattern.asStream, groupID, ingroup);
		
	}
	
	*embedLoop { arg inevent, stream, groupID, ingroup;
		var event;
		 loop {
			event = stream.next(inevent);
			
			if(event.isNil or: { inevent.isNil }) {
				if(inevent.isNil) {  
					event = Event.default; 
				} {
					event = inevent.copy
				};				
				
				stream.next(nil);
				
				event[\type] = \kill;
				event[\delta] = 0;
				event[\id] = groupID;
				event[\lag] = event.use { ~sustain.value }; // delay freeing to avoid click
				
				event.play;
				if(inevent.isNil) { ^nil.yield } { ^inevent }
			};
						
			inevent = event.yield;
			inevent !? { inevent.put(\group, groupID);  };
		}
	}
}

Pbus : FilterPattern {
	var <>numChannels, <>rate, <>dur=2.0, <>fadeTime;
	
	*new { arg pattern, dur=2.0, fadeTime=0.02, numChannels=2, rate=\audio;
		^super.new(pattern).dur_(dur).numChannels_(numChannels).rate_(rate).fadeTime_(fadeTime)
	}
	
	embedInStream { arg inevent;
		var server, groupID, linkID, bus, ingroup;
		var patterns, event, freeBus, stream;
				
		if(inevent.isNil) { ^nil.yield };
		server = inevent[\server] ?? { Server.default };
		groupID = server.nextNodeID;
		linkID = server.nextNodeID;
		ingroup = inevent[\group];
		
		// could use a special event type for this:
		if(rate == \audio) {
			bus = server.audioBusAllocator.alloc(numChannels);
			freeBus = { server.audioBusAllocator.free(bus) };
		} {
			bus = server.controlBusAllocator.alloc(numChannels);
			freeBus = { server.controlBusAllocator.free(bus) };
		};
		
		event = inevent.copy;
		event[\addAction] = 1;
		event[\type] = \group;
		event[\delta] = 1e-9;
		event[\id] = groupID;
		event[\group] = ingroup;
		event.yield;
		
		
		inevent = event = inevent.copy;
		
		event[\type] = \on;
		event[\group] = groupID;
		event[\addAction] = 3;
		event[\delta] = 1e-9;
		event[\id] = linkID;
		event[\fadeTime] = fadeTime;
		event[\instrument] = format("system_link_%_%", rate, numChannels);
		event[\in] = bus;
		
		// doneAction = 3;   
		// remove and deallocate both this synth and the preceeding node 
		// (which is the group).
		
		event[\msgFunc] = #{ |out, in, fadeTime, gate=1|
			[\out, out, \in, in, \fadeTime, fadeTime, \gate, gate, \doneAction, 3] 
		};
		
		inevent = event.yield;
		
		
		// now embed the pattern
		
		stream = Pchain(pattern, (group: groupID, out: bus)).asStream;
		
		
		loop {
			
			if(inevent.isNil) {
				stream.next(nil); // end child streams
				
				event = (inevent ?? { Event.default }).copy;
				event[\type] = \off;
				event[\id] = linkID;
				event[\gate] = dur.neg;
				event[\hasGate] = true;				
				event.play;
				
				freeBus.value;
				^nil.yield;
			};
			
			event = stream.next(inevent);
			
			if(event.isNil) {
				
				event = (inevent ?? { Event.default }).copy;
				event[\type] = \off;
				event[\id] = linkID;
				event[\gate] = dur.neg;
				event[\hasGate] = true;				
				event.play;
				
				freeBus.value;
				^inevent
			
			};
			inevent = event.yield;
			
		}
	}
	
}

