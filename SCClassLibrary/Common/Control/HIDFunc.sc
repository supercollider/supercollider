HIDUsageDispatcher : AbstractWrappingDispatcher {

	*new { ^super.new; }

	typeKey { ^'HID' }

	register {
		HID.addRecvFunc( this );
		registered = true;
	}

	unregister {
		HID.removeRecvFunc( this );
		registered = false;
	}

	getKeysForFuncProxy { | funcProxy|
		^(funcProxy.elUsage ? \anyUsage);
	} // here's the top level dispatch key - in our case an array of page, usage

	value { | value, rawValue, usage, page, elid, ele, devid, device|
		var key = HIDUsage.idsToName( page, usage );
		active[key].value(
			value, rawValue,
			usage, page,
			elid, ele,
			devid, device,
			key
		);
		active[\anyUsage].value(
			value, rawValue,
			usage, page,
			elid, ele,
			devid, device,
			\anyUsage
		);
	}

	// wrapper objects based on arg type and testing requirements
	wrapFunc { | funcProxy|
		var func, srcID, srcTemplate, argTemplate;
		func = funcProxy.func;
		srcTemplate = funcProxy.deviceTemplate;
		argTemplate = funcProxy.argTemplate;

		if ( argTemplate.notNil ) {
			if ( funcProxy.argTemplateType == \rawValue ) {
				func = HIDRawValueMatcher(argTemplate, func);
			} {
				func = HIDValueMatcher(argTemplate, func);
			}
		};

		^case(
			{ srcTemplate.notNil }, { HIDFuncDeviceProtoMatcher( srcTemplate, func ) },
			{ func }
		);
	}
}

HIDDeviceDispatcher : HIDUsageDispatcher {

	// wrapper objects based on arg type and testing requirements
	wrapFunc { | funcProxy|
		var func, srcTemplate, argTemplate;
		func = funcProxy.func;
		srcTemplate = funcProxy.deviceTemplate;
		argTemplate = funcProxy.argTemplate;

		if ( argTemplate.notNil ) {
			if ( funcProxy.argTemplateType == \rawValue ) {
				func = HIDRawValueMatcher(argTemplate, func);
			} {
				func = HIDValueMatcher(argTemplate, func);
			}
		};

		^case(
			{ srcTemplate.notNil }, { HIDFuncDeviceProtoMatcher( srcTemplate, func ) },
			{ func }
		);
	}
}

HIDElementProtoDispatcher : AbstractWrappingDispatcher {

	*new { ^super.new; }

	typeKey { ^'HID' }

	register {
		HID.addRecvFunc( this );
		registered = true;
	}

	unregister {
		HID.removeRecvFunc( this );
		registered = false;
	}

	getKeysForFuncProxy { | funcProxy|
		^\proto;
	} // here's the top level dispatch key - in our case an array of page, usage

	// these arguments are from HID:
	value { | value, rawValue, usage, page, elid, ele, devid, device|
		active[ \proto ].value(
			value, rawValue,
			usage, page,
			elid, ele,
			devid, device,
			\proto
		);
	}

	// wrapper objects based on arg type and testing requirements
	wrapFunc { | funcProxy|
		var func, srcID, eleTemplate, srcTemplate, argTemplate;
		func = funcProxy.func;
		eleTemplate = funcProxy.elementTemplate;
		srcTemplate = funcProxy.deviceTemplate;
		argTemplate = funcProxy.argTemplate;

		if ( argTemplate.notNil ) {
			if ( funcProxy.argTemplateType == \rawValue ) {
				func = HIDRawValueMatcher(argTemplate, func);
			} {
				func = HIDValueMatcher(argTemplate, func);
			}
		};

		if ( eleTemplate.notNil ) {
			func = HIDFuncElementProtoMatcher( eleTemplate, func );
		};

		^case(
			{ srcTemplate.notNil }, { HIDFuncDeviceProtoMatcher( srcTemplate, func ) },
			{ func }
		);
	}
}

