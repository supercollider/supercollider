NodeMap {
	var <values, <mappings, <>commands;
	
	*new {
		^super.new.clear
	}
	
	clear {
		mappings = IdentityDictionary.new;
		values = IdentityDictionary.new;
	}
	
	addCommand { arg cmd;
		if(commands.isNil, { this.clearCommands });
		commands.add(cmd);
	}
	
	clearCommands {
		commands = List.new;
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
		var cmd;
		cmd = List.new;
		targetNode = targetNode.asTarget;
		this.updateCommand(cmd, targetNode);
		targetNode.server.sendCmdList(cmd, latency);
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
	
	updateCommand { arg cmdList, target;
		var mapArgs, valArgs, multiValArgs;
			target = target.asTarget;
				mapArgs = this.mapArgs;
				valArgs = this.valArgs;
				multiValArgs = this.multiValArgs;
										
				if(mapArgs.isEmpty.not, {
					target.addCommand(cmdList, 14, mapArgs);
				});
				if(valArgs.isEmpty.not, {
					target.addCommand(cmdList, 15, valArgs);
				});
				if(multiValArgs.isEmpty.not, {
					target.addCommand(cmdList, 16, multiValArgs);
				});
				
				commands.do({ arg item;
					target.addCommand(cmdList, item.first, item.copyToEnd(1)) 
				});
	}

	
}

BusNodeMap : NodeMap {

		translate { arg bus; ^bus.index }

}

