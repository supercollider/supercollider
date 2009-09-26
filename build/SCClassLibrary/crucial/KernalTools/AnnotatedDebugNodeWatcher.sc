
AnnotatedDebugNodeWatcher : DebugNodeWatcher {

	getAnnotation { arg server, nodeID;
		var a;
		a = Library.at(AbstractPlayer, server, nodeID);
		if(a.notNil,{ ^"("++a++")" },{ ^"" });
	}

	doPost { arg action, nodeID, groupID, prevID, nextID;
		Post << (server.name.asString + ":" +
		action + nodeID + this.getAnnotation(server, nodeID) + Char.nl
		+ "         " + "group:" + groupID + this.getAnnotation(server,groupID)
		/*+ "[" + prevID + this.getAnnotation(server,prevID) + "<->" + nextID + this.getAnnotation(server,nextID) + "]"*/
		) << Char.nl
	}

	n_go { arg nodeID, groupID, prevID, nextID;
		this.doPost("GO ", nodeID, groupID, prevID, nextID);
		nodes.add(nodeID);
	}

	n_end { arg nodeID, groupID, prevID, nextID;
		nodes.remove(nodeID);
		this.doPost("END", nodeID, groupID, prevID, nextID);
	}

	n_off { arg nodeID, groupID, prevID, nextID;
		this.doPost("OFF", nodeID, groupID, prevID, nextID);
	}

	n_on { arg nodeID, groupID, prevID, nextID;
		this.doPost("ON ", nodeID, prevID, nextID);	}
}