HIDElementDispatcher : HIDUsageDispatcher {

	getKeysForFuncProxy { | funcProxy|
		var keys = funcProxy.elUsage ? \anyElement;
		if ( keys.isKindOf( Array ) ) {
			^keys;
		} {
			^keys.bubble; // how does this deal with nil?
		};
	} // here's the top level dispatch key - in our case an array of page, usage

	// these arguments are from HID:
	value { | value, rawValue, usage, page, elid, ele, devid, device|
		active[ elid ].value(
			value, rawValue,
			usage, page,
			elid, ele,
			devid, device,
			elid
		);
		active[\anyElement].value(
			value, rawValue,
			usage, page,
			elid, ele,
			devid, device,
			\anyElement
		);
	}

	wrapFunc { | funcProxy|
		var func, srcID, srcTemplate, argTemplate;
		func = funcProxy.func;
		srcTemplate = funcProxy.deviceTemplate;
		argTemplate = funcProxy.argTemplate;

		if ( argTemplate.notNil ) {
			if ( funcProxy.argTemplateType == \rawValue ) {
				func = HIDRawValueMatcher(argTemplate, func);
			} {
				func = HIDValueMatcher(argTemplate, func);
			}
		};

		^case(
			{ srcTemplate.notNil }, { HIDFuncDeviceProtoMatcher( srcTemplate, func ) },
			{ func }
		);
	}
}

