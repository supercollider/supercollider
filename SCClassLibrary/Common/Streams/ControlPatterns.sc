//human device interface pattern. pulls values from devices like gamepads etc.

Phid : Pattern {
	var <>element, <>locID, <>repeats;

	//element can be key or index
	*new { arg element, locID=0, repeats=inf;
		^super.newCopyArgs(element, locID, repeats)
	}
	storeArgs { ^[element,locID,repeats] }
	embedInStream { arg event;
		var all, device, spec, elements, deviceName, min, max;

		all = HIDDeviceService.devices;
		if(all.isNil, {
			HIDDeviceService.buildDeviceList;
			all = HIDDeviceService.devices;
		});
		device = all.at(locID);
		if(device.isNil, { "device not found".error; ^nil });
		if(element.isNumber.not, {
			element = HIDDeviceService.keyToIndex(element, locID);
			if(element.isNil, { "key not found".error; ^nil });
		});
		elements = device.elements;
		min = elements.at(element).min;
		max = elements.at(element).max;
		spec = ControlSpec.new(min, max, 'lin', 1);
		if((min === 0) and: {max === 1}, {
				repeats.value(event).do({
					event = device.value(element).yield
				});
				^event
		}, {
				repeats.value(event).do({
					event = spec.unmap(device.value(element)).yield
				});
				^event;
		})
	}
}
