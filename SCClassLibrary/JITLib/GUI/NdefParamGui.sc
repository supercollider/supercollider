// list of named params - if single number, show EZSlider,
// else show EZText

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

		// could be smarter:
		// accepts strings, numbers, ... what else?
	dragAction { |i|
		^{ 	arg drag;
			var key = prevState[\editKeys][i];
			var dragged = drag.object;
			if (dragged.isKindOf(String)) { dragged = dragged.interpret };
			if (dragged.notNil) {
				if(dragged.isKindOf(NodeProxy)) {
					drag.string = "->" + dragged.key;
					object.map(key, dragged);
					this.checkUpdate;
				} {
					if (dragged.isKindOf(SimpleNumber)) {
						object.set(key, dragged);
					};
				}
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

		#drags, valFields = { |i|
			var drag, field;

			try { // QT temp fix.
				drag = DragBoth(zone, Rect(0, 0, sinkWidth, skin.buttonHeight))
				.string_("-").align_(\center)
				.visible_(false)
				.font_(font);
			};
			drag.action_(this.dragAction(i));

			field = CompositeView(zone, Rect(0, 0, bounds.width - sinkWidth - 20, height))
			.resize_(2)
			.background_(skin.background);

			[drag, field];

		}.dup(numItems).flop;

		widgets = nil.dup(numItems); // keep EZGui types here

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
		var settings, newKeys, overflow;

		if (object.isNil) {
			^(name: 'anon', settings: [], editKeys: [], overflow: 0, keysRotation: 0)
		};

		settings = object.getKeysValues;
		newKeys = settings.collect(_[0]);

		overflow = (newKeys.size - numItems).max(0);
		keysRotation = keysRotation.clip(0, overflow);
		newKeys = newKeys.drop(keysRotation).keep(numItems);

		^(object: object, editKeys: newKeys, settings: settings,
			overflow: overflow, keysRotation: keysRotation)
	}

	checkUpdate {
		var newState = this.getState;

		if (newState == prevState) {
			^this
		};

		if (object.isNil) {
			prevState = newState;
			^this.clearFields(0);
		};

		if (newState[\overflow] > 0) {
			scroller.visible_(true);
			scroller.numItems_(newState[\settings].size);
			scroller.value_(newState[\keysRotation]);

		} {
			scroller.visible_(false);
		};

		if (newState[\editKeys] == prevState[\editKeys]) {
			this.setByKeys(newState[\editKeys], newState[\settings]);
		} {
			this.setByKeys(newState[\editKeys], newState[\settings]);
			if (newState[\overflow] == 0) { this.clearFields(newState[\editKeys].size) };
		};

		prevState = newState;
	}

//	clearFields { (object.size .. valFields.size).do(this.clearField(_)) }


	setByKeys { |newKeys, newSettings|

		var prevSettings = prevState[\settings] ? [];
		var newVal, oldVal, oldKey;

		newKeys.do { |newKey, i|

			newVal = newSettings.detect { |pair| pair[0] == newKey };
			if (newVal.notNil) { newVal = newVal[1] };

			oldKey = prevState[\editKeys][i];
			if (oldKey.notNil) {
				oldVal = prevSettings.detect(_[0] == oldKey);
				if (oldVal.notNil) { oldVal = oldVal[1] };
			};
			if (oldKey != newKey or: { oldVal != newVal }) {
			//	"val for % has changed: %\n".postf(key, newval);
				this.setField(i, newKey, newVal, newKey == oldKey);
			};
		};
	}

	setFunc { |key|
		^{ |sl| object.set(key, sl.value) }
	}

	clearField { |index|
		var area = valFields[index];
		try {
			drags[index].visible_(false);
			area.children.copy.do { |view| view.remove };
			area.background_(skin.background);
			area.refresh;
			widgets[index] = nil;
		};
	}

	setField { |index, key, value, sameKey = false|
		var area = valFields[index];
		var widget = widgets[index];

	//	[replaceKeys, key].postcs;
		if (replaceKeys[key].notNil) {
			area.background_(skin.hiliteColor);
		} {
			area.background_(skin.background);
		};
		try { // QT temp fix.
			if (value.isKindOf(NodeProxy)) {
				drags[index].object_(value).string_("->" ++ value.key);
			} {
				drags[index].object_(nil).string_("-");
			};
					// dodgy - defer should go away eventually.
					// needed for defer in setToSlider...
			{ drags[index].visible_(true) }.defer(0.05);
		};
		if (value.isKindOf(SimpleNumber) ) {
			this.setToSlider(index, key, value, sameKey);
			^this
		};

		this.setToText(index, key, value, sameKey);
	}
}
