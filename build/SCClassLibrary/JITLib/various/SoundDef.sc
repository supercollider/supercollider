
//globally referenceable wrapper for a SynthDef.
//keeps servers updated.

SoundDef {
	var <synthDef, <controlNames;
	classvar <all, <>servers;
	
	*new { arg key, func, rates, prependArgs;
		var synthDef;
		if(func.isNil, { ^all.at(key.asSymbol) });
		synthDef = SynthDef(key, func, rates, prependArgs);
		^super.newCopyArgs(synthDef).toLib(key.asSymbol) //add some preparation later
	}
	
	*initClass { 
		Class.initClassTree(Server);
		all = IdentityDictionary.new;
		servers = [Server.default]
	}
	
	toLib { arg key;
		var old, local;
		this.send;
		this.class.all.put(key, this);
		//cache control names to use with patterns
		controlNames = synthDef.controlNames.collect({ arg item; item.name })
	}
	
	*at { arg key;
		^all.at(key)
	}
	
	*sendAll {
		all.do({ arg item; item.send });
	}
	
	*loadAll {
		all.do({ arg item; item.load });
	}
	
	newMsg { arg args, target, addActionNumber=1;
		^[9,synthDef.name,-1,addActionNumber,target.asTarget.nodeID]++args
	}
	
	asDefName {
		^synthDef.name
	}
		
	send { 	
		this.class.servers.do({ arg server;
			synthDef.send(server);
		})
	}
	
	load { arg server;
		this.class.servers.do({ arg server;
			synthDef.load(server);
		})
	}
	
	play { arg args, target;
		^Synth.new(this, args, target)
	}
	
}