//watches an address for created nodes and ending nodes.

BasicNodeWatcher {
	var <nodes, <addr, <responders;
	var <isWatching=false;
	
	*new { arg addr;
		^super.new.ninit(addr)
	}
	
	ninit { arg address;
		var cmds;
		addr = address;
		this.clear;
		responders = [];
		addr.asArray.do({ arg addrItem; //support for multiple servers
			this.cmds.do({ arg cmd;
				var method;
				method = cmd.copyToEnd(1).asSymbol;
				responders = responders.add(
					OSCresponderNode(addrItem, cmd, 
						{ arg time, resp, msg; this.respond(method, msg) }
					)
				)
			});
		});
	}
	cmds {  ^#["/n_go", "/n_end"] }
	
	respond { arg method, msg;
				//msg.postln;
				msg = msg.copy;
				msg.removeAt(0);
				this.performList(method, msg)
	}
		
	start {
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
	nodeIsPlaying_ { arg nodeID; 	
		//sometimes it is needed to set this before the server reply
		^nodes.add(nodeID);
	}
		
	clear { 
		nodes = IdentitySet.new;
		
	}
		
	n_end { arg nodeID;
		postln("ended" + nodeID);
		nodes.remove(nodeID);
		//nodes.postln;
	}
	n_go { arg nodeID;
		postln("started" + nodeID);
		nodes.add(nodeID);
		//nodes.postln;
	}
	

}


///////////////////////////////////
//watches registered nodes and sets their isPlaying/isRunning flag. 
//a node needs to be  registered to be linked, other nodes are ignored.

NodeWatcher : BasicNodeWatcher {
	

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	respond { arg method, msg;
						msg.postln;
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