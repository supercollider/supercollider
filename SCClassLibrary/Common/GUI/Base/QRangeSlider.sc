RangeSlider : QAbstractStepValue {
	*qtClass { ^'QcRangeSlider' }

	*new { |parent, bounds|
		^super.new(parent, bounds).initQRangeSlider(bounds)
	}

	initQRangeSlider { |bounds|
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

	pixelStep {
		// FIXME for now we are using step instead
		^this.step
	}

	orientation_ { |aSymbol|
		this.setProperty(\orientation, QOrientation(aSymbol))
	}

	lo {
		^this.getProperty(\loValue)
	}

	lo_ { |aFloat|
		this.setProperty(\loValue, aFloat)
	}

	activeLo_ { |aFloat|
		this.lo_(aFloat);
		this.doAction
	}

	hi {
		^this.getProperty(\hiValue)
	}

	hi_ { |aFloat|
		this.setProperty(\hiValue, aFloat)
	}

	activeHi_ { |aFloat|
		this.hi_(aFloat);
		this.doAction
	}

	range {
		^(this.hi - this.lo)
	}

	range_ { |aFloat|
		this.hi_(this.lo + aFloat;)
	}

	activeRange_ { |aFloat|
		this.range_(aFloat);
		this.doAction
	}

	setSpan { |lo, hi|
		this.lo_(lo);

		// setting lo may have flipped the roles of the endpoints
		if(this.lo == lo) {
			this.hi_(hi)
		} {
			this.lo_(hi)
		}
	}

	setSpanActive { |lo, hi|
		this.setSpan(lo, hi);
		this.doAction
	}

	setDeviation { |deviation, average|
		this.setSpan(average - deviation, average + deviation)
	}

	knobColor { ^this.getProperty(\knobColor) }
	knobColor_ { |color| this.setProperty(\knobColor, color) }

	background { ^this.getProperty(\grooveColor) }
	background_ { |color| this.setProperty(\grooveColor, color) }

	defaultGetDrag { ^Point(this.lo, this.hi) }
	defaultCanReceiveDrag { ^(View.currentDrag.class === Point) }
	defaultReceiveDrag {
		var pt = View.currentDrag;
		this.setSpanActive(pt.x, pt.y)
	}
}
