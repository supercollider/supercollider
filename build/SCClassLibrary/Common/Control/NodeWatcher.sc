//watches a server for created nodes and ending nodes.

BasicNodeWatcher {
	var <nodes, <server, <responders;
	var <isWatching=false;
	
	*new { arg server;
		^super.new.ninit(server)
	}
	
	ninit { arg s;
		var cmds;
		server = s;
		this.clear;
		responders = this.cmds.collect({ arg cmd;
				var method;
				method = cmd.copyToEnd(1).asSymbol;
				OSCresponder(server.addr, cmd, 
					{ arg time, resp, msg; this.respond(method, msg) }
				)
		});
	}
	cmds {  ^#["/n_go", "/n_end"] }
	
	respond { arg method, msg;
				//msg.postln;
				msg.removeAt(0);
				this.performList(method, msg)
	}
	
	start {
		server.notify(true);
		if(isWatching.not, {
			responders.do({ arg item; item.add });
			isWatching = true;
		})
	}
	stop {
		if(isWatching, {
			responders.do({ arg item; item.remove });
			isWatching = false;
			this.clear;
		})
	}
	
	nodeIsPlaying { arg nodeID;
		^nodes.includes(nodeID);
	}
		
	clear { 
		nodes = IdentitySet.new;
		
	}
		
	n_end { arg nodeID;
		nodes.remove(nodeID);
	}
	n_go { arg nodeID;
		nodes.add(nodeID);
	}
	

}


///////////////////////////////////
//watches registered nodes and sets their isPlaying/isRunning flag. 
//a node needs to be  registered to be linked, other nodes are ignored.

NodeWatcher : BasicNodeWatcher {
	

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	respond { arg method, msg;
						msg.removeAt(0);
						if(nodes.at(msg.at(0)).notNil, {//for speed
							msg = this.lookUp(msg);
							if(msg.notNil, {
								this.performList(method, msg)
							})
						})
	}
	lookUp { arg msg;
		var res;
		res = msg.collect({ arg nodeID; nodes.at(nodeID) });
		^if(res.at(0).notNil, { res }, { nil });
		//todo: remote creation scheme
	}
	
	clear {
		nodes = IdentityDictionary.new 
	}
	
	
	register { arg node;
		if(isWatching, {
			nodes.put(node.nodeID, node);
		});
	}
	
	unregister { arg node;
		nodes.removeAt(node.nodeID);
		//node.freeNodeID;
	}
	
	
	//////////////private implementation//////////////
	
	n_go { arg node, group, prev, next;
		
		node.group = group;
		node.isPlaying = true;
		node.isRunning = true;
		
	}
	
	
	n_end { arg node;
		this.unregister(node);
		node.group = nil;
		node.isPlaying = false;
		node.isRunning = false;
	}

	n_off { arg node, group, prev, next;
		
		node.isRunning = false;
	}	

	n_on { arg node, group, prev, next;
		
		node.isRunning = true;
	}
	
	


}



NodeWatcherLinker : NodeWatcher {
	//if you want nodes to be linked, use this

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on", "/n_move"] }
	
	n_go { arg node, group, prev, next;
		//group is set in the prMove calls
		//this needs to be revisited, as group respond might 
		//come back from server after its synth respond.
		
		if(prev.notNil, {
			node.prMoveAfter(prev);
		}, {
			if(next.notNil, {
				node.prMoveBefore(next);
			}, {
				group.asGroup.prAddTail(node); 
			})
		});
		
		
		node.isPlaying = true;
		node.isRunning = true;
		
	}
	
	n_move { arg node, group, prev, next;
		//group is set in the prMove calls
		node.remove;
		if(prev.notNil, {
			node.prMoveAfter(prev);
		}, {
			if(next.notNil, {
				node.prMoveBefore(next);
			}, {
				group.prAddTail(node);
			})
		});
	}
	
	n_end { arg node;
		node.remove;
		this.unregister(node);
		node.group = nil;
		node.isPlaying = false;
		node.isRunning = false;
	}

	n_off { arg node, group, prev, next;
		
		node.isRunning = false;
	}	

	n_on { arg node, group, prev, next;
		
		node.isRunning = true;
	}
	

}