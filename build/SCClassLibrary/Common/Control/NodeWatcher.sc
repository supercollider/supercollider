NodeWatcher {
	var <nodes, <server, <responders;
	var <isListening=false;
	
	*new { arg server;
		^super.new.ninit(server)
	}
	ninit { arg s;
		var cmds;
		server = s;
		
		this.clear;
		cmds = #['/n_go', '/n_end', '/n_off', '/n_on', '/n_move'];		responders = cmds.collect({ arg cmd;
				var method;
				method = cmd.asString.copyToEnd(1).asSymbol;
				OSCresponder(server.addr, cmd, 
					{ arg time, resp, msg;
						//msg.postln;
						msg.removeAt(0);
						msg = this.lookUp(msg);
						if(msg.notNil, {
							this.performList(method, msg)
						})
						
					})
				});
	}

	lookUp { arg msg;
		var res;
		res = msg.collect({ arg nodeID; nodes.at(nodeID) });
		^if(res.at(0).notNil, { res }, { nil });
		//todo: remote creation scheme
	}
	
	startListen {
		server.notify(true);
		if(isListening.not, {
			responders.do({ arg item; item.add });
			isListening = true;
		})
	}
	stopListen {
		if(isListening, {
			responders.do({ arg item; item.remove });
			this.clear;
			isListening = false;
		})
	}
	
	register { arg node;
		if(isListening, {
			nodes.put(node.nodeID, node);
		});
	}
	
	unregister { arg node;
		nodes.removeAt(node.nodeID);
		node.freeNodeID;
	}
	
	clear { 
		nodes = IdentityDictionary.new;
		
	}
	
	//////////////private implementation//////////////
	
	n_go { arg node, group, prev, next;
		//group is set in the prMove calls
		if(prev.notNil, {
			node.prMoveAfter(prev);
		}, {
			if(next.notNil, {
				node.prMoveBefore(next);
			}, {
				group.prAddTail(node); 
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
		//(node.asString ++ " was freed").postln;

		this.unregister(node);
				
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