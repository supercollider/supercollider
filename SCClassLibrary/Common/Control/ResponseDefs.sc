AbstractResponderFunc {
	classvar allFuncProxies;
	var <func, <srcID, <enabled = false, <dispatcher, <permanent = false;

	*initClass { Class.initClassTree(AbstractDispatcher); allFuncProxies = IdentitySet.new; }

	enable {
		if(enabled.not, {
			if(permanent.not, { CmdPeriod.add(this) });
			dispatcher.add(this);
			enabled = true;
			allFuncProxies.add(this);
		});
	}

	disable {
		if(permanent.not, { CmdPeriod.remove(this) });
		dispatcher.remove(this);
		enabled = false;
	}

	prFunc_ {|newFunc|
		func = newFunc;
		this.changed(\function);
	}

	add {|newFunc| func = func.addFunc(newFunc); this.changed(\function); }

	remove {|removeFunc| func = func.removeFunc(removeFunc); this.changed(\function); }

	gui { this.subclassResponsibility(thisMethod) }

	cmdPeriod { this.free }

	oneShot {
		var oneShotFunc, wrappedFunc;
		wrappedFunc = func;
		oneShotFunc = { arg ...args; this.free; wrappedFunc.value(*args); };
		this.prFunc_(oneShotFunc);
	}

	permanent_{|bool|
		permanent = bool;
		if(bool && enabled, { CmdPeriod.remove(this) }, {CmdPeriod.add(this) })
	}

	fix { this.permanent_(true) }

	free { allFuncProxies.remove(this); this.disable }

	clear { this.prFunc_(nil) }

	*allFuncProxies {
		var result;
		result = IdentityDictionary.new;
		allFuncProxies.do({|funcProxy|
			var key;
			key = funcProxy.dispatcher.typeKey;
			result[key] = result[key].add(funcProxy)
		});
		^result;
	}

	*allEnabled {
		var result;
		result = IdentityDictionary.new;
		allFuncProxies.select(_.enabled).do({|funcProxy|
			var key;
			key = funcProxy.dispatcher.typeKey;
			result[key] = result[key].add(funcProxy)
		});
		^result;
	}

	*allDisabled {
		var result;
		result = IdentityDictionary.new;
		allFuncProxies.reject(_.enabled).do({|funcProxy|
			var key;
			key = funcProxy.dispatcher.typeKey;
			result[key] = result[key].add(funcProxy)
		});
		^result;
	}

}

// defines the required interface
AbstractDispatcher {
	classvar <all;
	var registered = false;

	*new { ^super.new.init; }

	init { all.add(this);}

	*initClass { all = IdentitySet.new }

	add {|funcProxy| this.subclassResponsibility(thisMethod) } // proxies call this to add themselves to this dispatcher; should register this if needed

	remove {|funcProxy| this.subclassResponsibility(thisMethod) } // proxies call this to remove themselves from this dispatcher; should unregister if needed

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

	add {|funcProxy|
		var func, keys;
		funcProxy.addDependant(this);
		func = this.wrapFunc(funcProxy);
		wrappedFuncs[funcProxy] = func;
		keys = this.getKeysForFuncProxy(funcProxy);
		keys.do({|key| active[key] = active[key].addFunc(func) }); // support multiple keys
		if(registered.not, {this.register});
	}

	remove {|funcProxy|
		var func, keys;
		funcProxy.removeDependant(this);
		keys = this.getKeysForFuncProxy(funcProxy);
		func = wrappedFuncs[funcProxy];
		keys.do({|key| active[key] = active[key].removeFunc(func) }); // support multiple keys
		wrappedFuncs[funcProxy] = nil;
		if(active.size == 0, {this.unregister});
	}

		// old Funk vs. new Funk
	updateFuncForFuncProxy {|funcProxy|
		var func, oldFunc, keys;
		func = this.wrapFunc(funcProxy);
		oldFunc = wrappedFuncs[funcProxy];
		wrappedFuncs[funcProxy] = func;
		keys = this.getKeysForFuncProxy(funcProxy);
		keys.do({|key| active[key] = active[key].replaceFunc(oldFunc, func) }); // support multiple keys
	}

	wrapFunc { this.subclassResponsibility(thisMethod) }

	getKeysForFuncProxy { |funcProxy| this.subclassResponsibility(thisMethod) }

	update {|funcProxy, what| if(what == \function, { this.updateFuncForFuncProxy(funcProxy) }) }

	free { wrappedFuncs.keys.do({|funcProxy| funcProxy.removeDependant(this) }); super.free }

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

	wrapFunc {|funcProxy|
		var func, srcID, recvPort, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		recvPort = funcProxy.recvPort;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = OSCArgsMatcher(argTemplate, func)});
		^case(
			{ srcID.notNil && recvPort.notNil }, { OSCFuncBothMessageMatcher(srcID, recvPort, func) },
			{ srcID.notNil }, { OSCFuncAddrMessageMatcher(srcID, func) },
			{ recvPort.notNil }, { OSCFuncRecvPortMessageMatcher(recvPort, func) },
			{ func }
		);
	}

	getKeysForFuncProxy {|funcProxy| ^[funcProxy.path];}

	value {|msg, time, addr, recvPort| active[msg[0]].value(msg, time, addr, recvPort);}

	register {
		thisProcess.addOSCRecvFunc(this);
		registered = true;
	}

	unregister {
		thisProcess.removeOSCRecvFunc(this);
		registered = false;
	}

	typeKey { ^('OSC unmatched').asSymbol }

}

