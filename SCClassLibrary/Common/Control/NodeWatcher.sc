//watches a server address for node-related messages

AbstractNodeWatcher {

	var <server, <responders;
	var <isWatching = false;

	*new { |server|
		^super.new.ninit(server)
	}

	ninit { |argServer|
		var cmds;
		server = argServer;
		this.clear;
		responders = [];
		server.addr.asArray.do { |addrItem| //support for multiple addresses
			this.cmds.do { |cmd|
				var method;
				method = cmd.copyToEnd(1).asSymbol;
				responders = responders.add(
					OSCFunc({ |msg| this.respond(method, msg) }, cmd, addrItem)
						.permanent_(true)
						.disable
				)
			}
		}
	}

	clear {}

	cmds { ^nil }

	respond { |method, msg|
		this.performList(method, msg.drop(1))
	}

	start {
		if(isWatching.not) {
			responders.do { |item| item.enable };
			isWatching = true
		}
	}
	stop {
		if(isWatching) {
			responders.do { |item| item.free };
			isWatching = false;
			this.free
		}
	}

}



BasicNodeWatcher : AbstractNodeWatcher {
	var <nodes;

	nodeIsPlaying { |nodeID|
		^nodes.includes(nodeID)
	}
	nodeIsPlaying_ { |nodeID|
		//sometimes it is needed to set this before the server reply
		^nodes.add(nodeID)
	}

	clear {
		nodes = IdentitySet.new
	}

	cmds { ^#["/n_go", "/n_end"] }

	n_end { |nodeID|
		nodes.remove(nodeID)
	}
	n_go { |nodeID|
		nodes.add(nodeID)
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
		ServerBoot.add(this);
		ServerQuit.add(this)
	}

	*cmdPeriod { all.do { |item| item.clear } }

	*doOnServerQuit { |aServer|
		this.doOnServerBoot(aServer)
	}

	*doOnServerBoot { |aServer|
		var serverName = aServer.name;
		var serverNodeWatchers = all.removeAt(serverName);

		if(serverNodeWatchers.notNil) {
			serverNodeWatchers.clear
		}
	}

	*newFrom { |server|
		var res;
		res = all.at(server.name);
		if(res.isNil) {
			res = this.new(server);
			res.start;
			all.put(server.name, res)
		};
		^res
	}

	*register { |node, assumePlaying = false|
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.register(node, assumePlaying)
	}

	*unregister { |node|
		var watcher;
		watcher = this.newFrom(node.server);
		watcher.unregister(node)
	}

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }
	respond { |method, msg|
		var node, group;
		node = nodes.at(msg.at(1));
		if(node.notNil) {
			group = nodes.at(msg.at(2));
			this.performList(method, node, group)
		}
	}

	clear {
		// we must copy 'nodes'
		// b/c a /n_end dependant might add or remove nodes
		// from the collection
		// NEVER iterate over a collection that might change
		nodes.copy.do { |node|
			node.isPlaying = false;
			node.isRunning = false;
			node.changed(\n_end)
		};
		nodes = IdentityDictionary.new
	}

	register { |node, assumePlaying = false|
		if(server.serverRunning.not) { nodes.removeAll; ^this };
		if(isWatching) {
			if(assumePlaying and: { nodes.at(node.nodeID).isNil }) { node.isPlaying = true };
			nodes.put(node.nodeID, node)
		}
	}

	unregister { |node|
		nodes.removeAt(node.nodeID)
	}


	//////////////private implementation//////////////

	n_go { |node|
		node.isPlaying = true;
		node.isRunning = true;
		node.changed(\n_go)  // notify all the node's dependents of the change
	}

	n_end { |node|
		this.unregister(node);
		node.isPlaying = false;
		node.isRunning = false;
		node.changed(\n_end)
	}

	n_off { |node|
		node.isRunning = false;
		node.changed(\n_off)
	}

	n_on { |node|
		node.isRunning = true;
		node.changed(\n_on)
	}

}


DebugNodeWatcher : BasicNodeWatcher {

	cmds { ^#["/n_go", "/n_end", "/n_off", "/n_on"] }

	//////////////private implementation//////////////

	doPost { |action, nodeID, groupID, prevID, nextID|
		Post << ("[ server: " + server.name + "]" +
				action + nodeID + "in group" + groupID + "between nodes" + prevID + "and" + nextID
		) << Char.nl
	}

	n_go { |nodeID, groupID, prevID, nextID|
		this.doPost("started", nodeID, groupID, prevID, nextID);
		nodes.add(nodeID)
	}

	n_end { |nodeID, groupID, prevID, nextID|
		nodes.remove(nodeID);
		this.doPost("ended", nodeID, groupID, prevID, nextID)
	}

	n_off { |nodeID, groupID, prevID, nextID|
		this.doPost("turned off", nodeID, groupID, prevID, nextID)
	}

	n_on { |nodeID, groupID, prevID, nextID|
		this.doPost("turned on", nodeID, prevID, nextID)
	}

}
