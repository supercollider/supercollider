
ParamView {
	var <value, <label, <>action, <spec;
	var <zone, <zones, <slider, <ranger, <textview;
	var <ezviews, <currview, <viewType;

	*new { |parent, bounds, label, spec, action, initVal, initAction = false|
		^super.new.init(parent, bounds, label, spec, action, initVal, initAction);
	}

	bounds { ^zone.bounds }
	bounds_ { |bounds| zone.bounds_(bounds) }

	background { ^currview.labelView.background }
	background_ { |col|
		ezviews.do { |ez| ez.labelView.background_(col) }
	}

	label_ { |alabel|
		label = alabel;
		ezviews.do { |ez| ez.labelView.string_(alabel) };
	}

	enabled_ { |bool=true| zone.enabled_(bool) }
	enabled { ^zone.enabled }
	visible_ { |bool=true| zone.visible_(bool) }
	visible { ^zone.visible }

	init { |parent, bounds, argLabel, argSpec, argAction, initVal, initAction, initType = 0|
		var rect2 = bounds.moveTo(0,0);
		zone = CompositeView(parent, bounds);
		zones = 3.collect { |i| CompositeView(zone, rect2); };
		zone.resize_(2);
		zones.do(_.resize_(2));

		label = argLabel ? "-";
		spec = argSpec.asSpec;
		action = argAction;

		slider = EZSlider(zones[0], rect2, label, spec);
		ranger = EZRanger(zones[1], rect2, label, spec);
		textview = EZText(zones[2], rect2, label);

		zones.do { |z| z.children[0].resize_(2) };

		ezviews = [slider, ranger, textview];
		ezviews.do { |ez|
			ez.action_({ |ez| this.valueAction_(ez.value) });
		};
		if (initVal.notNil) { this.value(initVal) };
		if (initAction) { this.doAction };
	}

	// types: 0 = slider, 1 = ranger, 2 = text
	viewType_ { |newType = 0, force = false|
		if (force or: { newType != viewType }) {
			if (newType.inclusivelyBetween(0, 2)) {
				zones.do { |z, i| z.visible_(newType == i) };
				viewType = newType;
				currview = ezviews[newType];
			};
		};
	}

	valueType { |newval|
		^case
		{ newval.isNumber } { 0 }
		{ newval.isKindOf(Array) and:
			{ newval.size == 2 and:
				{ newval.every(_.isNumber) }
			}
		} { 1 }
		{ 2 }
	}

	value_ { |val|
		if (val != value) {
			this.viewType_(this.valueType(val));
		};
		value = val;
		currview.value_(value);
	}

	doAction { action.value(this) }
	valueAction_ { |val| this.value_(val).doAction }

	spec_ { |newspec|
		spec = newspec.asSpec;
		slider.controlSpec_(spec);
		ranger.controlSpec_(spec);
	}
}
