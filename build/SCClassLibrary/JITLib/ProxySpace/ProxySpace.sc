ProxySpace : EnvironmentRedirect {	classvar <>lastEdited;//undo support
		var <group, <server, <>clock;
	var <>defaultNumChannels=2; //default values for numChannels 
		*new { arg target, clock;
		^super.new.einit(target, clock)
	}
		*pop { 		if(this.inside, {			currentEnvironment = currentEnvironment.saveEnvir;		}, { "¥ ProxySpace is inactive already".postln })	}	*push { arg target, clock;
		^super.push.einit(target, clock);
	}
	
	//todo add group to target	einit { arg target,argClock; 
		server = target.asTarget.server;  
		clock = argClock;
	}
		at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			proxy.clock = clock;
			this.prPut(key, proxy);
		});
		^proxy
		}
	
	put { arg key, obj;
		var proxy;
		proxy = this.prAt(key);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			proxy.clock = clock;
			this.prPut(key, proxy);
		}, {
			if(obj.isNil, { this.removeAt(key);  });
			
		});
		proxy.setObj(obj, true, true, false, clock);
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
	release {
		this.do({ arg proxy; proxy.release });
	}
			
	*undo {		lastEdited.tryPerform(\undo)	}
	postln {
	 "______".post;
	 lastEdited.postln;
	}
		}