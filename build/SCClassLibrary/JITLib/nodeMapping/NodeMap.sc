NodeMap {
	var <values, <mappings, <>msgs;
	var <currentMsg;
	
	*new {
		^super.new.clear
	}
	
	clear {
		mappings = IdentityDictionary.new;
		values = IdentityDictionary.new;
		currentMsg = List.new;
	}
	
	addMsg { arg msg;
		if(msgs.isNil, { this.clearMsgs });
		msgs.add(msg);
	}
	
	clearMsgs {
		msgs = List.new;
	}
	
	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			mappings.put(args.at(i).asSymbol, args.at(i+1));
		});
	}
	
	unmap { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			var key, oldval;
			key = args.at(i).asSymbol;
			mappings.removeAt(key);
			//mappings.put(key, -1); //what is better?
			oldval = values.at(key);
			if(oldval.notNil, {
				this.set(key, oldval) 
			})
		});
		
	}
	
//	get { arg target ... args;
//		var resp, server;
//		server = target.asTarget.server;
//		if(args.isEmpty, { args = mappings.keys });
//		resp = OSCresponder(server.addr, '/c_set', { arg ... msg;
//			msg.postln;
//			resp.remove;
//		}).add;
//		server.sendBundle(nil, ["/c_get"]++args);
//		
//	}
	
	setn { arg ... args;
		this.performList(\set, args);
		
	}

	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			values.put(args.at(i).asSymbol, args.at(i+1));
		});
	}
	
	unset { arg ... keys;
		keys.do({ arg key;
			values.removeAt(key);
		});
		
	}
	
	send { arg server, nodeID, latency;
		var bundle;
		bundle = List.new;
		this.addToBundle(bundle, nodeID);
		server.listSendBundle(latency, bundle);
	}
	
	sendToNode { arg node, latency;
		node = node.asTarget;
		this.send(node.server, node.nodeID, latency)
	}
	
	mapArgs {	
				var mapArgs;
				if(mappings.isEmpty, { ^#[] });
				mapArgs = List.new;
				mappings.keysValuesDo({ arg key, value;
										value = this.translate(value);
										if(value.notNil, {
											mapArgs.add(key); 
											mapArgs.add(value);
										})
				});
				^mapArgs
					
	}
	
	translate { arg val; ^val }
	
	valArgs {
				var valArgs;
				if(values.isEmpty, { ^#[] });
				valArgs = List.new;
				values.keysValuesDo({ arg key, value;
								if(value.isSequenceableCollection.not, {
									if(mappings.at(key).isNil, {
										valArgs.add(key); 
										valArgs.add(value);
									});
								});
				});
				^valArgs

	}
	multiValArgs {
				var valArgs;
				valArgs = List.new;
				if(values.isEmpty, { ^#[] });
				values.keysValuesDo({ arg key, value;
								if(value.isSequenceableCollection, {
									if(mappings.at(key).isNil, {
										valArgs.add(key); 
										valArgs.add(value.size);
										valArgs.addAll(value);
									});
								});
				});
				^valArgs
	}
	
	
	addToBundle { arg msgList, target;
		var mapArgs, valArgs, multiValArgs;
			target = target.asNodeID;
				mapArgs = this.mapArgs;
				valArgs = this.valArgs;
				multiValArgs = this.multiValArgs;
										
				if(mapArgs.isEmpty.not, {
					msgList.add([14, target]++mapArgs);
				});
				if(valArgs.isEmpty.not, {
					msgList.add([15, target]++valArgs);
				});
				if(multiValArgs.isEmpty.not, {
					msgList.add([16, target]++multiValArgs);
				});
				
				msgs.do({ arg item;
					msgList.add(item.insert(1, target));
				});
	}

	
}

BusNodeMap : NodeMap {

		translate { arg bus; ^bus.index }

}

