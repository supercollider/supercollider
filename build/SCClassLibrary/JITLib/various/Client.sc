Client {
	classvar <>named;
	var <name, <addr;
	var <>password;

	*new { arg name=\default, addr;
		^super.newCopyArgs(name).minit(addr).add
	}
	
	*initClass {
		named = IdentityDictionary.new;
	}
	
	defaultPort { ^57120 } //this is hardcoded in the sources for now
	defaultAddr { ^NetAddr("127.0.0.1", 57120) }
	
	minit { arg argAddr;
		this.addr = argAddr;
	}
	
	add {
		if(named.at(name).notNil, { named.at(name).stop });
		named.put(name, this);
	}

	
	addr_ { arg address;
		addr = address ?? {Êthis.defaultAddr };
		addr = addr.asCollection;
		addr.do({ arg item; item.port_(this.defaultPort) });
	}
	
	prepareSendBundle { arg args;
		args = ["/client"]++args;
		if(this.checkBundleSize(args) > 8192) { "bundle too large (> 8192)".warn; ^nil };
		^args
	}
	send { arg ... args;
		args = this.prepareSendBundle(args);
		addr.do({ arg a; a.sendBundle(nil, args) })
	}
	sendTo { arg index ... args;		var a;		args = this.prepareSendBundle(args);		a = addr[index];		if(a.notNil) { a.sendBundle(nil, args)  };	}
	checkBundleSize { arg bundle;
		var size=0, dict;
		
		dict = (\String: { arg x; x.size }, \Integer: 4, \Float: 4);
		bundle.do({ arg el;
			size = size + (dict.at(el.class.name).value(el) ? 0);
		});
		^size
	}
	
	interpret { arg string;
		addr.do({ arg a; a.sendBundle(nil, ["/client", \interpret, password, string]); });
	}
	
	recv { arg name, string;
		if(password.isNil, { "set password to allow interpret".inform; ^this });
		this.interpret("ClientFunc.new(" ++ name ++ string ++ ")");
	}
	

}

LocalClient : Client {
	
	classvar <>named, >default;
	var <resp, <isListening=false;
	
	*default { ^default ?? { default = this.new } }
	
	*initClass {
		named = IdentityDictionary.new;
	}
	
	add {
		if(named.at(name).notNil, { named.at(name).stop });
		named.put(name, this);
	}
	
	minit { arg argAddr;
		super.minit(argAddr);
		resp = OSCresponderNode(addr.first, '/client', { arg time, responder, msg;
			var key, func;
			key = msg[1];
			func = ClientFunc.at(key);
			func.valueArray(msg.drop(2));
		});

	}
	
	defaultAddr { ^nil } // default: listen to all.
	
	start {
		if(isListening.not, { resp.add; isListening = true });
	}
	
	stop {
		resp.remove;
		isListening = false;
	}
	
	// a powerful tool for both good and evil
		
	allowInterpret {
		ClientFunc.new(\interpret, { arg pw, string;
			if(password.notNil and: { pw === password }, { {string.asString.interpret}.defer })
		});
	}
	
	disallow {
		ClientFunc.removeAt(\interpret);
	}
}


ClientFunc {
	classvar <>all;
	var <name, <func;
	
	*new { arg name, func;
		^super.newCopyArgs(name, func).toLib;
	}
	*initClass {Êthis.clear }
	*clear { all = IdentityDictionary.new }
	*at { arg key; ^all.at(key) }
	*removeAt { arg key; ^all.removeAt(key) }
	toLib { all.put(name.asSymbol, this) }
	value { arg ... args; func.valueArray(args) }
	valueArray { arg args; func.valueArray(args) }
	
	
}

