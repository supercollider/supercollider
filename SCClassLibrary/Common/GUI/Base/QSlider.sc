Slider : QAbstractStepValue {
	//compatibility stuff:
	var <orientation;

	*qtClass { ^'QcSlider' }

	*new { |parent, bounds|
		^super.new(parent, bounds).initSlider(bounds)
	}

	value {
		^this.getProperty(\value)
	}

	value_ { |argVal|
		this.setProperty(\value, argVal)
	}

	valueAction_ { |val|
		this.value_(val);
		action.value(this)
	}

	step { ^this.getProperty(\step) }

	thumbSize { ^this.getProperty(\handleLength) }
	thumbSize_ { |pixels| this.setProperty(\handleLength, pixels) }

	knobColor { ^this.getProperty(\knobColor) }
	knobColor_ { |color| this.setProperty(\knobColor, color) }

	background { ^this.getProperty(\grooveColor) }
	background_ { |color| this.setProperty(\grooveColor, color) }

	initSlider { |bounds|
		var r;
		if(bounds.notNil) {
			r = bounds.asRect;
			if(r.width > r.height) {
				this.orientation_(\horizontal)
			} {
				this.orientation_(\vertical)
			}
		}
	}

	pixelStep { ^this.getProperty(\pixelStep) }

	orientation_ { |aSymbol|
		orientation = aSymbol;
		this.setProperty(\orientation, QOrientation(aSymbol))
	}

	defaultKeyDownAction { |char, modifiers, unicode, keycode, key|
		var scale = this.getScale(modifiers);
		switch(char)
			{ $r } { this.valueAction = 1.0.rand }
			{ $n } { this.valueAction = 0.0 }
			{ $x } { this.valueAction = 1.0 }
			{ $c } { this.valueAction = 0.5 }
			{
				switch(key,
					16r5d, { this.increment(scale) },
					16r1000013, { this.increment(scale) },
					16r1000014, { this.increment(scale) },
					16r5b, { this.decrement(scale) },
					16r1000015, { this.decrement(scale) },
					16r1000012, { this.decrement(scale) },
					{ ^this } // if unhandled, let Qt process the event
				);
				this.doAction
			};
		^true // accept the event and stop its processing
	}

	defaultGetDrag { ^this.value }
	defaultCanReceiveDrag { ^View.currentDrag.isNumber }
	defaultReceiveDrag {
		this.valueAction = View.currentDrag
	}
}