OSCMessagePatternDispatcher : OSCMessageDispatcher {

	value {|msg, time, addr, recvPort|
		var pattern;
		pattern = msg[0];
		active.keysValuesDo({|key, func|
			if(key.matchOSCAddressPattern(pattern), {func.value(msg, time, addr, recvPort);});
		})
	}

	typeKey { ^('OSC matched').asSymbol }

}

OSCFunc : AbstractResponderFunc {
	classvar <>defaultDispatcher, <>defaultMatchingDispatcher, traceFunc, traceRunning = false;
	var <path, <recvPort, <argTemplate;

	*initClass {
		defaultDispatcher = OSCMessageDispatcher.new;
		defaultMatchingDispatcher = OSCMessagePatternDispatcher.new;
		traceFunc = {|msg, time, addr, recvPort|
			"OSC Message Received:\n\ttime: %\n\taddress: %\n\trecvPort: %\n\tmsg: %\n\n".postf(time, addr, recvPort, msg);
		}
	}

	*new { arg func, path, srcID, recvPort, argTemplate, dispatcher;
		^super.new.init(func, path, srcID, recvPort, argTemplate, dispatcher ? defaultDispatcher);
	}

	*newMatching { arg func, path, srcID, recvPort, argTemplate;
		^super.new.init(func, path, srcID, recvPort, argTemplate, defaultMatchingDispatcher);
	}

	*trace {|bool = true, hideStatusMsg = false|
		if(bool, {
			if(traceRunning.not, {
				traceFunc = if(hideStatusMsg, {
					{|msg, time, addr, recvPort|
						if(((msg[0] === '/status.reply' and: {Server.all.any{|it|it.addr == addr}})).not, {
"OSC Message Received:\n\ttime: %\n\taddress: %\n\trecvPort: %\n\tmsg: %\n\n".postf(time, addr, recvPort, msg);
						});
					}
					}, {
						{|msg, time, addr, recvPort|
							"OSC Message Received:\n\ttime: %\n\taddress: %\n\trecvPort: %\n\tmsg: %\n\n".postf(time, addr, recvPort, msg);
						}

				});
				thisProcess.addOSCRecvFunc(traceFunc);
				CmdPeriod.add(this);
				traceRunning = true;
			});
		}, {
			thisProcess.removeOSCRecvFunc(traceFunc);
			CmdPeriod.remove(this);
			traceRunning = false;
		});
	}

	*cmdPeriod { this.trace(false) }

	init {|argfunc, argpath, argsrcID, argrecvPort, argtemplate, argdisp|
		path = (argpath ? path).asString;
		if(path[0] != $/, {path = "/" ++ path}); // demand OSC compliant paths
		path = path.asSymbol;
		srcID = argsrcID ? srcID;
		recvPort = argrecvPort ? recvPort;
		if(recvPort.notNil and: {thisProcess.openUDPPort(recvPort).not}, {
			Error("Could not open UDP port"+recvPort).throw;
		});
		argtemplate = argtemplate.collect({|oscArg|
			if(oscArg.isKindOf(String), {oscArg.asSymbol}, {oscArg}); // match Symbols not Strings
		});
		argTemplate = argtemplate ? argTemplate;
		func = argfunc ? func;
		dispatcher = argdisp ? dispatcher;
		this.enable;
		allFuncProxies.add(this);
	}

	printOn { arg stream; stream << this.class.name << "(" <<* [path, srcID, recvPort, argTemplate] << ")" }

}

