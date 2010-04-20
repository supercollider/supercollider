	// needs a defer for making new EZs after a clearField... 
	// get rid of this later if possible. 

EnvirGui : JITGui {

	var <valFields, <widgets, labelWidth;
	var <keysRotation = 0, <specs;
	
	var <replaceKeys;

	editKeys {
		^prevState[\editKeys]
	}

	addReplaceKey { |replaced, replacer, spec| 
		replaceKeys.put(replaced, replacer);
		if (spec.notNil) { specs.put(replaced, spec) };
	}

	removeReplaceKey { |replaced| 
		replaceKeys.removeAt(replaced)
	}

	*new { |object, numItems = 8, parent, bounds, makeSkip = true, options = #[]| 
		^super.new(object, numItems, parent, bounds, makeSkip = true, options);
	}
	
	setDefaults { 
		if (parent.notNil) { skin = skin.copy.put(\margin, 0@0) };

		defPos = 530@660;
		minSize = 250 @ (numItems * skin.buttonHeight + (skin.margin.y * 2));
	}
	
	makeViews {
		var height = skin.buttonHeight;

		specs = ();  
		replaceKeys = ();	
		prevState = ( overflow: 0, keysRotation: 0, editKeys: []);
		
		labelWidth = zone.bounds.width * 0.17;
				 
		valFields = {
			CompositeView(zone, Rect(0, 0, bounds.width - 20, height))
			.resize_(2)
			.background_(skin.background);
		}.dup(numItems);
		
		widgets = nil.dup(numItems); // keep EZGui types here

		zone.decorator.reset.shift(zone.bounds.width - 16, 0);

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
		
		if (object.isNil) { ^(editKeys: [], overflow: 0, keysRotation: 0) }; 
		
		newKeys = object.keys.asArray.sort;
		overflow = (newKeys.size - numItems).max(0);
		keysRotation = keysRotation.clip(0, overflow);
		newKeys = newKeys.drop(keysRotation).keep(numItems); 
				
		^(object: object, editKeys: newKeys, overflow: overflow, keysRotation: keysRotation)
	}
	
	checkUpdate { |doFull = false| 
		var newState = this.getState;
		var newKeys = newState[\editKeys]; 
		
		if (doFull.not and: { newState == prevState }) { ^this };
		
		if (object.isNil) { 
			prevState = newState;
			^this.clearFields(0);
		};

		if (newState[\overflow] > 0) {
			scroller.visible_(true);
			scroller.numItems_(object.size);
			scroller.value_(newState[\keysRotation]);
			
		} {
			scroller.visible_(false);
		};

		if (newKeys == prevState[\editKeys]) { 
			this.setByKeys(newKeys);
		} {
			this.setByKeys(newKeys);
			if (newState[\overflow] == 0) { this.clearFields(newKeys.size) };
		};
		
//		"newState: %\n".postf(newState);
//		"prevState: %\n".postf(prevState);
		prevState = newState.put(\object, object.copy);
	}

	clearFields { |from = 0| (numItems - 1 .. from).do(this.clearField(_)) }

	setByKeys { |newKeys|
		var prevEnvir = prevState[\object] ?? {()};
		var newVal, oldVal, oldKey;

		newKeys.do { |newKey, i|
			oldKey = prevState[\editKeys][i];
			newVal = object[newKey];
			oldVal = prevEnvir[newKey];
			if (oldKey != newKey or: { oldVal != newVal }) {
			//	"val for % has changed: %\n".postf(key, newval);
				this.setField(i, newKey, newVal, newKey == oldKey);
			};
		};
	}

	clearField { |index|
		var area = valFields[index];
		try {
			area.children.copy.do { |view| view.remove };
			area.background_(skin.background);
			area.refresh;
			widgets[index] = nil;
		};
	}
	
	setFunc { |key|
		^{ |elem| object.put(key, elem.value) }
	}
	
	setToSlider { |index, key, value, sameKey| 
		var widget = widgets[index];
		var area;
		
	//	"setToSlider...".postln; 
		 
		if (widget.isKindOf(EZSlider)) {
		//	"was slider already".postln; 
			if (sameKey.not) { 
		//		"new key - reset widget ...".postln; 
				widget.set(
					this.showKeyFor(key), 
					this.getSpec(key, value), 
					this.setFunc(key),
					value);
			} {
		//		"old key, just set ...".postln; 
				widget.value = value;
			};
			^this
		} {
		//	"make new slider!".postln; 
			this.clearField(index); 
				// don't know why, but defer seems needed:
			{ 
				area = valFields[index];
				widget = EZSlider(area, area.bounds.extent,
					this.showKeyFor(key),
					this.getSpec(key, value),
					this.setFunc(key),
					value,
					labelWidth: labelWidth, 
					numberWidth: labelWidth
				)
					.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;
			}.defer(0.03); 
			
			^this
		};
	}
	
	setToRanger { |index, key, value, sameKey| 
		var widget = widgets[index];
		var area;
		
		if (widget.isKindOf(EZRanger)) {
			if (sameKey.not) { 
				widget.set(
					this.showKeyFor(key), 
					this.getSpec(key), 
					this.setFunc(key),
					value
				);
			} {
				widget.value = value;
			};
		} {
			this.clearField(index);
			{ 
				area = valFields[index];
				widget = EZRanger(valFields[index], valFields[index].bounds.extent, 
					this.showKeyFor(key), 
					this.getSpec(key, value.maxItem),
					this.setFunc(key),
					value,
					labelWidth: labelWidth, 
					numberWidth: labelWidth
				)
					.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;
			}.defer(0.03); 
			^this
		};
	}
	
	showKeyFor { |key| ^(replaceKeys[key] ? key).asString }

	setToText { |index, key, value, sameKey = false|
		var widget = widgets[index];
		var area; 

			// default: EZText
		if (widget.isKindOf(EZText)) { 
			if (sameKey.not) { 
				
				widget.labelView.string = this.showKeyFor(key);
			};
			widget.action = this.setFunc(key);
			widget.value = value;
			^this
		} { 
			this.clearField(index);
			{
				area = valFields[index];
	
				widget = EZText(area, 
					area.bounds.extent, 
					this.showKeyFor(key),
					this.setFunc(key),
					value, false, labelWidth, labelHeight: 18);
					widget.font_(font);
					widget.view.resize_(2);
					widgets[index] = widget;
				
				widget.value_(value);
			}.defer(0.03); 
		};
	}

	setField { |index, key, value, sameKey = false|
		var area = valFields[index];
		var widget = widgets[index];

	//	[\replaceKeys, replaceKeys, key].postcs;
	
		if (replaceKeys[key].notNil) { 
			area.background_(skin.hiliteColor);
		} { 
			area.background_(skin.background);
		};

		if (value.isKindOf(SimpleNumber) ) {
			this.setToSlider(index, key, value, sameKey);
			^this
		};
			// Ranger - only if spec exists and value is 2 numbers
		if (value.size == 2
			and: { key.asSpec.notNil 
			and: { value.every(_.isKindOf(SimpleNumber)) }
		}) {
			this.setToRanger(index, key, value, sameKey);
			^this
		};
		
		this.setToText(index, key, value, sameKey);
		
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
		spec = spec ?? { Spec.guess(key, value) };
		specs.put(key, spec);
		^spec
	}
}
