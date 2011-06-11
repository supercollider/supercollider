AbstractResponderProxy {
	classvar allProxies;
	var <func, <srcID, <enabled = false, <dispatcher, <permanent = false;
	
	*initClass { Class.initClassTree(AbstractDispatcher); allProxies = IdentitySet.new; }
	
	enable { 
		if(enabled.not, {
			if(permanent.not, { CmdPeriod.add(this) });
			dispatcher.add(this);
			enabled = true;
			allProxies.add(this);
		});
	} 
	
	disable { 
		if(permanent.not, { CmdPeriod.remove(this) });
		dispatcher.remove(this);
		enabled = false;
	}
	
	func_ {|newFunc|  
		func = newFunc; 
		this.changed(\function);
	}
	
	gui { this.subclassResponsibility(thisMethod) }
	
	cmdPeriod { this.free }
	
	oneShot {
		var oneShotFunc, wrappedFunc;
		wrappedFunc = func;
		oneShotFunc = { arg ...args; wrappedFunc.value(*args); this.free };
		this.func_(oneShotFunc);
	}
	
	permanent_{|bool| 
		permanent = bool;
		if(bool && enabled, { CmdPeriod.remove(this) }, {CmdPeriod.add(this) })
	}
	
	fix { this.permanent_(true) }
	
	free { allProxies.remove(this); this.disable }
	
	*allProxies { 
		var result;
		result = IdentityDictionary.new;
		allProxies.do({|proxy|
			var key;
			key = proxy.dispatcher.typeKey;
			result[key] = result[key].add(proxy)
		});
		^result;
	}
	
	*allEnabled { 
		var result;
		result = IdentityDictionary.new;
		allProxies.select(_.enabled).do({|proxy|
			var key;
			key = proxy.dispatcher.typeKey;
			result[key] = result[key].add(proxy)
		});
		^result;	
	}
	
	*allDisabled { 
		var result;
		result = IdentityDictionary.new;
		allProxies.reject(_.enabled).do({|proxy|
			var key;
			key = proxy.dispatcher.typeKey;
			result[key] = result[key].add(proxy)
		});
		^result;	
	}
	
}

// defines the required interface
AbstractDispatcher {
	classvar <>all;
	var registered = false;
	
	*new { ^super.new.init; }
	
	init { all.add(this);}
		
	*initClass { all = IdentitySet.new }
	
	add {|proxy| this.subclassResponsibility(thisMethod) } // proxies call this to add themselves to this dispatcher; should register this if needed
	
	remove {|proxy| this.subclassResponsibility(thisMethod) } // proxies call this to remove themselves from this dispatcher; should unregister if needed
		
	value { this.subclassResponsibility(thisMethod) }
	
	valueArray {arg args; ^this.value(*args) } // needed to work in FunctionLists
	
	register { this.subclassResponsibility(thisMethod) } // register this dispatcher to listen for its message type
	
	unregister { this.subclassResponsibility(thisMethod) } // unregister this dispatcher so it no longer listens
	
	free { this.unregister; all.remove(this) } // I'm done
	
	typeKey { this.subclassResponsibility(thisMethod) } // a Symbol
	
	update { } // code here to update any changed state in this dispatcher's proxies, e.g. a new function; default does nothing

}

// basis for the default dispatchers
// uses function wrappers for matching
AbstractWrappingDispatcher :  AbstractDispatcher {
	var active, <wrappedFuncs;
	
	init { super.init; active = IdentityDictionary.new; wrappedFuncs = IdentityDictionary.new; }
	
	add {|proxy| 
		var func, keys;
		proxy.addDependant(this);
		func = this.wrapFunc(proxy);
		wrappedFuncs[proxy] = func;
		keys = this.getKeysForProxy(proxy);
		keys.do({|key| active[key] = active[key].addFunc(func) }); // support multiple keys
		if(registered.not, {this.register});
	}
	
	remove {|proxy|
		var func, keys;
		proxy.removeDependant(this);
		keys = this.getKeysForProxy(proxy);
		func = wrappedFuncs[proxy];
		keys.do({|key| active[key] = active[key].removeFunc(func) }); // support multiple keys
		wrappedFuncs[proxy] = nil;
		if(active.size == 0, {this.unregister});
	}
	
		// old Funk vs. new Funk
	updateFuncForProxy {|proxy|
		var func, oldFunc, keys;
		func = this.wrapFunc(proxy);
		oldFunc = wrappedFuncs[proxy];
		wrappedFuncs[proxy] = func;
		keys = this.getKeysForProxy(proxy);
		keys.do({|key| active[key] = active[key].replaceFunc(oldFunc, func) }); // support multiple keys
	}
		
	wrapFunc { this.subclassResponsibility(thisMethod) }
	
	getKeysForProxy { this.subclassResponsibility(thisMethod) }
	
	update {|proxy, what| if(what == \function, { this.updateFuncForProxy(proxy) }) }
	
	free { wrappedFuncs.keys.do({|proxy| proxy.removeDependant(this) }); super.free }
	
}