OSCdef : OSCFunc {
	classvar <all;
	var <key;

	*initClass {
		all = IdentityDictionary.new;
	}

	*new { arg key, func, path, srcID, recvPort, argTemplate, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.new(func, path, srcID, recvPort, argTemplate, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.init(func, path, srcID, recvPort, argTemplate, dispatcher);
					if(wasDisabled, { res.disable; });
				} {|err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	*newMatching { arg key, func, path, srcID, recvPort, argTemplate;
		^this.new(key, func, path, srcID, recvPort, argTemplate, defaultMatchingDispatcher);
	}

	addToAll {|argkey| key = argkey; all.put(key, this) }

	free { all[key] = nil; super.free; }

	printOn { arg stream; stream << this.class.name << "(" <<* [key, path, srcID, recvPort, argTemplate] << ")" }

	*freeAll {
		var objs = all.copy;
		objs.do(_.free)
	}
}


// if you need to test for address func gets wrapped in this
OSCFuncAddrMessageMatcher : AbstractMessageMatcher {
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
OSCFuncRecvPortMessageMatcher : AbstractMessageMatcher {
	var recvPort;

	*new {|recvPort, func| ^super.new.init(recvPort, func);}

	init {|argrecvPort, argfunc| recvPort = argrecvPort; func = argfunc; }

	value {|msg, time, addr, testRecvPort|
		if(testRecvPort == recvPort, {
			func.value(msg, time, addr, testRecvPort)
		})
	}
}

OSCFuncBothMessageMatcher : AbstractMessageMatcher {
	var addr, recvPort;

	*new {|addr, recvPort, func| ^super.new.init(addr, recvPort, func);}

	init {|argaddr, argrecvPort, argfunc| addr = argaddr; recvPort = argrecvPort; func = argfunc; }

	value {|msg, time, testAddr, testRecvPort|
		if(testAddr.addr == addr.addr and: {addr.port.matchItem(testAddr.port)} and: {testRecvPort == recvPort}, {
			func.value(msg, time, testAddr, testRecvPort)
		})
	}
}

OSCArgsMatcher : AbstractMessageMatcher {
	var argTemplate;

	*new{|argTemplate, func| ^super.new.init(argTemplate, func) }

	init {|argArgTemplate, argFunc| argTemplate = argArgTemplate.asArray; func = argFunc; }

	value {|testMsg, time, addr, recvPort|
		testMsg[1..].do({|item, i|
			if(argTemplate[i].matchItem(item).not, {^this});
		});
		func.value(testMsg, time, addr, recvPort)
	}
}

///////////////////// MIDI

// for \noteOn, \noteOff, \control, \polytouch
MIDIMessageDispatcher : AbstractWrappingDispatcher {
	var >messageType;

	*new {|messageType| ^super.new.messageType_(messageType) }

	getKeysForFuncProxy {|funcProxy| ^(funcProxy.msgNum ? (0..127)).asArray;} // noteNum, etc.

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
	wrapFunc {|funcProxy|
		var func, chan, srcID, argTemplate;
		func = funcProxy.func;
		chan = funcProxy.chan;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = MIDIValueMatcher(argTemplate, func)});
		^case(
			{ srcID.notNil && chan.isArray }, {MIDIFuncBothCAMessageMatcher(chan, srcID, func)},
			{ srcID.notNil && chan.notNil }, {MIDIFuncBothMessageMatcher(chan, srcID, func)},
			{ srcID.notNil }, {MIDIFuncSrcMessageMatcher(srcID, func)},
			{ chan.isArray }, {MIDIFuncChanArrayMessageMatcher(chan, func)},
			{ chan.notNil }, {MIDIFuncChanMessageMatcher(chan, func)},
			{ func }
		);
	}

	typeKey { ^('MIDI ' ++ messageType).asSymbol }
}

// for \touch, \program, \bend
MIDIMessageDispatcherNV : MIDIMessageDispatcher {

	getKeysForFuncProxy {|funcProxy| ^(funcProxy.chan ? (0..15)).asArray;} // chan

	value {|src, chan, val| active[chan].value(val, chan, src);}

	// wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, chan, srcID, argTemplate;
		func = funcProxy.func;
		chan = funcProxy.chan;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = MIDIValueMatcher(argTemplate, func)});
		^case(
			{ srcID.notNil }, {MIDIFuncSrcMessageMatcherNV(srcID, func)},
			{ func }
		);
	}
}

// for \sysex
MIDISysexDispatcher : MIDIMessageDispatcher {

	getKeysForFuncProxy {|funcProxy| ^(funcProxy.srcID ? \all)}

	value {|srcID, data|
		active[srcID].value(data, srcID);
		active[\all].value(data, srcID);
	}

	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = MIDIValueMatcher(argTemplate, func)});
		^func;
	}
}

