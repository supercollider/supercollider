ProxySpace : EnvironmentRedirect {	classvar <>lastEdited;//undo support
	classvar <>all; //access
		var <group, <server, <>clock;
	var <name;
	
	*initClass {
		all = IdentitySet.new;
	}
		*new { arg server, name, clock;
		^super.new.einit(server, name, clock)
	}
			*push { arg server, name, clock;
		^this.new(server, name, clock).push
	}
	
	
	//todo add group to target	einit { arg srv, argName, argClock; 
		server = srv;  
		clock = argClock;
		name = argName.asSymbol;
		if(name.notNil, { this.class.all.add(this) });
	}
	
	makeProxy { arg key;
			var proxy;
			proxy = NodeProxy(server);
			proxy.clock = clock;
			envir.put(key, proxy);
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
		proxy = envir.at(key);
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
	
	
	play { arg key=\out, busIndex=0, nChan=2;
		this.at(key).play(busIndex, nChan);
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
	remove { all.remove(this) }
		*clearAll {
		all.do({ arg item; item.clear });
	}	
	*undo {		lastEdited.tryPerform(\undo)	}
	
	printOn { arg stream; 
		stream << "ProxySpace: ";
		this.keysValuesDo({ arg key, item; 
			stream <<< key << " " 
			<< if(item.rate==='audio', { "ar" }, { "kr" }) 
			<< "(" << item.numChannels << ")   "
		});
		stream << Char.nl
		
	}
		}

SharedProxySpace  : ProxySpace {
	

	einit { arg srv, argName, argClock, controlKeys, audioKeys;
		super.einit(srv,argName, argClock); 
		//default:
		//initialize single letters as shared busses: xyz are audio busses, the rest control
		controlKeys = controlKeys ?? { Array.fill(25 - 3, { arg i; asSymbol(asAscii(97 + i)) }) };
		audioKeys = audioKeys ?? { Array.fill(3, { arg i; asSymbol(asAscii(97 + 25 - 3 + i)) }) };
		
		controlKeys.do({ arg key; this.makeSharedProxy(key, 'control') });
		audioKeys.do({ arg key; this.makeSharedProxy(key, 'audio') });
	}
	
	makeSharedProxy { arg key, rate;
			var proxy, srv;
			srv = server.broadcast;
			proxy = if(rate.isNil, {
				SharedNodeProxy(srv)
			},{
				SharedNodeProxy.perform(rate,srv)
			});
			proxy.clock = clock;
			envir.put(key, proxy);
			^proxy
	}
	


}