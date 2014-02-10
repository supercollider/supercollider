///////////////////// HID

/* There's a question here of how to construct the dispatchers. The general idea is to use the 'most significant' parameter as an initial filter for speed reasons, and then use wrappers to test other matches. In this case I'd suggest dispatching via a single key (perhaps constructed), but it depends on the lower level implementation if that makes sense. */

HIDUsageDispatcher : AbstractWrappingDispatcher {

	*new { ^super.new; }

    typeKey { ^'HID' }

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

	getKeysForFuncProxy {|funcProxy|
        ^(funcProxy.elUsage ? \anyUsage); // how does this deal with nil?
    } // here's the top level dispatch key - in our case an array of page, usage

    // these arguments are different from HID:
    // value, physValue, rawValue, arrayValue, usage, page, elid, devid
	value { |value, rawValue, usage, page, elid, ele, devid, device|
        // |value, physValue, rawValue, arrayValue, usage, page, elid, devid|
        var key = HIDUsage.idsToName( page, usage );
        // ["usage", key, value, physValue, rawValue, arrayValue, usage, page, elid, devid].postln;
        active[key].value(
            value, rawValue,
            usage, page,
            elid, ele,
            devid, device,
            key
        );
        // this seems kind of ugly!
        active[\anyUsage].value(
            value, rawValue,
            usage, page,
            elid, ele,
            devid, device,
            \anyUsage
        );
    }

	// wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.devUsage;
		argTemplate = funcProxy.argTemplate;
        // TODO: filter on deviceInfo!

		// here test for various matching possibilities
		// for HID should probably look something like this.

        // TODO: if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)} );

		^case(
			{ srcID.notNil }, { HIDFuncSrcUsageMatcher(srcID, func) },
			{ func }
        );
        //^func;
	}

}

HIDUsageIDDispatcher : HIDUsageDispatcher {

    *new { ^super.new; }

    getKeysForFuncProxy {|funcProxy|
        var keyArray;
        var keys;
        if ( funcProxy.elUsage.isKindOf( Array ) or: funcProxy.elPage.isKindOf( Array )){
            // nil -> 'nil'
            [ funcProxy.elPage, funcProxy.elUsage ].flop.collect{ |fpu|
                "%_%".format( *fpu ).asSymbol;
            };
        }{
            // nil -> 'nil'
            keys = "%_%".format( funcProxy.elPage, funcProxy.elUsage ).asSymbol;
        };
        ^keys
    } // here's the top level dispatch key - in our case an array of page, usage

    // these arguments are from HID:
    // value, physValue, rawValue, arrayValue, usage, page, elid, devid
	value { |value, rawValue, usage, page, elid, ele, devid, device|
        // |value, physValue, rawValue, arrayValue, usage, page, elid, devid|
        var keys = [
            "%_%".format( page, usage ).asSymbol,
            "nil_%".format( usage ).asSymbol,
            "%_nil".format( page ).asSymbol,
            'nil_nil'
        ];
        // ["usageIDs", keys, value, physValue, rawValue, arrayValue, usage, page, elid, devid].postln;
        keys.do{ |key|
            active[key].value(
                value, rawValue,
                usage, page,
                elid, ele,
                devid, device,
                key
            );
        };
    }

    // wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.devUsage;
        argTemplate = funcProxy.argTemplate;
        // TODO: filter on deviceInfo!

		// here test for various matching possibilities
		// for HID should probably look something like this.

        // TODO: if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)} );

		^case(
			{ srcID.notNil }, { HIDFuncSrcNameMatcher(srcID, func) },
			{ func }
        );
        //^func;
	}
}

HIDDeviceDispatcher : HIDUsageDispatcher {

    	// wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.devUsage;
		argTemplate = funcProxy.argTemplate;
        // TODO: filter on deviceInfo!

		// here test for various matching possibilities
		// for HID should probably look something like this.

        // TODO: if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)} );

		^case(
			{ srcID.notNil }, { HIDFuncSrcNameMatcher(srcID, func) },
			{ func }
        );
        //^func;
	}

}