// sysrt with data
MIDISysDataDispatcher : MIDIMessageDispatcher {

	register {
		var hook;
		// select the correct low-level hook.
		hook = switch(messageType,
			\sysex, {\sysex},
			\mtcQF, {\smpte},
			{\sysrt}
		);
		MIDIIn.perform(hook.asSetter, MIDIIn.perform(hook.asGetter).addFunc(this));
		registered = true;
	}

	unregister {
		var hook;
		// select the correct low-level hook.
		hook = switch(messageType,
			\sysex, {\sysex},
			\mtcQF, {\smpte},
			{\sysrt}
		);
		MIDIIn.perform(hook.asSetter, MIDIIn.perform(hook.asGetter).removeFunc(this));
		registered = false;
	}

	getKeysForFuncProxy {|funcProxy| ^(funcProxy.msgNum ? (0..15)).asArray;} // noteNum, etc.

	value {|srcID, index, data|
		active[index].value(data, srcID, index);
	}

	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = MIDIValueMatcher(argTemplate, func)});
		^case(
			{ srcID.notNil }, {MIDIFuncSrcSysMessageMatcherND(srcID, func)},
			{ func }
		);
	}
}

MIDISysDataDropIndDispatcher : MIDISysDataDispatcher {
	
	value {|srcID, index, data|
		active[index].value(data, srcID);
	}

}

MIDISysNoDataDispatcher : MIDISysDataDispatcher {

	value {|srcID, index|
		active[index].value(srcID);
	}

	wrapFunc {|funcProxy|
		var func, srcID;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		^case(
			{ srcID.notNil }, {MIDIFuncSrcSysMessageMatcher(srcID, func)},
			{ func }
		);
	}
}

MIDIMTCtoSMPTEDispatcher : MIDISysexDispatcher {

	value {|srcID, index, data|
		active[srcID].value(srcID, index, data);
		active[\all].value(srcID, index, data);
	}

	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		if(argTemplate.notNil, { func = MIDIValueMatcher(argTemplate, func)});
		func = MIDISMPTEAssembler(func);
		^func;
	}
}

// thanks to nescivi for code from MTC class!!
MIDISMPTEAssembler :  AbstractMessageMatcher {
	var <mtctime, <mtc_t, <mtc_v, <mtc_r, <dropFrame = false;

	*new{|func|
		^super.new.init(func);
	}

	init{|argfunc|
		/// MTC: only deals with forward running time!
		func = argfunc;
		mtc_t = Array.fill(8,0);
		mtc_v = Array.fill(4,0);
		mtc_r = 30;
		mtctime = 0;
	}

	// assemble MTC quarter frames into a value in seconds
	value { arg srcID, index, data;
		var hex;
		mtc_t[index] = data;
		if ( index == 7,
			{
				mtc_v[0] = mtc_t[1] + mtc_t[0];
				mtc_v[1] = mtc_t[3] + mtc_t[2];
				mtc_v[2] = mtc_t[5] + mtc_t[4];
				mtc_v[3] = mtc_t[7].mod(2) + mtc_t[6];
				hex = mtc_t[7].asHexString[6];
				switch (hex,
					$6, { mtc_r = 30; dropFrame = false},
					$4, { mtc_r = 30; dropFrame = true},
					$2, { mtc_r = 25; dropFrame = false},
					$0, { mtc_r = 24; dropFrame = false}
				);
				mtctime = (mtc_v[0]/mtc_r) + mtc_v[1] + (mtc_v[2]*60) + (mtc_v[3]*3600);
				func.value( mtctime, mtc_r, dropFrame, srcID );
		});
	}

}


