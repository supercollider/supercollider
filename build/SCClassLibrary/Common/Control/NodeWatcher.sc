//watches an address for created nodes and ending nodes.

AbstractNodeWatcher {

	var <addr, <responders;
	var <isWatching=false;
	
	*new { arg addr;
		^super.new.ninit(addr)
	}
	
	ninit { arg address;
		var cmds;
		addr = address;
		this.clear;
		responders = [];
		addr.asArray.do({ arg addrItem; //support for multiple addresses
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
	
	clear {}
	
	cmds { ^nil }
		
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

}

NodeIDWatcher : AbstractNodeWatcher {
	var <>nodeAllocator;
	
	*new { arg addr, nodeAllocator;
		^super.new(addr).nodeAllocator_(nodeAllocator);
	}
	cmds {  ^#["/n_end"] }

	n_end { arg nodeID;
		//postln("ended" + nodeID);
		nodeAllocator.free(nodeID);
	
	}
	
}



BasicNodeWatcher : AbstractNodeWatcher {
	var <nodes; 
	
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
	
	classvar <>all;
	
	*initClass {
		all = IdentityDictionary.new;
	}
	
	*newFrom { arg server;
		var res;
		res = all.at(server.name);
		if(res.isNil, {
			res = NodeWatcher.new(server.addr);
			res.start;
			all.put(server.name, res) 
		});
		^res
	}
	
	*register { arg node;
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.register(node);
	}
	
	*unregister { arg node;
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.unregister(node);
	}


	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	respond { arg method, msg;
						//msg.postln;
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



DebugNodeWatcher : BasicNodeWatcher {

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	
	//////////////private implementation//////////////
	
	n_go { arg nodeID, groupID, prevID, nextID;
	
		nodes.add(nodeID);
		postln(
		"started" + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		)
	}
	
	
	n_end { arg nodeID, groupID, prevID, nextID;
		nodes.remove(nodeID);
		postln(
		"ended  " + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		)
	}

	n_off { arg nodeID, groupID, prevID, nextID;
		postln(
		"turned off" + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		)
	}	

	n_on { arg nodeID, groupID, prevID, nextID;
		postln(
		"turned on " + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		)	
	}
	


}
