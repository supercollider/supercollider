//every synth that gets added or moved to that group
//is mapped to the given values and buses.

Ensemble : AbstractEnsemble {
	var  <nodeMap;
	
	*new { arg  target,addAction=\addToTail, nodeMap;
			^super.new(target,addAction).ginit(nodeMap);
	}
	*prNew { ^super.new.ginit }
	ginit { arg map;
		nodeMap = map ?? { NodeMap.new };
	}
	nodeMap_ { arg map;
		nodeMap = map;
		if(this.isPlaying, {nodeMap.send(this)});
	}
	map { arg ... args;
		nodeMap.performList(\map, args);
		if(this.isPlaying,{
		server.sendBundle(server.latency, 
			["/n_map", nodeID]++args);
		});
	}
	
	unmap { arg ... args;
		nodeMap.performList(\unmap, args);
		
		forBy(0, args.size-1, 2, { arg i; 
			var key;
			key = args.at(i).asSymbol;
			super.map(key, -1) 
		});
	}
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying,{
		server.sendBundle(server.latency, 
			[15, nodeID]++args);
		})
	}
	
	unset { arg ... keys;
		nodeMap.performList(\unset, keys);
	}
	
	setn { arg controlName,  values ... args;
		nodeMap.performList(\setn, [controlName,  values]++ args);
		if(this.isPlaying,{
			args = args.collect({ arg cnv;
				[cnv.at(0), cnv.at(1).size, cnv.at(1)];
			}).flat;
			server.sendBundle(server.latency, [16, nodeID, controlName, values.size, values] ++ args);
		})
	}
	
	fill { arg controlName, numControls, value ... args;
		this.addMsg(["/n_fill"]++[numControls, value]++args);
		server.sendBundle(server.latency, 
			[17, nodeID,controlName,numControls, value]++args); //"n_setn"
	}	
	addMsg { arg cmdArray;
		nodeMap.addMsg(cmdArray)
	}
	
	newMsg { arg msgList, target, addAction=\addToTail, nodeMap;
		var res;
		res = super.prNew;
		msgList.add(res.newMsg(target, addAction));
		res.nodeMap_(nodeMap); 
		nodeMap.updateMsg(msgList, res);//not needed?
		^res
	}
	updateMsg { arg cmdList, reciever;
		^nodeMap.updateMsg(cmdList, reciever);
	}
	
	
}


