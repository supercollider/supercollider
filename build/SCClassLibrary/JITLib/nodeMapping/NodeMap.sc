NodeMap {
	var <values, <mappings, <>msgs;
	var <bundle, <upToDate=false;
	
	
	*new {
		^super.new.clear
	}
	
	clear {
		mappings = IdentityDictionary.new;
		values = IdentityDictionary.new;
	}
	
	addMsg { arg msg;
		msgs = msgs.add(msg);
		upToDate = false;
	}
	
	
	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			mappings.put(args.at(i), args.at(i+1));
		});
		upToDate = false;
	}
	
	unmap { arg ... keys;
		keys.do({ arg key;
			mappings.removeAt(key);
		});
		upToDate = false;
		
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
		upToDate = false;
		
	}

	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			values.put(args.at(i), args.at(i+1));
		});
		upToDate = false;
	}
	
	unset { arg ... keys;
		keys.do({ arg key;
			values.removeAt(key);
		});
		upToDate = false;
		
	}
	
	send { arg server, nodeID, latency;
		var msgBundle;
		msgBundle = List.new;
		this.addToBundle(msgBundle, nodeID);
		server.listSendBundle(latency, msgBundle.postln);
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
										if(value.notNil, {
											mapArgs.add(key); 
											mapArgs.add(value);
										})
				});
				^mapArgs
					
	}
	
	
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
	
	updateBundle { arg nodeID;
			var mapArgs, valArgs, multiValArgs;
			
			if(upToDate, {
				bundle.do({ arg item;
					item.put(1, nodeID);
				});
			}, {
				bundle = List.new;
				mapArgs = this.mapArgs;
				valArgs = this.valArgs;
				multiValArgs = this.multiValArgs;
										
				if(mapArgs.isEmpty.not, {
					bundle.add([14, nodeID]++mapArgs);
				});
				if(valArgs.isEmpty.not, {
					bundle.add([15, nodeID]++valArgs);
				});
				if(multiValArgs.isEmpty.not, {
					bundle.add([16, nodeID]++multiValArgs);
				});
				msgs.do({ arg item;
					bundle.add(item.insert(1, nodeID));
				});
				upToDate = true;
			});

	
	}
	
	addToBundle { arg inBundle, target;
			target = target.asNodeID;
			this.updateBundle(target);
			inBundle.addAll(bundle);
	}	

	
}