HIDFunc : AbstractResponderFunc {
	classvar <>defaultDispatchers, traceFunc, traceRunning = false;
	var <type;

	var <elUsage, <devUsage;
	//var <elPage, <devPage;

	var <elementTemplate;
	var <deviceTemplate;
	var <argTemplate;
	var <argTemplateType;

	*initClass {
		defaultDispatchers = IdentityDictionary.new;
		defaultDispatchers[\usage] = HIDUsageDispatcher.new;
		defaultDispatchers[\usageID] = HIDElementProtoDispatcher.new;
		defaultDispatchers[\device] = HIDDeviceDispatcher.new;
		defaultDispatchers[\proto] = HIDElementProtoDispatcher.new;
		defaultDispatchers[\element] = HIDElementDispatcher.new;

		traceFunc = {
			| value, rawValue, usage, page, elid, ele, devid, dev|
			"HID Element Data:\tdevid: %, elid: %\n\tdevice: \t%, \tpage: % \tusage: %\n\telement:\t%\n\t\t\t\tpage: %\tusage: %\traw value: %,\tvalue: %\n"
			.postf( devid, elid, dev.info, dev.usagePage, dev.usage, ele, page, usage, rawValue, value );
		}
	}

	/* how to do the variable number of arguments here?
	*new { arg type, func, ..filterArgs ;
	^super.new.init( type ? \usage, func, filterArgs);
	}
	*/

	*usage { arg func, elUsageName, devUsageName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		^super.new.initUsage( func, elUsageName, devUsageName, deviceInfo, argTemplate,argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[\usage]);
	}

	*usageID { arg func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		^super.new.initUsageID(func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \usageID ]);
	}

	*device { arg func, elUsageName, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		^super.new.initDevice(func, elUsageName, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \device ]);
	}

	*proto { arg func, protoElement, deviceInfo, argTemplate, argTemplateType, dispatcher;
		^super.new.initProtoElement(func, protoElement, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \proto ]);
	}

	*element { arg func, elID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		^super.new.initElement(func, elID, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \element ]);
	}

	*trace { | bool = true|
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
		argTemplateType = \rawValue;
		dispatcher = usageArgs.last ? defaultDispatchers[ type ];
		switch( type,
			\usage, { this.initUsage( *([argFunc] ++ usageArgs) ) },
			\usageID, { this.initUsageID( *([argFunc] ++ usageArgs) ) },
			\device, { this.initDevice( *([argFunc] ++ usageArgs) ) },
			\proto, { this.initProtoElement( *([argFunc] ++ usageArgs) ) },
			\element, { this.initElement( *([argFunc] ++ usageArgs) ) }
		);
	}

	initUsage { | argFunc, elUsageName, devUsageName, deviceInfo, argtemplate, argtemplatetype, argdisp |
		type = \usage;

		elUsage = elUsageName ? elUsage;
		devUsage = devUsageName ? devUsage;

		if ( devUsage.notNil ) {
			deviceTemplate = HIDProto.newType( devUsage );
		};

		if ( deviceInfo.notNil ) {
			if ( deviceInfo.isKindOf( HIDProto ) ) {
				deviceTemplate = deviceInfo;
			} {
				deviceTemplate = HIDProto.newFromDict( deviceInfo );
			}
		};

		argTemplate = argtemplate ? argTemplate;
		argTemplateType = argtemplatetype ? argTemplateType;

		func = argFunc ? func;

		dispatcher = argdisp ? dispatcher;

		this.enable;
		allFuncProxies.add(this);
	}

	initUsageID { | argFunc, elUsageID, elPageID, devName, deviceInfo, argtemplate, argtemplatetype, argdisp |
		type = \usageID;

		devUsage = devName ? devUsage;

		elementTemplate = HIDElementProto.newTypeID( elUsageID, elPageID );

		if ( devUsage.notNil ) {
			deviceTemplate = HIDProto.newProduct( devUsage );
		};
		if ( deviceInfo.notNil ) {
			if ( deviceInfo.isKindOf( HIDProto ) ) {
				deviceTemplate = deviceInfo;
			} {
				deviceTemplate = HIDProto.newFromDict( deviceInfo );
			}
		};

		argTemplate = argtemplate ? argTemplate;
		argTemplateType = argtemplatetype ? argTemplateType;
		func = argFunc ? func;
		dispatcher = argdisp ? dispatcher;

		this.enable;
		allFuncProxies.add(this);
	}

	initDevice { | argFunc, elUsageName, devName, deviceInfo, argtemplate, argtemplatetype, argdisp |
		type = \device;
		elUsage = elUsageName ? elUsage;
		devUsage = devName ? devUsage;

		if ( devUsage.notNil ) {
			deviceTemplate = HIDProto.newProduct( devUsage );
		};
		if ( deviceInfo.notNil ) {
			if ( deviceInfo.isKindOf( HIDProto ) ) {
				deviceTemplate = deviceInfo;
			} {
				deviceTemplate = HIDProto.newFromDict( deviceInfo );
			}
		};
		dispatcher = argdisp ? dispatcher;

		argTemplate = argtemplate ? argTemplate;
		argTemplateType = argtemplatetype ? argTemplateType;
		func = argFunc ? func;

		this.enable;
		allFuncProxies.add(this);
	}

	initProtoElement { | argFunc, protoElement, deviceInfo, argtemplate, argtemplatetype, argdisp |
		type = \proto;

		elementTemplate = protoElement;

		if ( deviceInfo.notNil ) {
			if ( deviceInfo.isKindOf( HIDProto ) ) {
				deviceTemplate = deviceInfo;
			} {
				deviceTemplate = HIDProto.newFromDict( deviceInfo );
			}
		};

		argTemplate = argtemplate ? argTemplate;
		argTemplateType = argtemplatetype ? argTemplateType;
		func = argFunc ? func;

		dispatcher = argdisp ? dispatcher;

		this.enable;
		allFuncProxies.add(this);
	}

	initElement { | argFunc, elID, devName, deviceInfo, argtemplate, argtemplatetype, argdisp |
		type = \element;
		elUsage = elID ? elUsage;
		devUsage = devName ? devUsage;

		argTemplate = argtemplate ? argTemplate;
		argTemplateType = argtemplatetype ? argTemplateType;
		func = argFunc ? func;

		dispatcher = argdisp ? dispatcher;

		if ( devUsage.notNil ) {
			deviceTemplate = HIDProto.newProduct( devUsage );
		};

		if ( deviceInfo.notNil ) {
			if ( deviceInfo.isKindOf( HIDProto ) ) {
				deviceTemplate = deviceInfo;
			} {
				deviceTemplate = HIDProto.newFromDict( deviceInfo );
			}
		};

		dispatcher = argdisp ? dispatcher;
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

	*new { arg key, func;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^nil;
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					switch( res.type,
						\usage,
						{
							res.initUsage(func, res.elUsage, res.devUsage, res.deviceTemplate, res.argTemplate, res.argTemplateType ? \rawValue, res.dispatcher ? defaultDispatchers[\usage]);
						},
						\usageID, {
							res.initUsageID(func, res.elUsage, res.elementTemplate.usagePage, res.deviceUsage, res.deviceTemplate, res.argTemplate, res.argTemplateType ? \rawValue, res.dispatcher ? defaultDispatchers[ \usageID ])
						},
						\device, {
							res.initDevice(func, res.elUsage, res.devUsage, res.deviceTemplate, res.argTemplate, res.argTemplateType ? \rawValue, res.dispatcher ? defaultDispatchers[ \device ])
						},
						\proto, {
							res.initProtoElement(func, res.elementTemplate, res.deviceTemplate, res.argTemplate, res.argTemplateType ? \rawValue, res.dispatcher ? defaultDispatchers[ \proto ]);
						},
						\element, {
							res.initElement(func, res.elUsage, res.devUsage, res.deviceTemplate, res.argTemplate, res.argTemplateType ? \rawValue, res.dispatcher ? defaultDispatchers[ \element ]);
						}
					);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	*usage { arg key, func, elUsageName, devUsageName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.usage( func, elUsageName, devUsageName, deviceInfo, argTemplate, argTemplateType, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.initUsage(func, elUsageName, devUsageName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[\usage]);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	*usageID { arg key, func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.usageID( func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.initUsageID(func, elUsageID, elPageID, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \usageID ]);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}


	*device { arg key, func, elUsageName, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.device( func, elUsageName, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.initDevice(func, elUsageName, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \device ]);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	*proto { arg key, func, protoElement, deviceInfo, argTemplate, argTemplateType, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.proto( func, protoElement, deviceInfo, argTemplate, argTemplateType, dispatcher ).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.initProtoElement(func, protoElement, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \proto ]);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	*element { arg key, func, elID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher;
		var res = all.at(key), wasDisabled;
		if(res.isNil) {
			^super.element( key, func, elID, deviceName, deviceInfo, argTemplate, argTemplateType, dispatcher ).addToAll(key);
		} {
			if(func.notNil) {
				wasDisabled = res.enabled.not;
				res.disable;
				try {
					res.initElement(func, elID, deviceName, deviceInfo, argTemplate, argTemplateType ? \rawValue, dispatcher ? defaultDispatchers[ \element ]);
					if(wasDisabled, { res.disable; });
				} { | err|
					res.free;
					err.throw;
				}
			}
		}
		^res
	}

	addToAll { | argkey| key = argkey; all.put(key, this) }

	free { all[key] = nil; super.free; }

	printOn { arg stream;
		stream << this.class.name << "." << type << "("<<* [ key, elUsage, devUsage, deviceTemplate, argTemplate] << ")"
	}

	*freeAll {
		var objs = all.copy;
		objs.do(_.free)
	}
}


// matching wrappers

HIDFuncDeviceProtoMatcher : AbstractMessageMatcher {
	var <devProto;

	var <matchingDevs;
	var <notMatchingDevs;

	initMatching { | argfunc|
		func = argfunc;
		matchingDevs = [];
		notMatchingDevs = [];
	}

	*new { | srcID, func| ^super.new.init(srcID, func);}

	init { | argsrcID, argfunc|
		devProto = argsrcID;
		this.initMatching( argfunc );
		this.checkMatchingDevs;
	}

	checkMatchingDevs {
		// find ids of already open devices that match:
		matchingDevs = matchingDevs ++
		HID.openDevices.select { | hid|
			devProto.matches( hid );
		}.keys.asArray;
		notMatchingDevs = notMatchingDevs ++
		HID.openDevices.select { | hid|
			devProto.matches( hid ).not;
		}.keys.asArray;
	}

	checkMatchingDev { | devid|
		var hid = HID.openDevices.at( devid );
		if ( devProto.matches( hid ) ) {
			matchingDevs = matchingDevs.add( devid );
			^true;
		} {
			notMatchingDevs = notMatchingDevs.add( devid );
			^false;
		};
	}

	value { | value, rawValue, usage, page, elid, ele, devid, device, key|
		if ( matchingDevs.includes( devid ) ) {
			func.value( value, rawValue, usage, page, elid, ele, devid, device, key );
		} {
			if ( notMatchingDevs.includes( devid ).not ) {
				// perhaps device was opened after HIDFunc was created
				if ( this.checkMatchingDev( devid ) ) {
					func.value( value, rawValue, usage, page, elid, ele, devid, device, key );
				};
			};
		};
	}
}

HIDFuncElementProtoMatcher : AbstractMessageMatcher {
	var elementProto;

	*new { | elProto, func| ^super.new.init(elProto, func);}

	init { | elProto, argfunc|
		elementProto = elProto;
		func = argfunc;
	}

	value { | value, rawValue, usage, page, elid, ele, devid, device, key|
		if ( elementProto.matches( ele ) ) {
			func.value( value, rawValue, usage, page, elid, ele, devid, device, key );
		}
	}
}

HIDValueMatcher : AbstractMessageMatcher {
	var argTemplate;

	*new { | argTemplate, func| ^super.new.init(argTemplate, func) }

	init { | argArgTemplate, argFunc| argTemplate = argArgTemplate; func = argFunc; }

	value { | ...testMsg|
		if(argTemplate.matchItem(testMsg[0]), { func.value(*testMsg)});
	}
}

HIDRawValueMatcher : AbstractMessageMatcher {
	var argTemplate;

	*new { | argTemplate, func| ^super.new.init(argTemplate, func) }

	init { | argArgTemplate, argFunc| argTemplate = argArgTemplate; func = argFunc; }

	value { | ...testMsg|
		if(argTemplate.matchItem(testMsg[1]), { func.value(*testMsg)});
	}
}
