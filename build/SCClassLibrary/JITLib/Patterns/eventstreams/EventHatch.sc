EventHatch {
	//collects values from Event to osc bundles
	
	//currentEnvironment should be Event
	//~finish should be called before.
	
	//setBundle2 etc are alternatives for test
	
	*newSynthMsg { arg nodeID;
		^["/s_new", ~instrument, nodeID, 0, ~group] ++ currentEnvironment.hatchAt(\argNames)
	}
	
	*setBundle { arg nodeID;
		var res;
		res = currentEnvironment.hatchAt(\argNames);
		^if(res.notEmpty, { ["/n_set", nodeID] ++ res }, { res });
	}
	
	*mapBundle { arg nodeID;
		var res;
		res = currentEnvironment.hatchAt(\mapNames);
		^if(res.notEmpty, { ["/n_map", nodeID] ++ res }, { res });	}
	
	*play {
		~server.sendBundle(nil, this.newSynthMsg(~server.nextNodeID))
	}

}

MultiChannelEventHatch : EventHatch {
	
	*newSynthMsg {
		var id, messages, instr;
		messages = currentEnvironment.hatchAt(\argNames);
		if(messages.isEmpty, { ^[[]] });
		messages[0] = messages[0].asArray; //bug..
		messages = messages.multiChannelExpand;
		instr = ~instrument.asArray;
		^messages.collect({ arg item, i;
			["/s_new", instr.wrapAt(i), ~server.nextNodeID, 0, ~group] ++ item 
		});
	}
	
	*setBundle { arg nodeID=16;	
		^super.setBundle(nodeID.asArray).multiChannelExpand ? #[[]] 
		//check bug in multiChannelExpand
	}
	
	*mapBundle { arg nodeID=16; 
		^super.mapBundle(nodeID.asArray).multiChannelExpand ? #[[]]
	}
	
	*setBundle2 { arg nodeID=16;	
		^super.setBundle2(nodeID.asArray).multiChannelExpand ? #[[]] 
		//check bug in multiChannelExpand
	}
	
	

	*play {
		this.newSynthMsg.do({ arg item;
			~server.sendBundle(nil, item)
		})
	}
}