MIDIFunc : AbstractResponderFunc {
	classvar <>defaultDispatchers, traceFuncs, traceRunning = false, sysIndices;
	var <chan, <msgNum, <msgType, <argTemplate;

	*initClass {
		defaultDispatchers = IdentityDictionary.new;
		traceFuncs = IdentityDictionary.new;
		sysIndices = IdentityDictionary[\mtcQF->1, \songPosition->2, \songSelect->3, \tuneRequest->6, \midiClock->8, \tick->9, \start->10, \continue->11, \stop->12, \activeSense->14, \reset->15];
		[\noteOn, \noteOff, \control, \polytouch].do({|type|
			defaultDispatchers[type] = MIDIMessageDispatcher(type);
			traceFuncs[type] = {|src, chan, num, val|
				"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\tchan: %\n\tnum: %\n\tval: %\n\n".postf(type, src, chan, num, val);
			};
		});
		[\sysex].do({|type|
			defaultDispatchers[type] = MIDISysexDispatcher(type);
			traceFuncs[type] = {|src, data|
				"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\tdata: %\n\n".postf(type, src, data);
			};
		});
		[\mtcQF].do({|type|
			defaultDispatchers[type] = MIDISysDataDispatcher(type);
			traceFuncs[type] = {|src, ind, data|
				"MIDI Message Received:\n\ttype: %\n\tindex: %\n\tsrc: %\n\tdata: %\n\n".postf(type, ind, src, data);
			};
		});
		[\smpte].do({|type|
			defaultDispatchers[type] = MIDIMTCtoSMPTEDispatcher(type);
/*			traceFuncs[type] = {|src, ind, data|
				"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\tdata: %\n\n".postf(type, src, data);
			};*/ // unneeded? Just trace raw MTC
		});
		[\songPosition, \songSelect].do({|type|
			defaultDispatchers[type] = MIDISysDataDropIndDispatcher(type);
			traceFuncs[type] = {|src, ind, data|
				if(ind == sysIndices[type], {
					"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\tdata: %\n\n".postf(type, src, data);
				});
			};
		});
		[\sysrt].do({|type|
			defaultDispatchers[type] = MIDISysDataDispatcher(type);
			traceFuncs[type] = {|src, ind, data|
				"MIDI Message Received:\n\ttype: %\n\tindex: %\n\tsrc: %\n\tdata: %\n\n".postf(type, ind, src, data);
			};// maybe unneeded
		});
		[\tuneRequest, \midiClock, \tick, \start, \continue, \stop, \activeSense, \reset].do({|type|
			defaultDispatchers[type] = MIDISysNoDataDispatcher(type);
			traceFuncs[type] = {|src, ind|
				if(ind == sysIndices[type], {
					"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\n".postf(type, src);
				});
			};
		});
		[\touch, \program, \bend].do({|type|
			defaultDispatchers[type] = MIDIMessageDispatcherNV(type);
			traceFuncs[type] = {|src, chan, num|
				"MIDI Message Received:\n\ttype: %\n\tsrc: %\n\tchan: %\n\tnum: %\n\n".postf(type, src, chan, num);
			};
		});
	}

	*trace {|bool = true|
		if(bool, {
			if(traceRunning.not, {
				[\noteOn, \noteOff, \control, \polytouch, \touch, \program, \bend, \sysex].do({|type|
					MIDIIn.addFuncTo(type, traceFuncs[type]);
				});
				[\tuneRequest, \midiClock, \tick, \start, \continue, \stop, \activeSense, \reset, \songPosition, \songSelect].do({|type|
					MIDIIn.addFuncTo(\sysrt, traceFuncs[type]);
				});
				[\mtcQF].do({|type|
					MIDIIn.addFuncTo(\smpte, traceFuncs[type]);
				});
				CmdPeriod.add(this);
				traceRunning = true;
			});
		}, {
			[\noteOn, \noteOff, \control, \polytouch, \touch, \program, \bend, \sysex].do({|type|
				MIDIIn.removeFuncFrom(type, traceFuncs[type]);
			});
			[\tuneRequest, \midiClock, \tick, \start, \continue, \stop, \activeSense, \reset, \songPosition, \songSelect].do({|type|
				MIDIIn.removeFuncFrom(\sysrt, traceFuncs[type]);
			});
			[\mtcQF].do({|type|
				MIDIIn.removeFuncFrom(\smpte, traceFuncs[type]);
			});
			CmdPeriod.remove(this);
			traceRunning = false;
		});
	}

	*cmdPeriod { this.trace(false) }

	*new { arg func, msgNum, chan, msgType, srcID, argTemplate, dispatcher;
		^super.new.init(func, msgNum, chan, msgType, srcID, argTemplate, dispatcher ? defaultDispatchers[msgType]);
	}

	*cc { arg func, ccNum, chan, srcID, argTemplate, dispatcher;
		^this.new(func, ccNum, chan, \control, srcID, argTemplate, dispatcher);
	}

	*noteOn { arg func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(func, noteNum, chan, \noteOn, srcID, argTemplate, dispatcher);
	}

	*noteOff { arg func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(func, noteNum, chan, \noteOff, srcID, argTemplate, dispatcher);
	}

	*polytouch { arg func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(func, noteNum, chan, \polytouch, srcID, argTemplate, dispatcher);
	}

	*touch { arg func, chan, srcID, argTemplate, dispatcher;
		^this.new(func, nil, chan, \touch, srcID, argTemplate, dispatcher);
	}

	*bend { arg func, chan, srcID, argTemplate, dispatcher;
		^this.new(func, nil, chan, \bend, srcID, argTemplate, dispatcher);
	}

	*program { arg func, chan, srcID, argTemplate, dispatcher;
		^this.new(func, nil, chan, \program, srcID, argTemplate, dispatcher);
	}

	///// system messages

	*sysex { arg func, srcID, argTemplate, dispatcher;
		^this.new(func, nil, nil, \sysex, srcID, argTemplate, dispatcher);
	}

	// system common

	// does this need to be registered on the SMPTE hook? Yes!
	*mtcQuarterFrame {arg func, srcID, argTemplate, dispatcher;
		^this.new(func, nil, nil, \mtcQF, srcID, argTemplate, dispatcher); // actually index 1 sysrt, but on smpte hook
	}

	*smpte {arg func, srcID, argTemplate, dispatcher;
		^this.new(func, nil, nil, \smpte, srcID, argTemplate, dispatcher); // actually index 1 sysrt, but on smpte hook
	}

	*songPosition {arg func, srcID, argTemplate, dispatcher;
		^this.new(func, 2, nil, \songPosition, srcID, argTemplate, dispatcher);
	}

	*songSelect {arg func, srcID, argTemplate, dispatcher;
		^this.new(func, 3, nil, \songSelect, srcID, argTemplate, dispatcher);
	}

	*tuneRequest {arg func, srcID, dispatcher;
		^this.new(func, 6, nil, \tuneRequest, srcID, nil, dispatcher);
	}

	*midiClock {arg func, srcID, dispatcher;
		^this.new(func, 8, nil, \midiClock, srcID, nil, dispatcher);
	}

	// system realtime

	// generic
	*sysrt { arg func, index, srcID, argTemplate, dispatcher;
		^this.new(func, index, nil, \sysrt, srcID, argTemplate, dispatcher);
	}

	*tick {arg func, srcID, dispatcher;
		^this.new(func, 9, nil, \tick, srcID, nil, dispatcher);
	}

	*start {arg func, srcID, dispatcher;
		^this.new(func, 10, nil, \start, srcID, nil, dispatcher);
	}

	*continue {arg func, srcID, dispatcher;
		^this.new(func, 11, nil, \continue, srcID, nil, dispatcher);
	}

	*stop {arg func, srcID, dispatcher;
		^this.new(func, 12, nil, \stop, srcID, nil, dispatcher);
	}

	*activeSense {arg func, srcID, dispatcher;
		^this.new(func, 14, nil, \activeSense, srcID, nil, dispatcher);
	}

	*reset {arg func, srcID, dispatcher;
		^this.new(func, 15, nil, \reset, srcID, nil, dispatcher);
	}


	init {|argfunc, argmsgNum, argchan, argType, argsrcID, argtempl, argdisp|
		msgNum = argmsgNum ? msgNum;
		msgNum = msgNum.isNumber.if({ msgNum.asInteger }, msgNum);
		msgNum = msgNum.isCollection.if({ msgNum.collect(_.asInteger) }, msgNum);
		chan = argchan ? chan;
		srcID = argsrcID ? srcID;
		func = argfunc ? func;
		msgType = argType ? msgType;
		dispatcher = argdisp ? dispatcher;
		argTemplate = argtempl ? argTemplate;
		this.enable;
		allFuncProxies.add(this);
	}

	// swap out func and wait
	learn {|learnVal = false|
		// check for cc or noteon?
		var learnFunc;
		/*this.remove(func);*/
		learnFunc = this.learnFunc(learnVal);
		this.disable;
		this.init(learnFunc); // keep old args if specified, so we can learn from particular channels, srcs, etc.
	}

	learnFunc {|learnVal|
		var oldFunc, learnFunc;
		oldFunc = func; // old funk is ultimately better than new funk
		if([\control, \noteOn, \noteOff, \polytouch].includes(msgType), {
			^{|val, num, chan, srcID|
				"MIDIFunc learned: type: %\tnum: %\tval: %\tchan: %\tsrcID: %\t\n".postf(msgType, num, val, chan, srcID);
				this.disable;
				this.remove(learnFunc);
				oldFunc.value(val, num, chan, srcID);// do first action
				this.init(oldFunc, num, chan, msgType, srcID, if(learnVal, val, nil));
			}
		});

		// noNum
		if([\touch, \program, \bend].includes(msgType), {
			^{|val, chan, srcID|
				"MIDIFunc learned: type: %\tval: %\tchan: %\tsrcID: %\t\n".postf(msgType, val, chan, srcID);
				this.disable;
				this.remove(learnFunc);
				oldFunc.value(val, chan, srcID);// do first action
				this.init(oldFunc, nil, chan, msgType, srcID, if(learnVal, val, nil));
			}
		});

		// sys with Data
		if([\mtcQF, \smpte, \songPosition, \songSelect, \sysrt].includes(msgType), {
			^{|val, srcID, index|
				"MIDIFunc learned: type: %\tnum: %\tval: %\tsrcID: %\t\n".postf(msgType, index, val, srcID);
				this.disable;
				this.remove(learnFunc);
				oldFunc.value(val, srcID, index);// do first action
				this.init(oldFunc, index, nil, msgType, srcID, if(learnVal, val, nil));
			}
		});

		// sys no Data
		if([\tuneRequest, \midiClock, \tick, \start, \continue, \stop, \activeSense, \reset].includes(msgType), {
			^{|srcID, index|
				"MIDIFunc learned: type: %\tnum: %\tsrcID: %\t\n".postf(msgType, index, srcID);
				this.disable;
				this.remove(learnFunc);
				oldFunc.value(srcID, index);// do first action
				this.init(oldFunc, index, nil, msgType, srcID, nil);
			}
		});

		// sysex
		if(msgType == \sysex, {
			^{|val, srcID|
				"MIDIFunc learned: type: %\tdata: %\tsrcID: %\t\n".postf(msgType, val, srcID);
				this.disable;
				this.remove(learnFunc);
				oldFunc.value(val, srcID);// do first action
				this.init(oldFunc, nil, nil, msgType, srcID, if(learnVal, val, nil));
			}
		});

		"msgType: % not standard, cannot learn".format(msgType).warn;
	}


	// post pretty
	printOn { arg stream; stream << this.class.name << "(" <<* [msgType, msgNum, chan, argTemplate] << ")" }

}

