ProxySpace : EnvironmentRedirect {	classvar <>lastEdited;//undo support
		var <group, <server;
	var <>defaultNumChannels=2; //default values for numChannels 
		*new { arg target;
		^super.new.einit(target)
	}	*pop { 		if(this.inside, {			currentEnvironment = currentEnvironment.saveEnvir;		}, { "¥ ProxySpace is inactive already".postln })	}	*push { arg server;
		^super.push.einit(server);
	}
	
	//todo add group to target	einit { arg target; server = target.asTarget.server; group = target }
		at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(nil, key);
			this.prPut(key, proxy);
		});
		^proxy
		}
	play { arg key=\out;
		this.at(key).play;
	}
		
		put { arg key, obj;		var proxy;
		proxy = this.prAt(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(obj, key);
			this.prPut(key, proxy);
		}, {
			if(obj.isNil, { this.removeAt(key); proxy.free });
			
		});
		proxy.setObj(obj, true);
		this.class.lastEdited = proxy;	}
	
		
	*undo {		lastEdited.tryPerform(\undo)	}
		///private///
	
	makeProxy { arg obj, key;
		var rate, array, n;
		^if(obj.notNil, {
			
			array = obj.value.asArray;
			array.do({ arg item; rate = item.tryPerform(\rate).postln; });
			if(rate === 'audio', {
				AudioProxy
			}, {
				ControlProxy
			}).new(server, array.size, key)
		}, {
			//ControlProxy.new(server, defaultNumChannels, key)
			NeutralProxy(this, key, defaultNumChannels);
		})
		
	}
	}