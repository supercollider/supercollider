///////////////////// HID

/* There's a question here of how to construct the dispatchers. The general idea is to use the 'most significant' parameter as an initial filter for speed reasons, and then use wrappers to test other matches. In this case I'd suggest dispatching via a single key (perhaps constructed), but it depends on the lower level implementation if that makes sense. */

HIDMessageDispatcher : AbstractWrappingDispatcher {

	*new { ^super.new; }

	getKeysForFuncProxy {|funcProxy| ^funcProxy.key;} // here's the top level dispatch key

	value {|key, vals, src| active[key].value(vals, src, key);} // vals are the arguments, src is the device

	register {
		// add here to the low level hooks, here's how it works for MIDI
		//MIDIIn.perform(messageType.asSetter, MIDIIn.perform(messageType.asGetter).addFunc(this));
		registered = true;
	}

	unregister {
		// remove here from the low level hooks, here's how it works for MIDI
		//MIDIIn.perform(messageType.asSetter, MIDIIn.perform(messageType.asGetter).removeFunc(this));
		registered = false;
	}

	// wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.srcID;
		argTemplate = funcProxy.argTemplate;
		// here test for various matching possibilities
		// for HID should probably look something like this.
		if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)});
		^case(
			{ srcID.notNil }, {HIDFuncSrcMessageMatcher(srcID, func)},
			{ func }
		);
	}

	typeKey { ^'HID' }
}


// since HID is arbitrarily extensible, I suggest just a simple key, like an OSC address
// the dispatcher could possible construct this if needed
HIDFunc : AbstractResponderFunc {
	classvar <>defaultDispatcher, traceFunc, traceRunning = false;
	var <hidelkey, <hiddev, <elpage, <elusage;
    var <argTemplate;

	*initClass {
		defaultDispatcher = HIDMessageDispatcher.new;
		// not sure what this should look like, but a trace func would be nice
        traceFunc = { |value, physValue, rawValue, arrayValue, usage, page, elid, devid, dev|
            // devid, dev, elid, page, usage, rawValue, value|
            "HID Element Data:\n\tdevid: %, elid: %\n\t%\n\tdevice: \t page: % \tusage: %\n\telement: \t page: %\tusage: %\tarray value: %,\traw value: %,\tphysical value: %,\tvalue: %\n"
            .postf( devid, elid, dev.info, dev.usagePage, dev.usage, page, usage, arrayValue, rawValue, physValue, value );
		}
	}

	*new { arg func, elType, elPage, deviceInfo, argTemplate, dispatcher;
		^super.new.init(func, elType, elPage, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}

    /// different types:

    // element/types:
    // filter by element usage + page
    // filter by element id (in case the usages are doubled  - in case of badly designed device firmware)

    // device/types:
    // filter by vendor, product, -> path
    // filter by page, usage

    // -> main:  devType, elType - determines the filtering
    // -> usage: pass in:

	*trace {|bool = true|
		if(bool, {
			if(traceRunning.not, {
                HID.addRecvFunc( traceFunc );
				CmdPeriod.add(this);
				traceRunning = true;
			});
		}, {
            HID.removeRecvFunc( traceFunc );
			CmdPeriod.remove(this);
			traceRunning = false;
		});
	}

	*cmdPeriod { this.trace(false) }

	init {|argfunc, argel, argdevID, argpage, argusage, argtemplate, argdisp|
		hidelkey = argel.asSymbol;
		hiddev = argdevID ? hiddev;
        elpage = argpage ? elpage;
        elusage = argusage ? elusage;
		argTemplate = argtemplate ? argTemplate;
		func = argfunc ? func;
		dispatcher = argdisp ? dispatcher;
		this.enable;
		allFuncProxies.add(this);
	}

	printOn { arg stream; stream << this.class.name << "(" <<* [ hidelkey, hiddev, elpage, elusage, argTemplate] << ")" }

}

HIDdef : HIDFunc {
	classvar <all;
	var <key;

	*initClass {
		all = IdentityDictionary.new;
	}

	*new { arg key, func, hidkey, srcID, argTemplate, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.new(func, hidkey, srcID, argTemplate, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.init(func, hidkey, srcID, argTemplate, dispatcher);
					if(wasDisabled, { res.disable; });
				} {|err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	addToAll {|argkey| key = argkey; all.put(key, this) }

	free { all[key] = nil; super.free; }

    printOn { arg stream; stream << this.class.name << "(" <<* [key, hidelkey, hiddev, elpage, elusage, argTemplate] << ")" }

	*freeAll {
		var objs = all.copy;
		objs.do(_.free)
	}
}

// matching wrappers
// if you need to test for srcID func gets wrapped in this
HIDFuncSrcMessageMatcher : AbstractMessageMatcher {
	var srcID;

	*new {|srcID, func| ^super.new.init(srcID, func);}

	init {|argsrcID, argfunc| srcID = argsrcID; func = argfunc; }

	value {|vals, testSrc, key|
		if(srcID == testSrc, {func.value(vals, testSrc, key)})
	}
}

// if you want to test for the actual message value, the func gets wrapped in this
// the MIDI version and this could probably be one class
HIDValueMatcher : AbstractMessageMatcher {
	var argTemplate;

	*new{|argTemplate, func| ^super.new.init(argTemplate, func) }

	init {|argArgTemplate, argFunc| argTemplate = argArgTemplate; func = argFunc; }

	value {|...testMsg|
		if(argTemplate.matchItem(testMsg.first), {func.value(*testMsg)});
	}
}