HIDElementDispatcher : HIDUsageDispatcher {

	getKeysForFuncProxy {|funcProxy|
        var keys = funcProxy.elUsage ? \anyElement;
        if ( keys.isKindOf( Array ) ){
            ^keys;
        }{
            ^keys.bubble; // how does this deal with nil?
        };
    } // here's the top level dispatch key - in our case an array of page, usage

    // these arguments are from HID:
    // value, physValue, rawValue, arrayValue, usage, page, elid, devid
	value { |value, rawValue, usage, page, elid, ele, devid, device|
        // |value, physValue, rawValue, arrayValue, usage, page, elid, devid|
        // active.postcs;
        // ["element", value, physValue, rawValue, arrayValue, usage, page, elid, devid].postln;
        active[ elid ].value(
            value, rawValue,
            usage, page,
            elid, ele,
            devid, device,
            elid
        );
        // this seems kind of ugly!
        active[\anyElement].value(
            value, rawValue,
            usage, page,
            elid, ele,
            devid, device,
            \anyElement
        );
    }

    // wrapper objects based on arg type and testing requirements
	wrapFunc {|funcProxy|
		var func, srcID, argTemplate;
		func = funcProxy.func;
		srcID = funcProxy.devUsage;
		argTemplate = funcProxy.argTemplate;
        // TODO: filter on deviceInfo!

		// here test for various matching possibilities
		// for HID should probably look something like this.

        // TODO: if(argTemplate.notNil, { func = HIDValueMatcher(argTemplate, func)} );

		^case(
			{ srcID.notNil }, { HIDFuncSrcNameMatcher(srcID, func) },
			{ func }
        );
        //^func;
	}
}