// The default dispatchers below store by the 'most significant' message argument for fast lookup
// These are for use when more than just the 'most significant' argument needs to be matched
AbstractMessageMatcher {
	var <>func;
	
	value { this.subclassResponsibility(thisMethod) }
	
	valueArray {arg args; ^this.value(*args) } // needed to work in FunctionLists

}

///////////////////// OSC

OSCMessageDispatcher : AbstractWrappingDispatcher {
	
	wrapFunc {|proxy|
		var func, srcID, recvPort;
		func = proxy.func;
		srcID = proxy.srcID;
		recvPort = proxy.recvPort;
		^case(
			{ srcID.notNil && recvPort.notNil }, { OSCProxyBothMessageMatcher(srcID, recvPort, func) },
			{ srcID.notNil }, { OSCProxyAddrMessageMatcher(srcID, func) },
			{ recvPort.notNil }, { OSCProxyRecvPortMessageMatcher(recvPort, func) },
			{ func }
		);
	}
	
	getKeysForProxy {|proxy| ^[proxy.path];}
	
	value {|time, addr, recvPort, msg| active[msg[0]].value(msg, time, addr, recvPort);}
	
	register { 
		thisProcess.recvOSCfunc = thisProcess.recvOSCfunc.addFunc(this); 
		registered = true; 
	}
	
	unregister { 
		thisProcess.recvOSCfunc = thisProcess.recvOSCfunc.removeFunc(this);
		registered = false;
	}
	
	typeKey { ^('OSC unmatched').asSymbol }
	
}

OSCMessagePatternDispatcher : OSCMessageDispatcher {
	
	value {|time, addr, recvPort, msg| 
		var pattern;
		pattern = msg[0];
		active.keysValuesDo({|key, func|
			if(key.matchOSCAddressPattern(pattern), {func.value(msg, time, addr, recvPort);});
		})
	}
	
	typeKey { ^('OSC matched').asSymbol }
	
}

OSCProxy : AbstractResponderProxy {
	classvar <>defaultDispatcher, <>defaultMatchingDispatcher, traceFunc, traceRunning = false;
	var <path, <recvPort;
	
	*initClass {
		defaultDispatcher = OSCMessageDispatcher.new;
		defaultMatchingDispatcher = OSCMessagePatternDispatcher.new;
		traceFunc = {|time, replyAddr, recvPort, msg|
			"OSC Message Received:\n\ttime: %\n\taddress: %\n\trecvPort: %\n\tmsg: %\n\n".postf(time, replyAddr, recvPort, msg);
		}
	}
	
	*new { arg func, path, srcID, recvPort, dispatcher;
		^super.new.init(func, path, srcID, recvPort, dispatcher ? defaultDispatcher);
	}
	
	*newMatching { arg func, path, srcID, recvPort;
		^super.new.init(func, path, srcID, recvPort, defaultMatchingDispatcher);
	}
	
	*trace {|bool = true| 
		if(bool, {
			if(traceRunning.not, {
				thisProcess.addOSCFunc(traceFunc);
				CmdPeriod.add(this);
				traceRunning = true;
			});
		}, {
			thisProcess.removeOSCFunc(traceFunc);
			CmdPeriod.remove(this);
			traceRunning = false;
		});
	}
	
	*cmdPeriod { this.trace(false) }
	
	init {|argfunc, argpath, argsrcID, argrecvPort, argdisp|
		path = (argpath ? path).asString;
		if(path[0] != $/, {path = "/" ++ path}); // demand OSC compliant paths
		path = path.asSymbol;
		srcID = argsrcID ? srcID;
		recvPort = argrecvPort ? recvPort;
		func = argfunc;
		dispatcher = argdisp ? dispatcher;
		this.enable;
		allProxies.add(this);
	}
	
	printOn { arg stream; stream << this.class.name << "(" <<* [path, srcID] << ")" }
	
}

