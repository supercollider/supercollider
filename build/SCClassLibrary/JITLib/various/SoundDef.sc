
//globally referenceable wrapper for a SynthDef.

SoundDef {
	var <obj, <synthDef, <servers;
	classvar <all;
	
	*new { arg key, obj;
		if(obj.isNil, { ^all.at(key) });
		^super.newCopyArgs(obj, obj.asSynthDef).toLib(key.asSymbol) //add some preparation later
	}
	
	*initClass {
		all = IdentityDictionary.new;
	}
	
	toLib { arg key;
		var old, local;
		if(synthDef.name.isNil,{ synthDef.name ? "soundDef_" ++ key });
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
	
	newMsg { arg args, target, addActionNumber=1;
		^[9,synthDef.name,-1,addActionNumber,target.asTarget.nodeID]++args
	}
	
	asDefName {
		^synthDef.name
	}
	argNames {
		^obj.eventArgNames
	}
	
	send { arg server;
		if(servers.includes(server).not, {
			servers.add(server);
		});
		synthDef.send(server);
	}
	
	load { arg server;
		if(servers.includes(server).not, {
			servers.add(server);
		});
		synthDef.load(server);
	}
	
	play { arg args, target;
		^Synth.new(this, args, target)
	}
	
}