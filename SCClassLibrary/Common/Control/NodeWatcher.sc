//watches a server address for node-related messages

AbstractNodeWatcher {

	var <server, <responders;
	var <isWatching=false;

	*new { arg server;
		^super.new.ninit(server)
	}

	ninit { arg argServer;
		var cmds;
		server = argServer;
		this.clear;
		responders = [];
		server.addr.asArray.do({ arg addrItem; //support for multiple addresses
			this.cmds.do({ arg cmd;
				var method;
				method = cmd.copyToEnd(1).asSymbol;
				responders = responders.add(
					OSCFunc({ arg msg; this.respond(method, msg) }, cmd, addrItem)
						.permanent_(true)
						.disable
				)
			});
		});
	}

	clear {}

	cmds { ^nil }

	respond { arg method, msg;
		this.performList(method, msg.drop(1))
	}

	start {
		if(isWatching.not, {
			responders.do({ arg item; item.enable });
			isWatching = true;
		})
	}
	stop {
		if(isWatching, {
			responders.do({ arg item; item.free });
			isWatching = false;
			this.free;
		})
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

	cmds {  ^#["/n_go", "/n_end"] }

	n_end { arg nodeID;
		nodes.remove(nodeID);
	}
	n_go { arg nodeID;
		nodes.add(nodeID);
	}
}




///////////////////////////////////
//watches registered nodes and sets their isPlaying/isRunning flag.
//a node needs to be  registered to be addressed, other nodes are ignored.

NodeWatcher : BasicNodeWatcher {

	classvar <>all;

	*initClass {
		all = IdentityDictionary.new;
		CmdPeriod.add(this);
	}

	*cmdPeriod { all.do { arg item; item.clear } }

	*newFrom { arg server;
		var res;
		res = all.at(server.name);
		if(res.isNil, {
			res = this.new(server);
			res.start;
			all.put(server.name, res)
		});
		^res
	}

	*register { arg node, assumePlaying=false;
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.register(node, assumePlaying);
	}

	*unregister { arg node;
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.unregister(node);
	}

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	respond { arg method, msg;
		var node, group;
		node = nodes.at(msg.at(1));
		if(node.notNil, {
				group = nodes.at(msg.at(2));
				this.performList(method, node, group)
		})
	}

	clear {
		// we must copy 'nodes'
		// b/c a /n_end dependant might add or remove nodes
		// from the collection
		// NEVER iterate over a collection that might change
		nodes.copy.do({ arg node;
			node.isPlaying = false;
			node.isRunning = false;
			node.changed(\n_end);
		});
		nodes = IdentityDictionary.new
	}

	register { arg node, assumePlaying=false;
		if(server.serverRunning.not) { nodes.removeAll; ^this };
		if(isWatching) {
			if(assumePlaying and: { nodes.at(node.nodeID).isNil }) { node.isPlaying = true };
			nodes.put(node.nodeID, node);
		};
	}

	unregister { arg node;
		nodes.removeAt(node.nodeID);
	}


	//////////////private implementation//////////////

	n_go { arg node;
		node.isPlaying = true;
		node.isRunning = true;
		node.changed(\n_go);  // notify all the node's dependents of the change
	}

	n_end { arg node;
		this.unregister(node);
		node.isPlaying = false;
		node.isRunning = false;
		node.changed(\n_end);
	}

	n_off { arg node;
		node.isRunning = false;
		node.changed(\n_off);
	}

	n_on { arg node;
		node.isRunning = true;
		node.changed(\n_on);
	}

}


DebugNodeWatcher : BasicNodeWatcher {

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }

	//////////////private implementation//////////////

	doPost { arg action, nodeID, groupID, prevID, nextID;
		Post << ("[ server: " + server.name + "]" +
		action + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		) << Char.nl
	}

	n_go { arg nodeID, groupID, prevID, nextID;
		this.doPost("started", nodeID, groupID, prevID, nextID);
		nodes.add(nodeID);
	}

	n_end { arg nodeID, groupID, prevID, nextID;
		nodes.remove(nodeID);
		this.doPost("ended", nodeID, groupID, prevID, nextID);
	}

	n_off { arg nodeID, groupID, prevID, nextID;
		this.doPost("turned off", nodeID, groupID, prevID, nextID);
	}

	n_on { arg nodeID, groupID, prevID, nextID;
		this.doPost("turned on", nodeID, prevID, nextID);
	}

}