MIDIdef : MIDIFunc {
	classvar <>all; // same as other def classes, do we need a setter really?
	var <key;

	*initClass {
		all = IdentityDictionary.new;
	}

	*new { arg key, func, msgNum, chan, msgType, srcID, argTemplate, dispatcher;
		var res = all.at(key);
		if(res.isNil) {
			^super.new(func, msgNum, chan, msgType, srcID, argTemplate, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				if(res.enabled, {
					res.disable;
					res.init(func, msgNum, chan, msgType, srcID, argTemplate, dispatcher ? defaultDispatchers[msgType]);
				}, { res.init(func, msgNum, chan, msgType, srcID, argTemplate, dispatcher ? defaultDispatchers[msgType]).disable; });
			}
		}
		^res
	}

	*cc { arg key, func, ccNum, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, ccNum, chan, \control, srcID, argTemplate, dispatcher);
	}

	*noteOn { arg key, func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, noteNum, chan, \noteOn, srcID, argTemplate, dispatcher);
	}

	*noteOff { arg key, func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, noteNum, chan, \noteOff, srcID, argTemplate, dispatcher);
	}

	*polytouch { arg key, func, noteNum, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, noteNum, chan, \polytouch, srcID, argTemplate, dispatcher);
	}

	*touch { arg key, func, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, chan, \touch, srcID, argTemplate, dispatcher);
	}

	*bend { arg key, func, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, chan, \bend, srcID, argTemplate, dispatcher);
	}

	*program { arg key, func, chan, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, chan, \program, srcID, argTemplate, dispatcher);
	}

	///// system messages

	*sysex { arg key, func, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, nil, \sysex, srcID, argTemplate, dispatcher);
	}

		// system common

	// does this need to be registered on the SMPTE hook? Yes!
	*mtcQuarterFrame {arg key, func, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, nil, \mtcQF, srcID, argTemplate, dispatcher); // actually index 1 sysrt, but on smpte hook
	}

	*smpte {arg key, func, srcID, argTemplate, dispatcher;
		^this.new(key, func, nil, nil, \smpte, srcID, argTemplate, dispatcher); // actually index 1 sysrt, but on smpte hook
	}

	*songPosition {arg key, func, srcID, argTemplate, dispatcher;
		^this.new(key, func, 2, nil, \songPosition, srcID, argTemplate, dispatcher);
	}

	*songSelect {arg key, func, srcID, argTemplate, dispatcher;
		^this.new(key, func, 3, nil, \songSelect, srcID, argTemplate, dispatcher);
	}

	*tuneRequest {arg key, func, srcID, dispatcher;
		^this.new(key, func, 6, nil, \tuneRequest, srcID, nil, dispatcher);
	}

	*midiClock {arg key, func, srcID, dispatcher;
		^this.new(key, func, 8, nil, \midiClock, srcID, nil, dispatcher);
	}

	// system realtime

	// generic
	*sysrt { arg key, func, index, srcID, argTemplate, dispatcher;
		^this.new(key, func, index, nil, \sysrt, srcID, argTemplate, dispatcher);
	}

	*tick {arg key, func, srcID, dispatcher;
		^this.new(key, func, 9, nil, \tick, srcID, nil, dispatcher);
	}

	*start {arg key, func, srcID, dispatcher;
		^this.new(key, func, 10, nil, \start, srcID, nil, dispatcher);
	}

	*continue {arg key, func, srcID, dispatcher;
		^this.new(key, func, 11, nil, \continue, srcID, nil, dispatcher);
	}

	*stop {arg key, func, srcID, dispatcher;
		^this.new(key, func, 12, nil, \stop, srcID, nil, dispatcher);
	}

	*activeSense {arg key, func, srcID, dispatcher;
		^this.new(key, func, 14, nil, \activeSense, srcID, nil, dispatcher);
	}

	*reset {arg key, func, srcID, dispatcher;
		^this.new(key, func, 15, nil, \reset, srcID, nil, dispatcher);
	}

	addToAll {|argkey| key = argkey; all.put(key, this) }

	free { all[key] = nil; super.free; }

	// post pretty
	printOn { arg stream; stream << this.class.name << "(" <<* [key, msgType, msgNum, chan, argTemplate] << ")" }

	*freeAll {
		var objs = all.copy;
		objs.do(_.free)
	}
}