OSCdef : OSCProxy {
	classvar <all; 
	var <key;
	
	*initClass {
		all = IdentityDictionary.new;
	}
	
	*new { arg key, func, path, srcID, recvPort, dispatcher;
		var res = all.at(key);
		if(res.isNil) {
			^super.new(func, path, srcID, recvPort, dispatcher).addToAll(key);
		} {
			if(func.notNil) { 
				if(res.enabled, {
					res.disable;
					res.init(func, path, srcID, recvPort, dispatcher);
				}, { res.init(func, path, srcID, recvPort, dispatcher).disable; });
			}
		}
		^res
	}
	
	addToAll {|argkey| key = argkey; all.put(key, this) }
	
	free { all[key] = nil; super.free; }
	
}


// if you need to test for address func gets wrapped in this
OSCProxyAddrMessageMatcher : AbstractMessageMatcher {
	var addr;
	
	*new {|addr, func| ^super.new.init(addr, func);}
	
	init {|argaddr, argfunc| addr = argaddr; func = argfunc; }
	
	value {|msg, time, testAddr, recvPort| 
		if(testAddr.addr == addr.addr and: {addr.port.matchItem(testAddr.port)}, {
			func.value(msg, time, testAddr, recvPort)
		})
	}
}

// if you need to test for recvPort func gets wrapped in this
OSCProxyRecvPortMessageMatcher : AbstractMessageMatcher {
	var recvPort;
	
	*new {|recvPort, func| ^super.new.init(recvPort, func);}
	
	init {|argrecvPort, argfunc| recvPort = argrecvPort; func = argfunc; }
	
	value {|msg, time, addr, testRecvPort| 
		if(testRecvPort == recvPort, {
			func.value(msg, time, addr, testRecvPort)
		})
	}
}

OSCProxyBothMessageMatcher : AbstractMessageMatcher {
	var addr, recvPort;
	
	*new {|addr, recvPort, func| ^super.new.init(addr, recvPort, func);}
	
	init {|argaddr, argrecvPort, argfunc| addr = argaddr; recvPort = argrecvPort; func = argfunc; }
	
	value {|msg, time, testAddr, testRecvPort| 
		if(testAddr.addr == addr.addr and: {addr.port.matchItem(testAddr.port)} and: {testRecvPort == recvPort}, {
			func.value(msg, time, testAddr, testRecvPort)
		})
	}
}

///////////////////// MIDI

// for \noteOn, \noteOff, \control, \polytouch
MIDIMessageDispatcher : AbstractWrappingDispatcher {
	var <>messageType;
	
	*new {|messageType| ^super.new.messageType_(messageType) }
	
	getKeysForProxy {|proxy| ^(proxy.msgNum ? (0..127)).asArray;} // noteNum, etc.
	
	value {|src, chan, num, val| active[num].value(val, num, chan, src);}
	
	register { 
		MIDIIn.perform(messageType.asSetter, MIDIIn.perform(messageType.asGetter).addFunc(this)); 
		registered = true; 
	}
	
	unregister { 
		MIDIIn.perform(messageType.asSetter, MIDIIn.perform(messageType.asGetter).removeFunc(this));
		registered = false;
	}
	
	// wrapper objects based on arg type and testing requirements
	wrapFunc {|proxy|
		var func, chan, srcID;
		func = proxy.func;
		chan = proxy.chan;
		srcID = proxy.srcID;
		^case(
			{ srcID.notNil && chan.isArray }, {MIDIProxyBothCAMessageMatcher(chan, srcID, func)},
			{ srcID.notNil && chan.notNil }, {MIDIProxyBothMessageMatcher(chan, srcID, func)},
			{ srcID.notNil }, {MIDIProxySrcMessageMatcher(srcID, func)},
			{ chan.isArray }, {MIDIProxyChanArrayMessageMatcher(chan, func)},
			{ chan.notNil }, {MIDIProxyChanMessageMatcher(chan, func)},
			{ func }
		);
	}
	
	typeKey { ^('MIDI ' ++ messageType).asSymbol }	
}

