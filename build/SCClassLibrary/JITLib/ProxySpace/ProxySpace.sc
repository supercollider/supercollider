ProxySpace : EnvironmentRedirect {	classvar <>lastEdited;//undo support
		var <group, <server;
	var <>defaultNumChannels=2; //default values for numChannels 
		*new { arg target;
		^super.new.einit(target)
	}	*pop { 		if(this.inside, {			currentEnvironment = currentEnvironment.saveEnvir;		}, { "¥ ProxySpace is inactive already".postln })	}	*push { arg server;
		^super.push.einit(server);
	}
	
	//todo add group to target	einit { arg target; server = target.asTarget.server;  }
		at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			this.prPut(key, proxy);
		});
		^proxy
		}
	play { arg key=\out;
		this.at(key).play;
	}
	record { arg key, path, headerFormat="aif", sampleFormat="int16";
		var rec;
		rec = RecNodeProxy.newFrom(this.at(key));
		Routine({
			1.0.wait;
			rec.record(path, headerFormat, sampleFormat);
		}).play;
		^rec 
	}
	free {
		this.do({ arg proxy; proxy.free });
	}
	release {
		this.do({ arg proxy; proxy.release });
	}
		put { arg key, obj;		var proxy;
		proxy = this.prAt(key);
		if(proxy.isNil, {
			proxy = NodeProxy(server);
			this.prPut(key, proxy);
		}, {
			if(obj.isNil, { this.removeAt(key);  });
			
		});
		proxy.setObj(obj, true);
		this.class.lastEdited = proxy;	}
	
		
	*undo {		lastEdited.tryPerform(\undo)	}
		}