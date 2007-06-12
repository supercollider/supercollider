//referenceable wrapper for a SynthDef.

SoundDef {
	var <obj, <synthDef, <servers;
	classvar <all;
	
	*new { arg key, obj;
		if(obj.isNil, { ^all.at(key) });
		^super.newCopyArgs(obj, obj.prepareForPlay.asSynthDef).toLib(key.asSymbol)
	}
	
	*initClass {
		all = IdentityDictionary.new;
	}
	
	toLib { arg key;
		var old, local;
		synthDef.name = "soundDef_" ++ key;
		servers = IdentitySet.new;
		local = Server.local;
		old = this.class.at(key);
		if(old.notNil, {
			//keep old servers updated
			old.servers.do({ arg item; this.send(item) });
		}, {
			//send to local by default
			if(local.serverRunning, {this.send(local)}, {this.load(local)});
		});
		this.class.all.put(key,this);
		 
	}
	
	*at { arg key;
		^all.at(key)
	}
	
	
	asDefName {
		^synthDef.name
	}
	argNames {
		^obj.eventArgNames
	}
	
	send { arg server;
		if(servers.includes(server).not, {
			synthDef.send(server);
			servers.add(server);
		})
	}
	
	load { arg server;
		if(servers.includes(server).not, {
			synthDef.load(server);
			servers.add(server);
		})
	}
	
	play { arg args, target;
		^Synth.new(this, args, target)
	}
	
}