
EnvirGui : JITGui {

	var <paramViews, labelWidth;
	var <keysRotation = 0, <specs;
	var <protoBut, <parentBut, <knowBut, <docBut;
	var <replaceKeys;

	var <>useRanger = true;

	editKeys {
		^prevState[\editKeys]
	}

	addReplaceKey { |replaced, replacer, spec|
		var pv = this.viewForParam(replaced);
		replaceKeys.put(replaced, replacer);
		if (pv.notNil) { pv.label_(replacer) };
	}

	removeReplaceKey { |replacer|
		var pv = this.viewForParam(replacer);
		var replaced = replaceKeys.removeAt(replaced);
		if (pv.notNil) { pv.label_(replacer) };
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

		specs = ().parent_(Spec.specs);
		replaceKeys = ();
		prevState = ( overflow: 0, keysRotation: 0, editKeys: []);

		labelWidth = zone.bounds.width * 0.17;

		this.makeOptionalViews(options);

		paramViews = {
			ParamView(zone, Rect(0, 0, bounds.width - 20, height))
			.background_(skin.background)
			.useRanger_({ this.useRanger });
		}.dup(numItems);
		paramViews.do { |pv| pv.zone.visible_(false); };

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
			.states_([[\CLR, skin.fontColor, skin.background]])
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
			.states_([[\doc, skin.fontColor, skin.background]])
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

	highlight { |index, prefix, color|
		var parView = paramViews[index];
		var parName = this.editKeys[index];

		parView.background_(color ? skin.onColor2);

		if (prefix.notNil and: parName.notNil) {
			parName = prefix ++ parName;
		};
		parView.label_(parName);
	}

	unhighlight { |index|
		var parView = paramViews[index];
		var parName = this.editKeys[index] ? "";

		parView.background_(skin.offColor);
		parView.label_(parName);
	}

	// also get specs as state that may have changed
	getState {
		var newKeys, overflow, currSpecs;

		if (object.isNil) { ^(editKeys: [], overflow: 0, keysRotation: 0) };

		newKeys = object.keys.asArray.sort;
		overflow = (newKeys.size - numItems).max(0);
		keysRotation = keysRotation.clip(0, overflow);
		newKeys = newKeys.drop(keysRotation).keep(numItems);
		currSpecs = newKeys.collect{ |key|
			[key, this.getSpec(key, object[key])] };

		^(  object: object,
			editKeys: newKeys,
			overflow: overflow,
			keysRotation: keysRotation,
			specs: currSpecs
		)
	}

	checkUpdate {
		var newState = this.getState;
		var newKeys = newState[\editKeys];
		var newSpecs = newState[\specs];

		this.updateButtons;

		if (object.isNil) {
			prevState = newState;
			^this.showFields(0);
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
			this.showFields(newKeys.size);
		};

		if (newSpecs != prevState[\specs]) {
			this.updateViewSpecs(newSpecs);
		};

		prevState = newState.put(\object, object.copy);
	}

	updateButtons {
		var flag = object.notNil;
		if (protoBut.notNil) {
			protoBut.enabled_(flag and: { object.proto.notNil })
		};
		if (parentBut.notNil) {
			parentBut.enabled_(flag and: { object.parent.notNil })
		};
		if (knowBut.notNil) {
			knowBut.enabled_(flag).value_((flag and: { object.know }).binaryValue)
		};
		if (docBut.notNil) { docBut.enabled_(flag) };
	}

	// backwards compat
	clearFields { |num| ^this.showFields(num) }

	showFields { |num = 0|
		paramViews.do { |pv, i|
			var isInUse = i < num;
			pv.visible_(isInUse);
		}
	}

	setByKeys { |newKeys|
		var prevEnvir = prevState[\object] ?? {()};
		var newVal, oldVal, oldKey;


		newKeys.do { |newKey, i|
			var isSameKey;
			var paramView = paramViews[i];

			oldKey = prevState[\editKeys][i];
			isSameKey = oldKey == newKey;
			newVal = object[newKey];
			oldVal = prevEnvir[newKey];

			if (isSameKey.not) {
				paramView.label_(replaceKeys[newKey] ? newKey);
				paramView.spec_(this.getSpec(newKey, newVal));
				paramView.value_(newVal);
				paramView.action = this.setFunc(newKey);
			} {
				if (oldVal != newVal) { paramView.value_(newVal) };
			};
		};
	}

	setFunc { |key|
		^{ |elem| object.put(key, elem.value) }
	}

	showKeyFor { |key| ^(replaceKeys[key] ? key) }

	colorizeArea { |area, hilite = true|
		if (hilite) {
			area.background_(skin.hiliteColor);
		} {
			area.background_(skin.background);
		};
	}

	updateViewSpecs { |newSpecs|
		newSpecs.do { |pair|
			var name, spec, pv;
			#name, spec = pair;
			if (spec.notNil) {
				pv = this.viewForParam(name);
				if (pv.notNil) { pv.spec_(spec) };
			};
		};
	}

	viewForParam { |name|
		name = replaceKeys[name] ? name;
		^paramViews.detect { |pv| pv.label == name };
	}

	putSpec { |key, obj|
		var pview, spec, value;

		// clear local spec only
		if (obj.isNil) {
			specs.put(key, nil);
			^this
		};
		spec = obj.asSpec;
		specs.put(key, spec);
		pview = this.viewForParam(key);
		if (pview.notNil) {
			value = pview.value;
			pview.spec_(spec);
			pview.value_(value)
		}
	}

	// this finds global specs and local specs in the gui only
	// - use JITLibExtensions for specs attached to the JITGui objects.
	// precedence: global specs first, then cached local,
	// else guess an initial spec and remember it.
	getSpec { |key, value|
		var spec = specs[key] ?? { key.asSpec };
		if (spec.isNil) {
			spec = Spec.guess(key, value);
			specs.put(key, spec);
		};
		^spec
	}
}
