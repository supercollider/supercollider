ProxySpace : EnvironmentRedirect {	classvar <>lastEdited;//undo support
	classvar <>all; //access
		var <group, <server, <>clock;
	var <>defaultNumChannels=2; //default values for numChannels 
	var <name;
	
	*initClass {
		all = IdentitySet.new;
	}
		*new { arg server, name, clock;
		^super.new.einit(server, name, clock)
	}
		*pop { 		if(this.inside, {			currentEnvironment = currentEnvironment.saveEnvir;		}, { "¥ ProxySpace is inactive already".postln })	}	*push { arg server, name, clock;
		^super.push.einit(server, name, clock);
	}
	
	//todo add group to target	einit { arg srv,argName, argClock; 
		server = srv;  
		clock = argClock;
		name = argName.asSymbol;
		this.class.all.add(this);
	}
	
	makeProxy { arg key;
			var proxy;
			proxy = NodeProxy(server);
			proxy.clock = clock;
			this.prPut(key, proxy);
			^proxy
	}
		at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		});
		^proxy
		}
	
	put { arg key, obj;
		var proxy;
		proxy = this.prAt(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		}, {
			if(obj.isNil, { 
				proxy.clear; 
				this.removeAt(key);  
			});
			
		});
		proxy.put(obj, 0, true, true);
		this.class.lastEdited = proxy;
	}
	
	
	play { arg key=\out;
		this.at(key).play;
	}
	
	record { arg key, path, headerFormat="aif", sampleFormat="int16";
		^this.at(key).record(path, headerFormat="aif", sampleFormat);
	}
	
	free {
		this.do({ arg proxy; proxy.free });
	}
	clear {
		this.do({ arg proxy; proxy.clear });
	}
	release {
		this.do({ arg proxy; proxy.release });
	}
		*clearAll {
		all.do({ arg item; item.clear });
	}	
	*undo {		lastEdited.tryPerform(\undo)	}
	
		}