EnvirGui {
	classvar <labelWidth = 60, <valWidth = 200;

	var <parent, <bounds, <name, <num, <zone;
	var <valFields, <widgets, <skin, <font;
	var <envir, <specs;
	var <oldEnvir, <oldKeys, <newKeys;
	var <skip, <scrolly, <keysRotation=0, <oldRotation=0;

	*new { arg envir, parent, bounds, name = \envir, num, makeSkip = true;

		if (envir.notNil and: num.isNil) { num = envir.size };
		num = num ? 10;

		^super.newCopyArgs(parent, bounds, name, num )
			.init(makeSkip)
			.envir_(envir);
	}

	envir_ { |invir|
		if (invir.isKindOf(Dictionary) or: invir.isNil) {
			envir = invir
		} {
			warn("EnvirGui: envir not a dictionary! \n%\n".format(invir));
			^this
		};
		this.checkUpdate;
	}

	checkUpdate {
		var overflow;
		if (envir == oldEnvir and: { keysRotation == oldRotation }) { ^this };
		if (envir.isNil) {
			oldKeys = [];
			oldRotation = 0;
			oldEnvir = ();
			^this.clearFields
		};

		newKeys = envir.keys.asArray.sort;

		overflow = (newKeys.size - num).max(0);
		keysRotation = keysRotation.clip(0, overflow);

		if (overflow > 0) {
			scrolly.visible_(true);
			scrolly.numItems_(newKeys.size);
			scrolly.value_(keysRotation);
			newKeys = newKeys.drop(keysRotation).keep(num);
		} {
			scrolly.visible_(false);
		};

		if (newKeys == oldKeys) {
			this.setByKeys;
		} {
			this.setByKeys;
			this.clearFields;
		};
		oldKeys = newKeys;
		oldEnvir = envir.copy;
		oldRotation = keysRotation;
	}

	clearFields { (envir.size .. valFields.size).do(this.clearField(_)) }

	setByKeys { |keys|
		var newval, oldval, oldkey;

		newKeys.do { |key, i|
			oldkey = oldKeys[i];
			newval = envir[key];
			oldval = oldEnvir[key];
			if (oldkey != key or: { oldval != newval }) {
			//	"val for % has changed: %\n".postf(key, newval);
				this.setField(i, key, newval, key == oldkey);
				oldEnvir[key] = newval;
			};
		};
	}

	init { |makeSkip|
		var height;
		skin = GUI.skins.jit;
		font = Font.new(*skin.fontSpecs);

		height = 	skin.buttonHeight;

		oldEnvir = (); oldKeys = []; specs = ();

		bounds = bounds ?? { Rect( 0, 0, labelWidth + valWidth + 24, num + 1 * height) };
		parent = parent ?? { Window(name.asString, bounds).front; };

		zone = CompositeView(parent.asView, bounds.resizeBy(2, 18))
			.resize_(2);
		zone.addFlowLayout(skin.margin, skin.gap);

		valFields = {
			CompositeView(zone, Rect(0, 0, bounds.width - 24, height))
			.resize_(2)
			.background_(skin.background);
		}.dup(num);
		widgets = nil.dup(num); // keep EZGui types here

		zone.decorator.reset.shift(zone.bounds.width - 24, 0);
		scrolly = EZScroller(zone,
			Rect(0, 0, 12, num * height),
			num, num,
			{ |sc| keysRotation = sc.value.asInteger.max(0); }
		).visible_(false);
		scrolly.slider.resize_(3);

		if (makeSkip) {
			skip = SkipJack({ this.checkUpdate }, 0.5, { parent.isClosed });
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
					{ |sl| envir.put(key, sl.value) },
					value
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
				widget = EZRanger(area, area.bounds.extent, key, key.asSpec,
					{ |sl| envir.put(key, sl.value) })
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
				{ arg ez; envir[key] = ez.textField.value.interpret; },
				value, false, labelWidth, labelHeight: 18);
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