// if you need to test for srcID func gets wrapped in this
MIDIFuncSrcMessageMatcher : AbstractMessageMatcher {
	var srcID;

	*new {|srcID, func| ^super.new.init(srcID, func);}

	init {|argsrcID, argfunc| srcID = argsrcID; func = argfunc; }

	value {|value, num, chan, testSrc|
		if(srcID == testSrc, {func.value(value, num, chan, testSrc)})
	}
}

// if you need to test for chan func gets wrapped in this
MIDIFuncChanMessageMatcher : AbstractMessageMatcher {
	var chan;

	*new {|chan, func| ^super.new.init(chan, func);}

	init {|argchan, argfunc| chan = argchan; func = argfunc; }

	value {|value, num, testChan, srcID|
		if(chan == testChan, {func.value(value, num, testChan, srcID)})
	}
}

// if you need to test for chanArray func gets wrapped in this
MIDIFuncChanArrayMessageMatcher : AbstractMessageMatcher {
	var chanBools;

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
MIDIFuncSrcMessageMatcherNV : MIDIFuncSrcMessageMatcher {

	value {|num, chan, testSrc|
		if(srcID == testSrc, {func.value(num, chan, testSrc)})
	}
}

// version for message types which don't pass a val
MIDIFuncSrcSysMessageMatcher : MIDIFuncSrcMessageMatcher {

	value {|testSrc, index, data|
		if(srcID == testSrc, {func.value(data, testSrc, index)})
	}
}

MIDIFuncSrcSysMessageMatcherND : MIDIFuncSrcMessageMatcher {

	value {|testSrc, index|
		if(srcID == testSrc, {func.value(testSrc, index)})
	}
}

// if you need to test for chan and srcID func gets wrapped in this
MIDIFuncBothMessageMatcher : AbstractMessageMatcher {
	var chan, srcID;

	*new {|chan, srcID, func| ^super.new.init(chan, srcID, func);}

	init {|argchan, argsrcID, argfunc| chan = argchan; srcID = argsrcID; func = argfunc; }

	value {|value, num, testChan, testSrc|
		if(srcID == testSrc and: {chan == testChan}, {func.value(value, num, testChan, testSrc)})
	}
}


// if you need to test for chanArray and srcID func gets wrapped in this
MIDIFuncBothCAMessageMatcher : AbstractMessageMatcher {
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

// if you want to test for the actual message value, the func gets wrapped in this
MIDIValueMatcher : AbstractMessageMatcher {
	var argTemplate;

	*new{|argTemplate, func| ^super.new.init(argTemplate, func) }

	init {|argArgTemplate, argFunc| argTemplate = argArgTemplate; func = argFunc; }

	value {|...testMsg|
		if(argTemplate.matchItem(testMsg.first), {func.value(*testMsg)});
	}
}
