Slider2D : QAbstractStepValue {
	*qtClass { ^'QcSlider2D' }

	*new { |parent, bounds|
		var me = super.new(parent, bounds);
		me.connectMethod('randomize()', \randomize);
		^me
	}

	pixelStepX {
		// FIXME for now we are using step instead
		^this.step
	}

	pixelStepY {
		// FIXME for now we are using step instead
		^this.step
	}

	x {
		^this.getProperty(\xValue)
	}

	x_ { |aFloat|
		this.setProperty(\xValue, aFloat)
	}

	activex_ { |aFloat|
		this.x_(aFloat);
		this.doAction
	}

	y {
		^this.getProperty(\yValue)
	}

	y_ { |aFloat|
		this.setProperty(\yValue, aFloat)
	}

	activey_ { |aFloat|
		this.y_(aFloat);
		this.doAction
	}

	setXY { |x, y|
		this.x_(x);
		this.y_(y)
	}

	setXYActive { |x, y|
		this.setXY(x, y);
		this.doAction
	}

	incrementX { |factor = 1.0| this.invokeMethod(\incrementX, factor.asFloat) }
	decrementX { |factor = 1.0| this.invokeMethod(\decrementX, factor.asFloat) }
	incrementY { |factor = 1.0| this.invokeMethod(\incrementY, factor.asFloat) }
	decrementY { |factor = 1.0| this.invokeMethod(\decrementY, factor.asFloat) }

	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |pixels| this.setProperty(\thumbSize, pixels) }

	randomize {
		this.setXYActive(1.0.rand, 1.0.rand)
	}

	knobColor { ^this.getProperty(\knobColor) }
	knobColor_ { |color| this.setProperty(\knobColor, color) }

	background { ^this.getProperty(\grooveColor) }
	background_ { |color| this.setProperty(\grooveColor, color) }

	defaultGetDrag { ^Point(this.x, this.y) }
	defaultCanReceiveDrag { ^(View.currentDrag.class === Point) }
	defaultReceiveDrag {
		var pt = View.currentDrag;
		this.setXYActive(pt.x, pt.y)
	}
}
