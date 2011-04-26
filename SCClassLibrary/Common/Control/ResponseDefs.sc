AbstractResponderProxy {
	classvar allProxies;
	var <func, <srcID, <enabled = false, <dispatcher, <permanent = false;
	
	*initClass { Class.initClassTree(AbstractDispatcher); allProxies = IdentitySet.new; }
	
	enable { 
		if(enabled.not, {
			if(permanent.not, { CmdPeriod.add(this) });
			dispatcher.add(this);
			enabled = true;
		});
	} 
	
	disable { 
		if(permanent.not, { CmdPeriod.remove(this) });
		dispatcher.remove(this);
		enabled = false;
	}
	
	func_ {|newFunc|  
		func = newFunc; 
		dispatcher.updateFuncForProxy(this);
	}
	
	gui { this.subclassResponsibility(thisMethod) }
	
	cmdPeriod { this.disable }
	
	oneShot {
		var oneShotFunc, wrappedFunc;
		wrappedFunc = func;
		oneShotFunc = { arg ...args; wrappedFunc.value(*args); this.disable };
		this.func_(oneShotFunc);
	}
	
	permanent_{|bool| 
		permanent = bool;
		if(bool && enabled, { CmdPeriod.remove(this) }, {CmdPeriod.add(this) })
	}
	
	fix { this.permanent_(true) }
	
	clear { allProxies.remove(this); this.disable }
	
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

AbstractDispatcher {
	classvar <>all;
	var registered = false;
	var <active, <wrappedFuncs;
	
	*new { ^super.new.init; }
	
	init { all.add(this); active = IdentityDictionary.new; wrappedFuncs = IdentityDictionary.new; }
		
	*initClass { all = IdentitySet.new }
	
	add {|proxy| 
		var func, keys;
		func = this.wrapFunc(proxy);
		wrappedFuncs[proxy] = func;
		keys = this.getKeysForProxy(proxy);
		keys.do({|key| active[key] = active[key].addFunc(func) }); // support multiple keys
		if(registered.not, {this.register});
	}
	
	remove {|proxy|
		var func, keys;
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
		
	value { this.subclassResponsibility(thisMethod) }
	
	valueArray {arg args; ^this.value(*args) } // needed to work in FunctionLists
	
	register { this.subclassResponsibility(thisMethod) }
	
	unregister { this.subclassResponsibility(thisMethod) }
	
	clear { this.unregister; all.remove(this) }
	
	typeKey { this.subclassResponsibility(thisMethod) } // a Symbol
	
}

AbstractFuncWrap {
	var <>func;
	
	valueArray {arg args; ^this.value(*args) } // needed to work in FunctionLists

}

///////////////////// OSC

OSCMessageDispatcher : AbstractDispatcher {
	
	wrapFunc {|proxy| ^if(proxy.srcID.notNil, {
			OSCProxyFuncWrap(proxy.srcID, proxy.func);
		}, { proxy.func });
	}
	
	getKeysForProxy {|proxy| ^[proxy.path];}
	
	value {|time, addr, msg| active[msg[0]].value(time, msg, addr);}
	
	register { 
		thisProcess.recvOSCfunc = thisProcess.recvOSCfunc.addFunc(this); 
		registered = true; 
	}
	
	unregister { 
		thisProcess.recvOSCfunc = thisProcess.recvOSCfunc.removeFunc(this);
		registered = false;
	}
	
	typeKey { ^('OSC ' ++ \unmatched).asSymbol }
	
}

OSCMessagePatternDispatcher : OSCMessageDispatcher {
	
	value {|time, addr, msg| 
		var pattern;
		pattern = msg[0];
		active.keysValuesDo({|key, func|
			if(key.matchOSCAddressPattern(pattern), {func.value(time, msg, addr);});
		})
	}
	
	typeKey { ^('OSC ' ++ \matched).asSymbol }
	
}

OSCProxy : AbstractResponderProxy {
	classvar <>defaultDispatcher, <>defaultMatchingDispatcher, traceFunc;
	var <path;
	
	*initClass {
		defaultDispatcher = OSCMessageDispatcher.new;
		defaultMatchingDispatcher = OSCMessagePatternDispatcher.new;
		traceFunc = {|time, replyAddr, msg|
			"OSC Message Received:\n\ttime: %\n\taddress: %\n\t msg: %\n\n".postf(time, replyAddr, msg);
		}
	}
	
	*new { arg func, path, srcID, dispatcher;
		^super.new.init(func, path, srcID, dispatcher ? defaultDispatcher);
	}
	
	*newMatching { arg func, path, srcID;
		^super.new.init(func, path, srcID, defaultMatchingDispatcher);
	}
	
	*trace {|bool = true| 
		if(bool, {
			thisProcess.addOSCFunc(traceFunc);
			CmdPeriod.add(this);
		}, {
			thisProcess.removeOSCFunc(traceFunc);
			CmdPeriod.remove(this);
		});
	}
	
	*cmdPeriod { this.trace(false) }
	
	init {|argfunc, argpath, argsrcID, argdisp|
		path = (argpath ? path).asString;
		if(path[0] != $/, {path = "/" ++ path}); // demand OSC compliant paths
		path = path.asSymbol;
		srcID = argsrcID ? srcID;
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
	
	*new { arg key, func, path, srcID;
		var res = all.at(key);
		if(res.isNil) {
			^super.new(func, path, srcID).addToAll(key);
		} {
			if(func.notNil) { 
				if(res.enabled, {
					res.disable;
					res.init(func, srcID, path);
				}, { res.init(func, srcID, path).disable; });
			}
		}
		^res
	}
	
	addToAll {|argkey| key = argkey; all.put(key, this) }
	
	clear { all[key] = nil; super.clear; }
	
}


// if you need to test for address func gets wrapped in this
OSCProxyFuncWrap : AbstractFuncWrap {
	var addr;
	
	*new {|addr, func| ^super.new.init(addr, func);}
	
	init {|argaddr, argfunc| addr = argaddr; func = argfunc; }
	
	value {|time, msg, testAddr| if(addr == testAddr, {func.value(time, msg, testAddr)}) }
}

///////////////////// MIDI

// for \noteOn, \noteOff, \control, \polytouch
MIDIMessageDispatcher : AbstractDispatcher {
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
			{ srcID.notNil && chan.isArray }, {MIDIProxyBothCAFuncWrap(chan, srcID, func)},
			{ srcID.notNil && chan.notNil }, {MIDIProxyBothFuncWrap(chan, srcID, func)},
			{ srcID.notNil }, {MIDIProxySrcFuncWrap(srcID, func)},
			{ chan.isArray }, {MIDIProxyChanArrayFuncWrap(chan, func)},
			{ chan.notNil }, {MIDIProxyChanFuncWrap(chan, func)},
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
			{ srcID.notNil }, {MIDIProxySrcFuncWrapNV(srcID, func)},
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
	
	clear { all[key] = nil; super.clear; }
	
}


// if you need to test for srcID func gets wrapped in this
MIDIProxySrcFuncWrap : AbstractFuncWrap {
	var srcID;
	
	*new {|srcID, func| ^super.new.init(srcID, func);}
	
	init {|argsrcID, argfunc| srcID = argsrcID; func = argfunc; }
	
	value {|value, num, chan, testSrc|
		if(srcID == testSrc, {func.value(value, num, chan, testSrc)}) 
	}
}

// if you need to test for srcID func gets wrapped in this
MIDIProxyChanFuncWrap : AbstractFuncWrap {
	var chan;
	
	*new {|chan, func| ^super.new.init(chan, func);}
	
	init {|argchan, argfunc| chan = argchan; func = argfunc; }
	
	value {|value, num, testChan, srcID|
		if(chan == testChan, {func.value(value, num, testChan, srcID)}) 
	}
}

// if you need to test for chanArray func gets wrapped in this
MIDIProxyChanArrayFuncWrap : AbstractFuncWrap {
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
MIDIProxySrcFuncWrapNV : MIDIProxySrcFuncWrap {
	
	value {|num, chan, testSrc|
		if(srcID == testSrc, {func.value(num, chan, testSrc)}) 
	}
}

// if you need to test for chan and srcID func gets wrapped in this
MIDIProxyBothFuncWrap : AbstractFuncWrap {
	var chan, srcID;
	
	*new {|chan, srcID, func| ^super.new.init(chan, srcID, func);}
	
	init {|argchan, argsrcID, argfunc| chan = argchan; srcID = argsrcID; func = argfunc; }
	
	value {|value, num, testChan, testSrc| 
		if(srcID == testSrc and: {chan == testChan}, {func.value(value, num, testChan, testSrc)}) 
	}	
}


// if you need to test for chanArray and srcID func gets wrapped in this
MIDIProxyBothCAFuncWrap : AbstractFuncWrap {
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

