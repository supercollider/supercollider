EventHatch {
	//collects values from Event to osc bundles
	
	//currentEnvironment should be Event
	//~finish should be called before.
	
	
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
	

}

MultiChannelEventHatch : EventHatch {
	
	*newSynthMsg {
		var id, messages, instr;
		messages = currentEnvironment.hatchAt(\argNames);
		messages = messages.multiChannelExpand;
		instr = ~instrument.asArray;
		^messages.collect({ arg item, i;
			["/s_new", instr.wrapAt(i), ~server.nextNodeID, 0, ~group ? 0] ++ item 
		});
	}
	
	*setBundle { arg nodeIDs;	
		^super.setBundle(nodeIDs.asArray).multiChannelExpand
	}
	
	*mapBundle { arg nodeIDs; 
		^super.mapBundle(nodeIDs.asArray).multiChannelExpand
	}
	

}


