///////////////////// HID

/* There's a question here of how to construct the dispatchers. The general idea is to use the 'most significant' parameter as an initial filter for speed reasons, and then use wrappers to test other matches. In this case I'd suggest dispatching via a single key (perhaps constructed), but it depends on the lower level implementation if that makes sense. */
/*
HIDUsageIDDispatcher : AbstractWrappingDispatcher {

}

HIDDeviceDispatcher : AbstractWrappingDispatcher {

}

HIDElementDispatcher : AbstractWrappingDispatcher {

}
*/

HIDUsageDispatcher : AbstractWrappingDispatcher {

	*new { ^super.new; }

	getKeysForFuncProxy {|funcProxy|
        /*
        var keyArray;
        var keys;
        if ( funcProxy.elUsage.isKindOf( Array ) ){
            keys = funcProxy.elUsage.collect{ |fpu|
                keyArray = HIDUsage.getUsageIds( fpu );
                "%_%".format( *keyArray ).asSymbol;
            };
        }{
            keyArray = HIDUsage.getUsageIds( funcProxy.elUsage );
            keys = "%_%".format( *keyArray ).asSymbol;
        };
        if ( funcProxy.elUsage.isNil ){
            keys = \anyUsage;
        };
        ^keys
        */
        ^(funcProxy.elUsage ? \anyUsage); // how does this deal with nil?
    } // here's the top level dispatch key - in our case an array of page, usage

    // these arguments are different from HID:
    // value, physValue, rawValue, arrayValue, usage, page, elid, devid
	value { |value, physValue, rawValue, arrayValue, usage, page, elid, devid|
        var key = HIDUsage.idsToName( page, usage );
        active[key].value(
            [value, physValue, rawValue, arrayValue],
            [usage, page],
            elid, devid,
            key
        );
        // this seems kind of ugly!
        active[\anyUsage].value(
            [value, physValue, rawValue, arrayValue],
            [usage, page],
            elid, devid,
            key
        );
    }

	register {
		// add here to the low level hooks
        HID.addRecvFunc( this );
		registered = true;
	}

	unregister {
		// remove here from the low level hooks
        HID.removeRecvFunc( this );
		registered = false;
	}

    /* not sure if I need to override
    add {|funcProxy|
		var func, keys;
		funcProxy.addDependant(this);
		func = this.wrapFunc(funcProxy); // here we wrap the responder object's func(s) in matchers if needed
		wrappedFuncs[funcProxy] = func;
		keys = this.getKeysForFuncProxy(funcProxy); // this method figures out what top level keys will match
		keys.do({|key| active[key] = active[key].addFunc(func) }); // support multiple keys
		if(registered.not, {this.register});
	}
    */

	// wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.devUsage;
		argTemplate = funcProxy.argTemplate;
		// here test for various matching possibilities
		// for HID should probably look something like this.
        //if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)} );
		^case(
			{ srcID.notNil }, { HIDFuncSrcUsageMatcher(srcID, func) },
			{ func }
        );
        //^func;
	}

	typeKey { ^'HID' }
}


// since HID is arbitrarily extensible, I suggest just a simple key, like an OSC address
// the dispatcher could possible construct this if needed
HIDFunc : AbstractResponderFunc {
	classvar <>defaultDispatcher, traceFunc, traceRunning = false;
    var <type;
    var <elUsage, <devUsage, <deviceTemplate, <argTemplate;
    var <argTemplate;

	*initClass {
		defaultDispatcher = HIDUsageDispatcher.new;
		// not sure what this should look like, but a trace func would be nice
        traceFunc = { |value, physValue, rawValue, arrayValue, usage, page, elid, devid, dev|
            // devid, dev, elid, page, usage, rawValue, value|
            "HID Element Data:\n\tdevid: %, elid: %\n\t%\n\tdevice: \t page: % \tusage: %\n\telement: \t page: %\tusage: %\tarray value: %,\traw value: %,\tphysical value: %,\tvalue: %\n"
            .postf( devid, elid, dev.info, dev.usagePage, dev.usage, page, usage, arrayValue, rawValue, physValue, value );
		}
	}

	*new { arg func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher;
		^super.new.init(func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}

    /// this is the same as *new
    *usage { arg func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher;
		^super.new.init(func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}

    /*
    *usageID { arg func, elUsageID, elPageID, devUsageID, devPageID, deviceInfo, argTemplate, dispatcher;
		^super.new.initUsageID(func, elUsageID, elPageID, devUsageID, devPageID, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}

    *device { arg func, elUsageName, deviceName, deviceInfo, argTemplate, dispatcher;
		^super.new.initDevice(func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}

    *element { arg func, elID, deviceName, deviceInfo, argTemplate, dispatcher;
		^super.new.initElement(func, elID, devUsageName, deviceInfo, argTemplate, dispatcher ? defaultDispatcher);
	}
    */

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

	init { | argFunc, elUsageName, devUsageName, deviceInfo, argtemplate, argdisp |
		type = \usage;
        elUsage = elUsageName ? elUsage;
        devUsage = devUsageName ? devUsage;
        deviceTemplate = deviceInfo ? deviceTemplate;
        argTemplate = argtemplate ? argTemplate;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;
		this.enable;
		allFuncProxies.add(this);
	}

    printOn { arg stream; stream << this.class.name << "." << type << "("<<* [ elUsage, devUsage, deviceTemplate, argTemplate] << ")" }

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

    printOn { arg stream; stream << this.class.name << "." << type << "("<<* [ elUsage, devUsage, deviceTemplate, argTemplate] << ")" }

	*freeAll {
		var objs = all.copy;
		objs.do(_.free)
	}
}

// matching wrappers
// if you need to test for srcID func gets wrapped in this
HIDFuncSrcUsageMatcher : AbstractMessageMatcher {
	var devUsage;
    var matchingDevs;
    var notMatchingDevs;

	*new {|srcID, func| ^super.new.init(srcID, func);}

	init {|argsrcID, argfunc|
        devUsage = argsrcID;
        func = argfunc;
        matchingDevs = [];
        notMatchingDevs = [];
        devUsage.do{ |it| this.addDevUsage( it ) };
    }

    addDevUsage{ |du|
        // find ids of already open devices that match:
        matchingDevs = matchingDevs ++
        HID.openDevices.asArray.select{ |hid|
            du == hid.usage.asSymbol
        };
        notMatchingDevs = notMatchingDevs ++
        HID.openDevices.asArray.select{ |hid|
            du != hid.usage.asSymbol
        };
    }

    checkDevUsage{ |du,devid|
        if ( HID.openDevices.at( devid ).usageName == du ){
            matchingDevs = matchingDevs.add( devid );
            ^true;
        }{
            notMatchingDevs = notMatchingDevs.add( devid );
            ^false;
        };
    }

    // question whether this is faster than direct lookup of device usage
	value { |vals, usageIDs, elid, devid, key|
        if ( matchingDevs.includes( devid ) ){
            func.value( vals, usageIDs, elid, devid, key );
        }{
            if ( notMatchingDevs.includes( devid ).not ){ // perhaps devices was opened after HIDFunc was created
                devUsage.do{ |it|
                    if ( this.checkDevUsage( it, devid ) ){
                        func.value( vals, usageIDs, elid, devid, key );
                    };
                };
            };
        };
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
