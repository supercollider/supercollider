NodeMap {
	var <values, <mappings, <>msgs;
	
	*new {
		^super.new.clear
	}
	
	clear {
		mappings = IdentityDictionary.new;
		values = IdentityDictionary.new;
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
			//mappings.removeAt(key);
			mappings.put(key, -1); //what is better?
			oldval = values.at(key);
			if(oldval.notNil, {
				this.set(key, oldval) 
			})
		});
		
	}
	
	get { arg target ... args;
		var resp, server;
		server = target.asTarget.server;
		if(args.isEmpty, { args = mappings.keys });
		resp = OSCresponder(server.addr, '/c_set', { arg ... msg;
			msg.postln;
			resp.remove;
		}).add;
		server.sendBundle(nil, ["/c_get"]++args);
		
	}
	
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
	
	send { arg targetNode, latency;
		var msg;
		msg = List.new;
		targetNode = targetNode.asTarget;
		this.updateMsg(msg, targetNode);
		targetNode.server.listSendBundle(latency, msg);
	}
	
	mapArgs {	
				var mapArgs;
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
	
	updateMsg { arg msgList, target;
		var mapArgs, valArgs, multiValArgs;
			target = target.asTarget;
				mapArgs = this.mapArgs;
				valArgs = this.valArgs;
				multiValArgs = this.multiValArgs;
										
				if(mapArgs.isEmpty.not, {
					msgList.add(target.getMsg(14, mapArgs));
				});
				if(valArgs.isEmpty.not, {
					msgList.add(target.getMsg(15, valArgs));
				});
				if(multiValArgs.isEmpty.not, {
					msgList.add(target.getMsg(16, multiValArgs));
				});
				
				msgs.do({ arg item;
					msgList.add(
						target.getMsg(item.first, item.copyToEnd(1))
					);
				});
	}

	
}

BusNodeMap : NodeMap {

		translate { arg bus; ^bus.index }

}

