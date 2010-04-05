EnvirGui : JITGui {

	var <name, <valFields, <widgets, labelWidth;
	var <keysRotation = 0, <specs;

	name_ { } 
	
	setDefaults { 
		defPos = 530@660;
		minSize = 250 @ (numItems * skin.buttonHeight);
	}
	
	makeViews {
		var height = skin.buttonHeight;

		specs = ();  
		prevState = [nil, [], 0, ()];
		
		labelWidth = zone.bounds.width * 0.15;
		valFields = {
			CompositeView(zone, Rect(0, 0, bounds.width - 24, height))
			.resize_(2)
			.background_(skin.background);
		}.dup(numItems);
		
		widgets = nil.dup(numItems); // keep EZGui types here

		zone.decorator.reset.shift(zone.bounds.width - 24, 0);

		scroller = EZScroller(zone,
			Rect(0, 0, 12, numItems * height),
			numItems, numItems,
			{ |sc| keysRotation = sc.value.asInteger.max(0); }
		).visible_(false);
		scroller.slider.resize_(3);
	}

	accepts { |obj|
		^(obj.isNil or: { obj.isKindOf(Dictionary) })
	}
	
		// backwards compatibility
	envir_ { |envir| this.object_(envir) }
	
	envir { ^object }
	
	getState { 
		var newKeys, overflow;
		
		if (object.isNil) { ^[nil, [], 0, ()] }; 
		
		newKeys = object.keys.asArray.sort;
		overflow = (newKeys.size - numItems).max(0);
		keysRotation = keysRotation.clip(0, overflow);
		newKeys = newKeys.drop(keysRotation).keep(numItems);
		
		^[object, newKeys, overflow, keysRotation]
	}
	
	checkUpdate {
		var newState = this.getState;
		var newKeys = newState[1];
		if (newState == prevState) { ^this };

		if (object.isNil) { 
			prevState = newState.put(0, nil);
			^this.clearFields;
		};

		if (newState[2] > 0) {					// overflow
			scroller.visible_(true);
			scroller.numItems_(object.size);
			scroller.value_(newState[3]);		// keysRotation
			
		} {
			scroller.visible_(false);
		};

		if (newKeys == prevState[1]) { 
			this.setByKeys(newKeys);
		} {
			this.setByKeys(newKeys);
			this.clearFields;
		};
		
//		"newState: %\n".postf(newState);
//		"prevState: %\n".postf(prevState);
		prevState = newState.put(0, object.copy);
	}

	clearFields { (object.size .. valFields.size).do(this.clearField(_)) }

	setByKeys { |newKeys|
		var prevEnvir = prevState[0] ?? {()};
		var newVal, oldVal, oldKey;

		newKeys.do { |newKey, i|
			oldKey = prevState[1][i];
			newVal = object[newKey];
			oldVal = prevEnvir[newKey];
			if (oldKey != newKey or: { oldVal != newVal }) {
			//	"val for % has changed: %\n".postf(key, newval);
				this.setField(i, newKey, newVal, newKey == oldKey);
			};
		};
	}

	clearField { |index|
		try {
			valFields[index].children.copy.do { |view| view.remove };
			valFields[index].refresh;
			widgets[index] = nil;
		};
	}

	setField { |index, key, value, sameKey = false|
		var area = valFields[index];
		var widget = widgets[index];

		if (value.isKindOf(SimpleNumber) ) {
			if (widget.isKindOf(EZSlider) and: sameKey) {
				widget.value = value;
				^this
			} {
				this.clearField(index);
				widget = EZSlider(area, area.bounds.extent,
					key,
					this.getSpec(key, value),
					{ |sl| object.put(key, sl.value) },
					value,
					labelWidth: labelWidth, 
					numberWidth: labelWidth
				)
					.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;
				^this
			};
		};

		if (value.isKindOf(Array) and: (value.size == 2) ) {
			if (widget.isKindOf(EZRanger) and: sameKey) {
				widget.value = value;
				^this
			} {
				this.clearField(index);
				widget = EZRanger(area, area.bounds.extent, key, 
					this.getSpec(key, value.maxItem),
					{ |sl| object.put(key, sl.value) },
					labelWidth: labelWidth, 
					numberWidth: labelWidth
				)
					.value_(value)
					.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;

				^this
			};
		};

			// default: EZText
		if (widget.isKindOf(EZText) and: sameKey) {
			widget.value = value;
			^this
		} {
			this.clearField(index);

			widget = EZText(area, area.bounds.extent, key,
				{ arg ez; object[key] = ez.textField.value.interpret; },
				value, false, labelWidth, labelHeight: 18);
				widget.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;
		};
		^this
	}

	findWidget { |key|
		^widgets.reject(_.isNil).detect { |ez| ez.labelView.string.asSymbol == key };
	}
	
	putSpec { |key, obj|
		var widge, spec;
		spec = obj.asSpec;
		specs.put(key, spec);
		// could check all widgets and update specs if same name ...
		widge = this.findWidget(key);
		if (widge.notNil) { widge.controlSpec_(spec).value_(widge.value) }
	}

	getSpec { |key, value|
		var spec = specs[key] ? Spec.specs[key];
		if (spec.notNil) { ^spec };
		
			// guess specs if not given.
		spec = if (value.abs > 0) {
			[value/20, value*20, \exp].asSpec
		} {
			[-2, 2, \lin].asSpec
		};
		specs.put(key, spec);
		^spec
	}
}