// since HID is arbitrarily extensible, I suggest just a simple key, like an OSC address
// the dispatcher could possible construct this if needed
HIDFunc : AbstractResponderFunc {
	classvar <>defaultDispatchers, traceFunc, traceRunning = false;
    var <type;
    var <elUsage, <devUsage, <deviceTemplate, <argTemplate;
    var <elPage, <devPage;
    var <argTemplate;

	*initClass {
        defaultDispatchers = IdentityDictionary.new;
        defaultDispatchers[\usage] = HIDUsageDispatcher.new;
        defaultDispatchers[\usageID] = HIDUsageIDDispatcher.new;
        defaultDispatchers[\device] = HIDDeviceDispatcher.new;
        defaultDispatchers[\element] = HIDElementDispatcher.new;
		// not sure what this should look like, but a trace func would be nice
        traceFunc = {
            |value, rawValue, usage, page, elid, ele, devid, dev|
            // |value, physValue, rawValue, arrayValue, usage, page, elid, devid, dev|
            "HID Element Data:\n\tdevid: %, elid: %\n\t%\n\tdevice: \t page: % \tusage: %\n\telement: \t page: %\tusage: %\traw value: %,\t,\tvalue: %\n"
            .postf( devid, elid, dev.info, dev.usagePage, dev.usage, page, usage, rawValue, value );
		}
	}

    /* how to do the variable number of arguments here?
	*new { arg type, func, ..filterArgs ;
		^super.new.init( type ? \usage, func, filterArgs);
	}
    */

    *usage { arg func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher;
        ^super.new.initUsage( func, elUsageName, devUsageName, deviceInfo, argTemplate, dispatcher ? defaultDispatchers[\usage] );
	}

    *usageID { arg func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, dispatcher;
        ^super.new.initUsageID(func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, dispatcher ? defaultDispatchers[ \usageID ]);
	}

    *device { arg func, elUsageName, deviceName, deviceInfo, argTemplate, dispatcher;
        ^super.new.initDevice(func, elUsageName, deviceName, deviceInfo, argTemplate, dispatcher ? defaultDispatchers[ \device ]);
	}

    *element { arg func, elID, deviceName, deviceInfo, argTemplate, dispatcher;
        ^super.new.initElement(func, elID, deviceName, deviceInfo, argTemplate, dispatcher ? defaultDispatchers[ \element ]);
	}

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

    init { | tp, argFunc, usageArgs |
		type = tp ? type;
        dispatcher = usageArgs.last ? defaultDispatchers[ type ];
        switch( type,
            \usage, { this.initUsage( *([argFunc] ++ usageArgs) ) },
            \usageID, { this.initUsageID( *([argFunc] ++ usageArgs) ) },
            \device, { this.initDevice( *([argFunc] ++ usageArgs) ) },
            \element, { this.initElement( *([argFunc] ++ usageArgs) ) }
        );
	}

	initUsage { | argFunc, elUsageName, devUsageName, deviceInfo, argtemplate, argdisp |
		type = \usage;
        elUsage = elUsageName ? elUsage;
        devUsage = devUsageName ? devUsage;
        deviceTemplate = deviceInfo ? deviceTemplate;
        argTemplate = argtemplate ? argTemplate;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;
        // TODO: something useful with deviceInfo
		this.enable;
		allFuncProxies.add(this);
	}

    initUsageID { | argFunc, elUsageID, elPageID, devName, deviceInfo, argtemplate, argdisp |
		type = \usageID;
        elUsage = elUsageID ? elUsage;
        elPage = elPageID ? elPage;
        devUsage = devName ? devUsage;
        deviceTemplate = deviceInfo ? deviceTemplate;
        argTemplate = argtemplate ? argTemplate;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;
        // TODO: something useful with deviceInfo
		this.enable;
		allFuncProxies.add(this);
	}

    initDevice{ |argFunc, elUsageName, devName, deviceInfo, argtemplate, argdisp |
        type = \device;
        elUsage = elUsageName ? elUsage;
        devUsage = devName ? devUsage;
        deviceTemplate = deviceInfo ? deviceTemplate;
        argTemplate = argtemplate ? argTemplate;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;
        // TODO: something useful with deviceInfo
		this.enable;
		allFuncProxies.add(this);
    }

    initElement{ |argFunc, elID, devName, deviceInfo, argtemplate, argdisp |
        type = \element;
        elUsage = elID ? elUsage;
        devUsage = devName ? devUsage;
        deviceTemplate = deviceInfo ? deviceTemplate;
        argTemplate = argtemplate ? argTemplate;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;
        // TODO: something useful with deviceInfo
		this.enable;
		allFuncProxies.add(this);
    }

    printOn { arg stream;
        // TODO: versions per type
        stream << this.class.name << "." << type << "("<<* [ elUsage, devUsage, deviceTemplate, argTemplate] << ")"
    }

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
    var <matchingDevs;
    var <notMatchingDevs;

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
        HID.openDevices.select{ |hid|
            du == hid.usageName.asSymbol
        }.keys.asArray;
        notMatchingDevs = notMatchingDevs ++
        HID.openDevices.select{ |hid|
            du != hid.usageName.asSymbol
        }.keys.asArray;
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
	value { |value,rawValue,usage,page, elid, ele, devid, device, key|
        // |vals, usageIDs, elid, devid, key|
        if ( matchingDevs.includes( devid ) ){
            func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
                // vals, usageIDs, elid, devid, key );
        }{
            if ( notMatchingDevs.includes( devid ).not ){ // perhaps device was opened after HIDFunc was created
                devUsage.do{ |it|
                    if ( this.checkDevUsage( it, devid ) ){
                        func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
                    };
                };
            };
        };
	}
}


