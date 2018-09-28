
NdefParamGui : EnvirGui {
	var <drags;

	setDefaults {
		defPos = 530@660;
		minSize = 320 @ (numItems * skin.buttonHeight);

		if (parent.notNil) { skin = skin.copy.margin = 0@0; };
	}

	accepts { |obj|
		^obj.isNil or: { obj.isKindOf(NodeProxy) };
	}

	dragAction { |i|
		^{ |drag|
			var key = prevState[\editKeys][i];
			var dragged = drag.object;
			if (dragged.isKindOf(String)) { dragged = dragged.interpret };
			if (dragged.notNil) {
				if(dragged.nodeMapMapsToControl) { drag.string = "->" + dragged.key };
				object.xset(key, dragged);
				this.checkUpdate;
			}
		}
	}

	makeViews {
		var sinkWidth = 40;
		var height = skin.buttonHeight;

		specs = ();
		replaceKeys = ();
		prevState = ( settings: [], overflow: 0, keysRotation: 0, editKeys: []);

		labelWidth = zone.bounds.width * 0.15;

		#drags, paramViews = { |i|
			var drag = DragBoth(zone, Rect(0, 0, sinkWidth, skin.buttonHeight))
				.string_("-").align_(\center)
				.visible_(false)
				.font_(font)
				.action_(this.dragAction(i));

			var parView = ParamView(zone, Rect(0, 0, bounds.width - sinkWidth - 20, height))
			// .resize_(2)
			.background_(skin.background)
			.useRanger_({ this.useRanger });

			parView.zone.visible_(false);

			[drag, parView];

		}.dup(numItems).flop;

		zone.decorator.reset.shift(zone.bounds.width - 16, 0);

		scroller = EZScroller(zone,
			Rect(0, 0, 12, numItems * height),
			numItems, numItems,
			{ |sc|
				keysRotation = sc.value.asInteger.max(0);
				prevState.put(\dummy, \dummy);
			}
		).visible_(false);
		scroller.slider.resize_(3);
	}

	getState {
		var settings, newKeys, overflow, currSpecs;

		if (object.isNil) {
			^(name: 'anon', settings: [], editKeys: [], overflow: 0, keysRotation: 0)
		};

		settings = object.getKeysValues;
		newKeys = settings.collect(_[0]);

		overflow = (newKeys.size - numItems).max(0);
		keysRotation = keysRotation.clip(0, overflow);
		newKeys = newKeys.drop(keysRotation).keep(numItems);
		currSpecs = newKeys.collect { |key|
			var pair = settings.detect { |pair| pair[0] == key };
			[key, this.getSpec(key, pair[1])];
		};

		^(object: object, editKeys: newKeys, settings: settings,
			overflow: overflow, keysRotation: keysRotation,
			specs: currSpecs
		)
	}

	checkUpdate {
		var newState = this.getState;
		var newKeys = newState[\editKeys];
		var newSpecs = newState[\specs];

		if (newState == prevState) {
			^this
		};

		if (object.isNil) {
			prevState = newState;
			^this.showFields(0);
		};

		if (newState[\overflow] > 0) {
			scroller.visible_(true);
			scroller.numItems_(newState[\settings].size);
			scroller.value_(newState[\keysRotation]);

		} {
			scroller.visible_(false);
		};

		if (newKeys == prevState[\editKeys]) {
			this.setByKeys(newKeys, newState[\settings]);
		} {
			this.setByKeys(newKeys, newState[\settings]);
			this.showFields(newKeys.size);
		};

		if (newSpecs != prevState[\specs]) {
			this.updateViewSpecs(newSpecs);
		};
		prevState = newState;
	}

	setByKeys { |newKeys, newSettings|

		var prevSettings = prevState[\settings] ? [];
		var newVal, oldVal, oldKey;

		newKeys.do { |newKey, i|
			var paramView = paramViews[i];

			newVal = newSettings.detect { |pair| pair[0] == newKey };
			if (newVal.notNil) { newVal = newVal[1] };

			oldKey = prevState[\editKeys][i];
			if (oldKey.notNil) {
				oldVal = prevSettings.detect { |pair| pair[0] == oldKey };
				if (oldVal.notNil) { oldVal = oldVal[1] };
			};

			if (oldKey != newKey) {
				paramView.label_(replaceKeys[newKey] ? newKey);
				paramView.spec_(this.getSpec(newKey, newVal));
				paramView.value_(newVal);
				paramView.action = this.setFunc(newKey);
			} {
				if (oldVal != newVal) { paramView.value_(newVal) };
			};
		};
	}

	showFields { |num = 0|
		paramViews.do { |pv, i|
			var isInUse = i < num;
			pv.visible_(isInUse);
			drags[i].visible_(isInUse);
		}
	}

	setFunc { |key|
		^{ |sl|
			// this special key (#) allows to set the source from a slider
			if(key == '#') {
				object.source = sl.value
			} {
				object.set(key, sl.value)
			}
		}
	}
}
