Client {
	classvar <>named;
	var <name, <addr;
	var <>cmdName='/client', <>password, <>verbose=false;

	*new { arg name=\default, addr;
		^super.newCopyArgs(name).minit(addr).add
	}
	
	*initClass {
		named = IdentityDictionary.new;
	}
	
	defaultPort { ^57120 } //this is hardcoded in the sources for now
	defaultAddr { ^[NetAddr("127.0.0.1", 57120)] }
	
	minit { arg argAddr;
		this.addr = argAddr;
	}
	
	add {
		if(named.at(name).notNil, { named.at(name).stop });
		named.put(name, this);
	}

	
	addr_ { arg address;
		if(address.isNil) 
			{ addr =this.defaultAddr }
			{
				addr = address.asArray;
				addr.do({ arg item; item.port_(this.defaultPort) });
			}
	}
	
	prepareSendBundle { arg args;
		args = args.collect { |msg| [cmdName] ++ msg };
		if(args.bundleSize > 65535) { "bundle too large (> 65535)".warn; ^nil };
		^args
	}
	send { arg ... args;
		if (verbose, { ["Client-send:", args].postln });
		this.sendBundle(nil, args);
	}
	sendBundle { arg latency ... args;
		args = this.prepareSendBundle(args);
		if (verbose) { ("Client % sends: %\n").postf(name, args) };
		addr.do({ arg a; a.sendBundle(latency, *args) })
	}
	sendTo { arg index ... args;
		var a;
		args = this.prepareSendBundle(args);
		if (verbose) { ("Client % sends % to: % \n").postf(name, args, index) };
		a = addr[index];
		if(a.notNil) { a.sendBundle(nil, args)  };
	}
		
	interpret { arg string;
		addr.do({ arg a; a.sendBundle(nil, [cmdName, \interpret, password, string]); });
	}
	
	recv { arg name, string;
		if(password.isNil, { "set password to allow interpret".inform; ^this });
		this.interpret("ClientFunc.new(" ++ name ++ string ++ ")");
	}
	remove { named.removeAt(name).stop }

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
		resp = addr.collect { arg netaddr;
				OSCresponderNode(netaddr, cmdName, { arg time, responder, msg;
				var key, func;
				key = msg[1];
				func = ClientFunc.at(key);
				if(verbose) { "LocalClient % received: %\n".postf(name, msg) };
				func.value(msg.drop(2), time, responder);
			});
		};

	}
	
	defaultAddr { ^[nil] } // default: listen to all.
	
	start {
		if(isListening.not, { resp.do { arg u; u.add }; isListening = true });
	}
	
	stop {
		resp.do { arg u; u.remove };
		isListening = false;
	}
	*stop {
		named.do { arg u; u.stop }
	}
	
	
	// a powerful tool for both good and evil
		
	allowInterpret {
		ClientFunc.new(\interpret, { arg pw, string;
			if(password.notNil and: { pw === password }, { { string.asString.interpret}.defer })
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
	*initClass { this.clear }
	*clear { all = IdentityDictionary.new }
	*at { arg key; ^all.at(key) }
	*removeAt { arg key; ^all.removeAt(key) }
	toLib { all.put(name.asSymbol, this) }
	value { arg args;
		func.valueArray(args)
	}
	
	
}

ResponderClientFunc : ClientFunc {
	value { arg args, resp, time;
		func.valueArray(resp, time, args)
	}

}