HIDFuncSrcNameMatcher : AbstractMessageMatcher {
	var devName;
    var <matchingDevs;
    var <notMatchingDevs;

	*new {|srcID, func| ^super.new.init(srcID, func);}

	init {|argsrcID, argfunc|
        devName = argsrcID;
        if ( devName.isKindOf( String ) ){
            devName = devName.bubble;
        };
        func = argfunc;
        matchingDevs = [];
        notMatchingDevs = [];
        devName.do{ |it| this.addDevName( it ) };
    }

    addDevName{ |du|
        // find ids of already open devices that match:
        matchingDevs = matchingDevs ++
        HID.openDevices.select{ |hid|
            du == hid.info.productName or: ( du == (hid.info.vendorName + hid.info.productName ) )
        }.keys.asArray;
        notMatchingDevs = notMatchingDevs ++
        HID.openDevices.select{ |hid|
            du != hid.info.usageName and: ( du == (hid.info.vendorName + hid.info.productName ) )
        }.keys.asArray;
    }

    checkDevName{ |du,devid|
        var thisdev = HID.openDevices.at( devid );
        // [du,devid,thisdev].postln;
        if (
            thisdev.info.productName == du or:
            ((thisdev.info.vendorName + thisdev.info.productName) == du)
        ){
            matchingDevs = matchingDevs.add( devid );
            ^true;
        }{
            notMatchingDevs = notMatchingDevs.add( devid );
            ^false;
        };
    }

    // question whether this is faster than direct lookup of device usage
	value { |value,rawValue,usage,page, elid, ele, devid, device, key|
        // |vals, usageIDs, elid, devid, key|
        // [ "srcName matcher", vals, usageIDs, elid, devid, key ].postln;
        if ( matchingDevs.includes( devid ) ){
            // "-->evaluating".postln;
            func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
        }{
            if ( notMatchingDevs.includes( devid ).not ){ // perhaps devices was opened after HIDFunc was created
                devName.do{ |it|
                    if ( this.checkDevName( it, devid ) ){
                        func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
                    };
                };
            };
        };
	}
}
// matching wrappers
// if you need to test for srcID func gets wrapped in this
HIDFuncSrcUsageIDMatcher : AbstractMessageMatcher {
	var devUsage;
    var <matchingDevs;
    var <notMatchingDevs;
	var devPage;

	*new {|devUsage, devPage, func|
        ^super.new.init(devUsage, devPage, func);
    }

	init {|argdevUsage, argdevPage, argfunc|
        devUsage = argdevUsage;
        devPage = argdevPage;
        func = argfunc;
        matchingDevs = [];
        notMatchingDevs = [];
        [ devUsage, devPage ].flop.do{ |dupair| this.addDevUsage( *dupair ) };
    }

    addDevUsage{ |usage,page|
        // find ids of already open devices that match:
        if ( usage.isNil ){ // usage is nil
            matchingDevs = matchingDevs ++
            HID.openDevices.select{ |hid|
                page == hid.usagePage
            }.keys.asArray;
            notMatchingDevs = notMatchingDevs ++
            HID.openDevices.select{ |hid|
                page != hid.usagePage
            }.keys.asArray;
        }{ // usage not nil
            if ( page.isNil ){ // page is nil
                matchingDevs = matchingDevs ++
                HID.openDevices.select{ |hid|
                    usage == hid.usage
                }.keys.asArray;
                notMatchingDevs = notMatchingDevs ++
                HID.openDevices.select{ |hid|
                    usage != hid.usage
                }.keys.asArray;
            }{ // page not nil
                matchingDevs = matchingDevs ++
                HID.openDevices.select{ |hid|
                    ( page == hid.usagePage  ) and: ( usage == hid.usage  )
                }.keys.asArray;
                notMatchingDevs = notMatchingDevs ++
                HID.openDevices.select{ |hid|
                    ( page != hid.usagePage  ) or: ( usage != hid.usage  )
                }.keys.asArray;
            }
        }
    }

    checkDevUsage{ |dupair,devid|
        if ( dupair[0].isNil ){ // usage is nil
            if ( HID.openDevices.at( devid ).usagePage == dupair[1] ){
                matchingDevs = matchingDevs.add( devid );
                ^true;
            }{
                notMatchingDevs = notMatchingDevs.add( devid );
                ^false;
            };
        };
        if ( dupair[1].isNil ){ // page is nil
            if ( HID.openDevices.at( devid ).usage == dupair[0] ){
                matchingDevs = matchingDevs.add( devid );
                ^true;
            }{
                notMatchingDevs = notMatchingDevs.add( devid );
                ^false;
            };
        };
        if ( ( HID.openDevices.at( devid ).usage == dupair[0] ) and: (HID.openDevices.at( devid ).usagePage == dupair[1] ) ){
            matchingDevs = matchingDevs.add( devid );
            ^true;
        }{
            notMatchingDevs = notMatchingDevs.add( devid );
            ^false;
        };
    }

    // question whether this is faster than direct lookup of device usage
	value { |value,rawValue,usage,page, elid, ele, devid, device, key|
        if ( matchingDevs.includes( devid ) ){
            func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
        }{
            if ( notMatchingDevs.includes( devid ).not ){ // perhaps device was opened after HIDFunc was created
                [ devUsage, devPage ].flop.do{ |it|
                    if ( this.checkDevUsage( it, devid ) ){
                        func.value( value,rawValue,usage,page, elid, ele, devid, device, key );
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
