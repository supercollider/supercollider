NumberBox : QAbstractStepValue {
	var <scroll, <scroll_step;
	var <align, <buttonsVisible = false;
	var <normalColor, <typingColor;
	var <object, <>setBoth = true;

	*qtClass { ^'QcNumberBox' }

	*new { |parent, bounds|
		var obj = super.new(parent, bounds);
		obj.initNumberBox;
		^obj
	}

	initNumberBox {
		scroll = true;
		scroll_step = 1;
		normalColor = Color.black;
		typingColor = Color.red
	}

	object_  { |obj|
		if(setBoth) {
			if(obj.isNumber) { this.value = obj } { this.string = obj.asString }
		};
		object = obj
	}

	value {
		var type = this.getProperty(\valueType);
		var val;
		switch(type,
			0 /* Number */, { val = this.getProperty(\value) },
			1 /* Inf */, { val = inf },
			2 /* -Inf */, { val = -inf },
			3 /* NaN */, { val = 0 },
			4 /* Text */, { val = 0 }
		);
		^val
	}

	value_ { |value|
		case
			// isNaN has to be on the first plase, because a NaN is also equal to inf and -inf
			{ value.isNaN } { this.invokeMethod(\setNaN) }
			{ value == inf } { this.invokeMethod(\setInfinite, true) }
			{ value == -inf } { this.invokeMethod(\setInfinite, false) }
			{ this.setProperty(\value, value.asFloat) }
	}

	valueAction_ { |val|
		this.value_(val);
		action.value(this)
	}

	string { ^this.getProperty(\text) }

	string_ { |string| this.setProperty(\text, string) }

	clipLo { ^this.getProperty(\minimum) }
	clipLo_ { |aFloat| this.setProperty(\minimum, aFloat) }

	clipHi { ^this.getProperty(\maximum) }
	clipHi_ { |aFloat| this.setProperty(\maximum, aFloat) }

	scroll_ { |aBool|
		scroll = aBool;
		this.setProperty(\scroll, aBool)
	}

	scroll_step_ { |aFloat|
		scroll_step = aFloat;
		this.setProperty(\scrollStep, aFloat)
	}

	decimals { ^this.getProperty(\decimals) }
	minDecimals { ^this.getProperty(\minDecimals) }
	maxDecimals { ^this.getProperty(\maxDecimals) }

	decimals_ { |decimals| this.setProperty(\decimals, decimals) }
	minDecimals_ { |decimals| this.setProperty(\minDecimals, decimals) }
	maxDecimals_ { |decimals| this.setProperty(\maxDecimals, decimals) }

	align_ { |alignment|
		align = alignment;
		this.setProperty(\alignment, QAlignment(alignment))
	}

	stringColor {
		^this.palette.baseText
	}

	stringColor_ { |color|
		this.palette = this.palette.baseText_(color)
	}

	normalColor_ { |aColor|
		normalColor = aColor;
		this.setProperty(\normalColor, aColor)
	}

	typingColor_ { |aColor|
		typingColor = aColor;
		this.setProperty(\editingColor, aColor)
	}

	background {
		^this.palette.base
	}

	background_ { |color|
		this.palette = this.palette.base_(color)
	}

	buttonsVisible_ { |aBool|
		buttonsVisible = aBool;
		this.setProperty(\buttonsVisible, aBool)
	}

	defaultGetDrag { ^this.value }
	defaultCanReceiveDrag { ^View.currentDrag.isNumber }
	defaultReceiveDrag {
		this.valueAction = View.currentDrag
	}
}