// for \touch, \program, \bend
MIDIMessageDispatcherNV : MIDIMessageDispatcher {
	
	getKeysForProxy {|proxy| ^(proxy.chan ? (0..15)).asArray;} // chan
	
	value {|src, chan, val| active[chan].value(val, chan, src);}
	
	// wrapper objects based on arg type and testing requirements
	wrapFunc {|proxy|
		var func, chan, srcID;
		func = proxy.func;
		chan = proxy.chan;
		srcID = proxy.srcID;
		// are these right?
		^case(
			{ srcID.notNil }, {MIDIProxySrcMessageMatcherNV(srcID, func)},
			{ func }
		);
	}
}


MIDIProxy : AbstractResponderProxy {
	classvar <>defaultDispatchers;
	var <chan, <msgNum, <msgVal, <msgType;
	
	*initClass {
		defaultDispatchers = IdentityDictionary.new;
		[\noteOn, \noteOff, \control, \polytouch].do({|type|
			defaultDispatchers[type] = MIDIMessageDispatcher(type);
		});
		[\touch, \program, \bend].do({|type|
			defaultDispatchers[type] = MIDIMessageDispatcherNV(type);
		});
	}
	
	*new { arg func, msgNum, chan, msgType, srcID, dispatcher;
		^super.new.init(func, msgNum, chan, msgType, srcID, dispatcher ? defaultDispatchers[msgType]);
	}
	
	*cc { arg func, ccNum, chan, srcID, dispatcher;
		^this.new(func, ccNum, chan, \control, srcID, dispatcher);
	}
	
	*noteOn { arg func, noteNum, chan, srcID, dispatcher;
		^this.new(func, noteNum, chan, \noteOn, srcID, dispatcher);
	}
	
	*noteOff { arg func, noteNum, chan, srcID, dispatcher;
		^this.new(func, noteNum, chan, \noteOff, srcID, dispatcher);
	}
	
	*polytouch { arg func, noteNum, chan, srcID, dispatcher;
		^this.new(func, noteNum, chan, \polytouch, srcID, dispatcher);
	}
	
	*touch { arg func, chan, srcID, dispatcher;
		^this.new(func, nil, chan, \touch, srcID, dispatcher);
	}
	
	*bend { arg func, chan, srcID, dispatcher;
		^this.new(func, nil, chan, \bend, srcID, dispatcher);
	}
	
	*program { arg func, chan, srcID, dispatcher;
		^this.new(func, nil, chan, \program, srcID, dispatcher);
	}
	
	init {|argfunc, argmsgNum, argchan, argType, argsrcID, argdisp|
		msgNum = msgNum ? argmsgNum;
		chan = chan ? argchan;
		srcID = argsrcID ? srcID;
		func = argfunc;
		msgType = argType ? msgType;
		dispatcher = argdisp ? dispatcher;
		this.enable;
		allProxies.add(this);
	}
	
	// post pretty
	printOn { arg stream; stream << this.class.name << "(" <<* [msgType, msgNum, chan] << ")" }

}

