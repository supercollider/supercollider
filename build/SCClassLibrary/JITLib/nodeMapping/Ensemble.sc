//every synth that gets added or moved to that group
//is mapped to the given values and buses.

Ensemble : AbstractEnsemble {
	var  <nodeMap;
	
	*new { arg  target,addAction=\addToTail, nodeMap;
			^super.new(target,addAction).ginit(nodeMap);
	}
	
	ginit { arg map;
		nodeMap=map ?? { NodeMap.new };
	}
	nodeMap_ { arg map;
		nodeMap = map;
		nodeMap.send(this);
	}
	map { arg ... args;
		nodeMap.performList(\map, args);
		server.sendBundle(server.latency, 
			["/n_map", nodeID]++args);
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
		server.sendBundle(server.latency, 
			["/n_set", nodeID]++args);
	}
	
	unset { arg ... keys;
		nodeMap.performList(\unset, keys);
	}
	addCommand { arg cmdArray;
		nodeMap.addCommand(cmdArray)
	}
	
	newCommand { arg cmdList, target, addAction=\addToTail;
		^super.newCommand(cmdList, target, addAction).ginit
	}
	updateCommand { arg cmdList, reciever;
		^nodeMap.updateCommand(cmdList, reciever);
	}
	
	
}


