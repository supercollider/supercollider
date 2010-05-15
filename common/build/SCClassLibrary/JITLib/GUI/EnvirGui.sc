	// needs a defer for making new EZs after a clearField... 
	// get rid of this later if possible. 

EnvirGui : JITGui {

	var <valFields, <widgets, labelWidth;
	var <keysRotation = 0, <specs;
	var <protoBut, <parentBut, <knowBut, <docBut;
	var <replaceKeys;

	var <>useRanger = true;

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
	
	makeViews { |options| 
		var height = skin.buttonHeight;

		specs = ();  
		replaceKeys = ();
		prevState = ( overflow: 0, keysRotation: 0, editKeys: []); 
		
		labelWidth = zone.bounds.width * 0.17;
		
		this.makeOptionalViews(options);
				
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
	
	name_ { |name|
		if (hasWindow) { parent.name_(this.winName(name)) };
		if (nameView.notNil) { nameView.string_(name) };
	}

	makeOptionalViews { |options, height = 20| 
		var extraFuncs = (
			name: 	{ this.makeNameView(70, height) }, 
			CLR: 	{ this.makeClrBut(30, height) },
			doc: 	{ this.makeDocBut(30, height) },
			proto: 	{ this.makeProtoBut(40, height) },
			parent: 	{ this.makeParentBut(40, height) },
			know: 	{ this.makeKnowBut(70, height) }
		);
		options.do { |key| extraFuncs[key].value; };
	}

	makeNameView { |nameWid, height|
		nameView = StaticText(zone, Rect(0,0, nameWid, height))
			.font_(font).align_(0);
	}
	
	makeClrBut { |width, height|
		Button(zone, width@height).font_(font)
			.states_([[\CLR, skin.fontColor, Color.clear]])
			.action_({ arg btn, mod;
				if (mod.isAlt) { object.clear } {
					"Safety - use alt-click to clear object.".postln;
				}
			})
	}
	
	makeProtoBut { |width, height|
		protoBut = Button(zone, Rect(0,0, width, height))
			.font_(font)
			.resize_(3)
			.states_([
				["proto", skin.fontColor, skin.offColor]
			])
			.enabled_(false)
			.action_({ EnvirGui(object.proto) });
	} 
	
	makeParentBut { |width, height|

		parentBut = Button(zone, Rect(0,0, width, height))
			.font_(font)
			.resize_(3)
			.states_([
				["parent", skin.fontColor, skin.offColor]
			])
			.enabled_(false)
			.action_({ EnvirGui(object.parent) });
	} 
	
	makeKnowBut { |width, height|

		knowBut = Button(zone, Rect(0,0, width, height))
			.font_(font).resize_(3)
			.states_([
				["know: false", skin.fontColor, skin.offColor],
				["know: true", skin.fontColor, skin.onColor]
			])
			.enabled_(false)
			.action_({ |but| object.know = (but.value > 0) });
	}
	
	makeDocBut { |width, height|
		docBut = Button(zone, width@height).font_(font)
			.states_([[\doc, skin.fontColor, Color.clear]])
			.enabled_(false)
			.action_({ |but, mod| 
				if (object.notNil) { object.asCompileString.newTextWindow };
			})
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
		
		this.updateButtons;

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
	
	updateButtons { 
		var flag = object.notNil; 
		if (protoBut.notNil) { protoBut.enabled_(flag and: { object.proto.notNil }) };
		if (parentBut.notNil) { parentBut.enabled_(flag and: { object.parent.notNil }) };
		if (knowBut.notNil) { knowBut.enabled_(flag).value_((flag and: { object.know }).binaryValue) };
		if (docBut.notNil) { docBut.enabled_(flag) };
	}
	
	clearFields { |from = 0| (numItems - 1 .. from).do(this.clearField(_)) }

	setByKeys { |newKeys|
		var prevEnvir = prevState[\object] ?? {()};
		var newVal, oldVal, oldKey;

		newKeys.do { |newKey, i| 
			var isSameKey; 
			
			oldKey = prevState[\editKeys][i]; 
			isSameKey = oldKey == newKey;
			newVal = object[newKey];
			oldVal = prevEnvir[newKey];
			if (isSameKey.not or: { oldVal != newVal }) {
			//	"val for % has changed: %\n".postf(key, newval);
				this.setField(i, newKey, newVal, isSameKey);
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
		var area = valFields[index];
		var keyToShow = this.showKeyFor(key);
		
	//	"setToSlider...".postln; 
		 
		if (widget.isKindOf(EZSlider)) {
		//	"was slider already".postln; 
			if (sameKey.not) { 
		//		"new key - reset widget ...".postln; 
				widget.set(
					keyToShow, 
					this.getSpec(key, value), 
					this.setFunc(key),
					value
				);
				this.colorizeArea(area, keyToShow != key);			} {
		//		"old key, just set ...".postln; 
				widget.value = value;
			};

			^this
		} {
		//	"make new slider!".postln; 
			this.clearField(index); 
				// don't know why, but defer seems needed:
			{ 
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
				
				this.colorizeArea(area, keyToShow != key);

			}.defer(0.03); 
			
			^this
		};
	}
	
	setToRanger { |index, key, value, sameKey| 
		var widget = widgets[index];
		var area = valFields[index];
		var keyToShow = this.showKeyFor(key);
		
		if (widget.isKindOf(EZRanger)) {
			if (sameKey.not) { 
				widget.set(
					keyToShow, 
					this.getSpec(key), 
					this.setFunc(key),
					value
				);
				this.colorizeArea(area, keyToShow != key);
			} {
				widget.value = value;
			};
		} {
			this.clearField(index);
			{
				widget = EZRanger(valFields[index], valFields[index].bounds.extent, 
					this.showKeyFor(key), 
					this.getSpec(key, value.maxItem),
					this.setFunc(key),
					value,
					labelWidth: labelWidth, 
					numberWidth: labelWidth
				).font_(font);
				
				widget.view.resize_(2);
				widgets[index] = widget;
				this.colorizeArea(area, keyToShow != key);
			}.defer(0.03); 
			^this
		};
	}
	
	showKeyFor { |key| ^(replaceKeys[key] ? key) }
	
	colorizeArea { |area, hilite = true| 
		if (hilite) { 
			area.background_(skin.hiliteColor);
		} { 
			area.background_(skin.background);
		};
	}

	setToText { |index, key, value, sameKey = false|
		var widget = widgets[index];
		var area = valFields[index];
		var keyToShow = this.showKeyFor(key);

			// default: EZText
		if (widget.isKindOf(EZText)) { 
			if (sameKey.not) { widget.labelView.string = keyToShow.asString };
			widget.action = this.setFunc(key);
			widget.value = value; 
			this.colorizeArea(area, keyToShow != key);
			
			^this
		} { 
			this.clearField(index);
			{
				widget = EZText(area, 
					area.bounds.extent, 
					this.showKeyFor(key),
					this.setFunc(key),
					value, false, 
					labelWidth, labelHeight: 18);
				widget.font_(font);
				widget.view.resize_(2);
				widgets[index] = widget;
				
				widget.value_(value);
				this.colorizeArea(area, keyToShow != key);
				
			}.defer(0.03); 
		};
	}

	setField { |index, key, value, sameKey = false|
		var widget = widgets[index];
		var area = valFields[index];
	
		if (value.isKindOf(SimpleNumber) ) {
			this.setToSlider(index, key, value, sameKey);
			^this
		};
			// Ranger - only if spec exists and value is 2 numbers
		if (useRanger 
			and: { value.size == 2 
			and: { key.asSpec.notNil 
			and: { value.every(_.isKindOf(SimpleNumber)) }
		} }) {
			this.setToRanger(index, key, value, sameKey);
			^this
		};
			// default
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