MIDIdef : MIDIProxy {
	classvar <>all; // same as other def classes, do we need a setter really?
	var <key;
	
	*initClass {
		all = IdentityDictionary.new;
	}
	
	*new { arg key, func, msgNum, chan, msgType, srcID, dispatcher;
		var res = all.at(key);
		if(res.isNil) {
			^super.new(func, msgNum, chan, msgType, srcID, dispatcher).addToAll(key);
		} {
			if(func.notNil) { 
				if(res.enabled, {
					res.disable;
					res.init(func, msgNum, chan, msgType, srcID, dispatcher ? defaultDispatchers[msgType]);
				}, { res.init(func, msgNum, chan, msgType, srcID, dispatcher ? defaultDispatchers[msgType]).disable; });
			}
		}
		^res
	}
	
	*cc { arg key, func, ccNum, chan, srcID, dispatcher;
		^this.new(key, func, ccNum, chan, \control, srcID, dispatcher);
	}
	
	*noteOn { arg key, func, noteNum, chan, srcID, dispatcher;
		^this.new(key, func, noteNum, chan, \noteOn, srcID, dispatcher);
	}
	
	*noteOff { arg key, func, noteNum, chan, srcID, dispatcher;
		^this.new(key, func, noteNum, chan, \noteOff, srcID, dispatcher);
	}
	
	*polytouch { arg key, func, noteNum, chan, srcID, dispatcher;
		^this.new(key, func, noteNum, chan, \polytouch, srcID, dispatcher);
	}
	
	*touch { arg key, func, chan, srcID, dispatcher;
		^this.new(key, func, nil, chan, \touch, srcID, dispatcher);
	}
	
	*bend { arg key, func, chan, srcID, dispatcher;
		^this.new(key, func, nil, chan, \bend, srcID, dispatcher);
	}
	
	*program { arg key, func, chan, srcID, dispatcher;
		^this.new(key, func, nil, chan, \program, srcID, dispatcher);
	}
	
	addToAll {|argkey| key = argkey; all.put(key, this) }
	
	free { all[key] = nil; super.free; }
	
}


// if you need to test for srcID func gets wrapped in this
MIDIProxySrcMessageMatcher : AbstractMessageMatcher {
	var srcID;
	
	*new {|srcID, func| ^super.new.init(srcID, func);}
	
	init {|argsrcID, argfunc| srcID = argsrcID; func = argfunc; }
	
	value {|value, num, chan, testSrc|
		if(srcID == testSrc, {func.value(value, num, chan, testSrc)}) 
	}
}

// if you need to test for srcID func gets wrapped in this
MIDIProxyChanMessageMatcher : AbstractMessageMatcher {
	var chan;
	
	*new {|chan, func| ^super.new.init(chan, func);}
	
	init {|argchan, argfunc| chan = argchan; func = argfunc; }
	
	value {|value, num, testChan, srcID|
		if(chan == testChan, {func.value(value, num, testChan, srcID)}) 
	}
}

// if you need to test for chanArray func gets wrapped in this
MIDIProxyChanArrayMessageMatcher : AbstractMessageMatcher {
	var chanBools, <>func;
	
	*new {|chanArray, func|
		var chanBools;
		// lookup bool by index fastest, so construct an Array here
		chanBools = Array.fill(16, {|i| chanArray.includes(i) });
		^super.new.init(chanBools, func);
	}
	
	init {|argchanbools, argfunc| chanBools = argchanbools; func = argfunc; }
	
	value {|value, num, testChan, srcID| 
		// lookup bool by index fastest
		if(chanBools[testChan], {func.value(value, num, testChan, srcID)}) 
	}
}

// version for message types which don't pass a val
MIDIProxySrcMessageMatcherNV : MIDIProxySrcMessageMatcher {
	
	value {|num, chan, testSrc|
		if(srcID == testSrc, {func.value(num, chan, testSrc)}) 
	}
}

// if you need to test for chan and srcID func gets wrapped in this
MIDIProxyBothMessageMatcher : AbstractMessageMatcher {
	var chan, srcID;
	
	*new {|chan, srcID, func| ^super.new.init(chan, srcID, func);}
	
	init {|argchan, argsrcID, argfunc| chan = argchan; srcID = argsrcID; func = argfunc; }
	
	value {|value, num, testChan, testSrc| 
		if(srcID == testSrc and: {chan == testChan}, {func.value(value, num, testChan, testSrc)}) 
	}	
}


// if you need to test for chanArray and srcID func gets wrapped in this
MIDIProxyBothCAMessageMatcher : AbstractMessageMatcher {
	var chanBools, srcID;
	
	*new {|chanArray, srcID, func| 
		var chanBools;
		// lookup bool by index fastest, so construct an Array here
		chanBools = Array.fill(16, {|i| chanArray.includes(i) });
		^super.new.init(chanBools, srcID, func);
	}
	
	init {|argbools, argsrcID, argfunc| chanBools = argbools; srcID = argsrcID; func = argfunc; }
	
	value {|value, num, testChan, testSrc| 
		if(srcID == testSrc and: {chanBools[testChan]}, {func.value(value, num, testChan, testSrc)}) 
	}	
}

