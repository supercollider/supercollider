LIDInfo {
	var <name, <bustype, <vendor, <product, <version;

	printOn { | stream |
		super.printOn(stream);
		stream << $( << name << ", ";
		[
			bustype,
			vendor,
			product,
			version
		].collect({ | x | "0x" ++ x.asHexString(4) }).printItemsOn(stream);
		stream.put($));
	}
}

LIDAbsInfo {
	var <value = 0, <min = 0, <max = 0, <fuzz = 0, <flat = 0;

	printOn { | stream |
		stream
		<< this.class.name << $(
		<< "value: " << value << ", "
		<< "min: " << min << ", "
		<< "max: " << max << ", "
		<< "fuzz: " << fuzz << ", "
		<< "flat: " << flat << $)
	}
}

LID {
	var dataPtr, <path, <info, <caps, <spec, <slots, <isGrabbed=false, <>action;
	classvar all, eventTypes, <>specs, <>deviceRoot = "/dev/input";
	
	*initClass {
		all = [];
		specs = IdentityDictionary.new;
		eventTypes = [
			// maps event type (index) to max code value		
			0x0001,		// EV_SYN
			0x01ff,		// EV_KEY
			0x000f,		// EV_REL
			0x003f,		// EV_ABS
			0x0007,		// EV_MSC

			nil, nil, nil, nil,
			nil, nil, nil, nil,
			nil, nil, nil, nil,

			0x000f,		// EV_LED
			0x0007,		// EV_SND

			nil,

			0x0001,		// EV_REP
			0x007f,		// EV_FF
			0x0000,		// EV_PWR
			0x0001,		// EV_FF_STATUS

			nil, nil, nil, nil,
			nil, nil, nil
		];
		UI.registerForShutdown({
			this.closeAll;
			this.prStopEventLoop;
		});
		this.prStartEventLoop;
	}
	*mouseDeviceSpec {
		^(
			// key
			b1: #[0x0001, 0x0110],	// left button
			b2: #[0x0001, 0x0111],	// middle button
			b3: #[0x0001, 0x0112],	// right button
			// rel
			x: #[0x0002, 0x0000],	// x axis
			y: #[0x0002, 0x0001],	// y axis
			s: #[0x0002, 0x0008]	// scroll wheel
		)
	}
	*all {
		^all.copy
	}
	*closeAll {
		all.copy.do({ | dev | dev.close });
	}
	*register { | name, spec |
		specs[name] = spec;
	}
	*new { | path |
		path = PathName(path);
		if (path.isRelativePath) {
			path = (deviceRoot ++ "/" ++ path.fullPath).standardizePath
		}{
			path = path.fullPath;
		};
		^all.detect({ | dev | dev.path == path }) ?? { super.new.prInit(path) }
	}
	isOpen {
		^dataPtr.notNil
	}
	close {
		if (this.isOpen) {
			this.prClose;
			all.remove(this);
		};
	}
	dumpCaps {
		caps.keys.do { | evtType |
			Post << "0x" << evtType.asHexString << ":\n";
			caps[evtType].do { | evtCode |
				Post << $\t << "0x" << evtCode.asHexString << "\n";
			}
		}
	}
	dumpEvents {
		action = { | evtType, evtCode, value |
			[evtType.asHexString, evtCode.asHexString, value].postln;
		}
	}
	slot { | evtType, evtCode |
		^slots.atFail(evtType, {
			Error("event type not supported").throw
		}).atFail(evtCode, {
			Error("event code not supported").throw
		})
	}
	at { | controlName |
		^this.slot(*spec.atFail(controlName, {
			Error("invalid control name").throw
		}))
	}
	getAbsInfo { | evtCode |
		^this.prGetAbsInfo(evtCode, LIDAbsInfo.new)
	}
	getKeyState { | evtCode |
		^this.prGetKeyState(evtCode)
	}
	getLEDState { | evtCode |
		^0
	}
	grab { | flag = true |
		// useful when using mouse or keyboard. be sure to have an
		// 'exit point', or your desktop will be rendered useless ...
		if (isGrabbed != flag) {
			this.prGrab(flag);
			isGrabbed = flag;
		};
	}
	ungrab {
		this.grab(false)
	}

	// PRIVATE
	*prStartEventLoop {
		_LID_Start
		^this.primitiveFailed
	}
	*prStopEventLoop {
		_LID_Stop
		^this.primitiveFailed
	}
	prInit { | argPath |
		this.prOpen(argPath);
		all = all.add(this);
		path = argPath;
		info = this.prGetInfo(LIDInfo.new);
		spec = specs.atFail(info.name, { IdentityDictionary.new });
		caps = IdentityDictionary.new;
		slots = IdentityDictionary.new;
		eventTypes.do { | evtTypeMax, evtType |
			if (evtType.notNil and: { this.prEventTypeSupported(evtType) }) {
				caps[evtType] = List.new;
				slots[evtType] = IdentityDictionary.new;
				for (0, evtTypeMax, { | evtCode |
					if (this.prEventCodeSupported(evtType, evtCode)) {
						caps[evtType].add(evtCode);
						slots[evtType][evtCode] = LIDSlot.new(
							this, evtType, evtCode
						);
					};
				});
				caps[evtType].sort;
			}
		};
	}
	prOpen { | path |
		_LID_Open
		^this.primitiveFailed
	}
	prClose {
		_LID_Close
		^this.primitiveFailed
	}
	prEventTypeSupported { | evtType |
		_LID_EventTypeSupported
		^this.primitiveFailed
	}
	prEventCodeSupported { | evtType, evtCode |
		_LID_EventCodeSupported
		^this.primitiveFailed
	}
	prGetInfo { | info |
		_LID_GetInfo
		^this.primitiveFailed
	}
	prGetKeyState { | evtCode |
		_LID_GetKeyState
		^this.primitiveFailed
	}
	prGetAbsInfo { | evtCode, absInfo |
		_LID_GetAbsInfo
		^this.primitiveFailed
	}
	prGrab { | flag |
		_LID_Grab
		^this.primitiveFailed
	}
	prHandleEvent { | evtType, evtCode, evtValue |
		// event callback
		if (action.notNil) {
			action.value(evtType, evtCode, evtValue);
		}{
			slots[evtType][evtCode].value_(evtValue);
		}
	}
}

LIDSlot {
	var <device, <type, <code, value=0, <spec, <>action;
	classvar slotTypeMap;

	*initClass {
		slotTypeMap = IdentityDictionary.new.addAll([
			0x0001 -> LIDKeySlot,
			0x0002 -> LIDRelSlot,
			0x0003 -> LIDAbsSlot
		]);
	}
	*new { | device, evtType, evtCode |
		^(slotTypeMap[evtType] ? this).newCopyArgs(device, evtType, evtCode).initSpec
	}
	initSpec {
		spec = ControlSpec(0, 1, \lin, 1, 0);
	}
	rawValue {
		^value
	}
	value {
		^spec.unmap(value)
	}
	value_ { | rawValue |
		value = rawValue;
		action.value(this);
	}
	next {
		^this.value
	}
}

LIDKeySlot : LIDSlot {
	initSpec {
		super.initSpec;
		value = device.getKeyState(code);
	}
}

LIDRelSlot : LIDSlot {
	initSpec {
	}
	value_ { | rawValue |
		value = value + rawValue;
		action.value(this);
	}
}

LIDAbsSlot : LIDSlot {
	initSpec {
		var info;
		info = device.getAbsInfo(code);
		spec = ControlSpec(info.min, info.max, \lin, 1);
		spec.default = spec.map(0.5).asInteger;
		value = info.value;
	}
}

// EOF