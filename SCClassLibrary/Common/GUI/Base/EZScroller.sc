EZScroller {
	var <maxItems=1, <numItems=1, <>action, <spec;
	var <value, <slider;

	*new { |w, bounds, maxItems, numItems, action, initVal=0|
		^super.newCopyArgs(maxItems, numItems, action)
			.init(w, bounds, initVal);
	}
	init { |w, bounds, initVal|
		slider = GUI.slider.new(w, bounds);
		slider.action = { |sl|
			this.valueAction_(spec.map(sl.value));
		};
		slider.keyDownAction { arg char, modifiers, unicode,keycode;
			if (unicode == 16rF700, { this.increment; ^this });
			if (unicode == 16rF703, { this.increment; ^this });
			if (unicode == 16rF701, { this.decrement; ^this });
			if (unicode == 16rF702, { this.decrement; ^this });
		};
		spec = [0, 0, \lin, 1].asSpec;
		this.adjust;
		this.value_(initVal);
	}
	increment { this.valueAction = this.value + this.spec.step }
	decrement { this.valueAction = this.value + this.spec.step }

	adjust {
		var slBounds = slider.bounds;
		var maxLength = slBounds.width max: slBounds.height + 2;
		var numTooMany = (numItems - maxItems).max(1);
		var fractionToShow = (maxItems / numItems).min(1);
		if (GUI.scheme.id == \cocoa) {
			// swingOSC posts a 'not implemented yet' warning.
			slider.thumbSize = fractionToShow * maxLength;
		};
	//	slider.step_(1 / numTooMany.max(1));		// this does the action - it should not.
		slider.setProperty(\step, 1 / numTooMany.max(1));
		spec.minval_(numTooMany);	// minval to invert spec
	}

	value_ { |val|
		value = spec.constrain(val);
		slider.value_(spec.unmap(value));
	}

	maxItems_ { |val| maxItems = val.max(1); this.adjust }
	numItems_ { |val| numItems = val.max(1); this.adjust }

	valueAction_ { |val| this.value_(val).doAction; }
	doAction { action.value(this) }

	enabled { ^slider.enabled }
	enabled_ { |flag| slider.enabled_(flag) }
	visible { ^slider.visible }
	visible_ { |flag| slider.visible_(flag) }

	remove { slider.remove }
}
