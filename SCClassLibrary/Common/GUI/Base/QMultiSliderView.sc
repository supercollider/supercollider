MultiSliderView : View {
	var <editable = true;
	var <elasticMode = false;
	var <indexThumbSize = 12, <valueThumbSize = 12, <gap = 1;
	var <drawLines = false, <drawRects = true;
	var <metaAction;


	*qtClass { ^'QcMultiSlider' }

	size { ^this.getProperty(\sliderCount) }
	size_ { |int| this.setProperty(\sliderCount, int) }

	indexIsHorizontal { ^this.getProperty(\orientation) == QOrientation(\vertical) }

	indexIsHorizontal_ { |bool|
		this.setProperty(\orientation, QOrientation(if(bool) { \vertical } { \horizontal }))
	}

	editable_ { |aBool|
		editable = aBool;
		this.setProperty(\editable, aBool)
	}

	readOnly {
		^editable.not
	}

	readOnly_ { |bool|
		this.editable_(bool.not)
	}

	step { ^this.getProperty(\step) }
	step_ { |val| this.setProperty(\step, val) }

	value {
		^this.getProperty(\values)
	}

	value_ { |array|
		if(array.isKindOf(DoubleArray).not and: { array.isKindOf(FloatArray).not })
			{ array = array.as(DoubleArray) };
		this.setProperty(\values, array)
	}

	valueAction_ { |val|
		this.value_(val);
		action.value(this)
	}

	currentvalue {
		^this.getProperty(\value)
	}

	currentvalue_ { |aFloat|
		this.setProperty(\value, aFloat)
	}

	index {
		^this.getProperty(\index)
	}

	index_ { |anInt|
		this.setProperty(\index, anInt)
	}

	selectionSize {
		^this.getProperty(\selectionSize)
	}

	selectionSize_ { |anInt|
		this.setProperty(\selectionSize, anInt)
	}

	reference { ^this.getProperty(\reference) }

	reference_ { |array|
		if(array.isKindOf(DoubleArray).not and: { array.isKindOf(FloatArray).not })
			{ array = array.as(DoubleArray) };
		this.setProperty(\reference, array)
	}

	startIndex_ { |anInt|
		this.setProperty(\startIndex, anInt)
	}

	elasticMode_ { |int|
		elasticMode = int.booleanValue;
		this.setProperty(\elastic, elasticMode)
	}

	thumbSize_ { |float|
		this.indexThumbSize_(float);
		this.valueThumbSize_(float)
	}

	indexThumbSize_ { |float|
		indexThumbSize = float;
		this.setProperty(\indexThumbSize, float)
	}

	valueThumbSize_ { |float|
		valueThumbSize = float;
		this.setProperty(\valueThumbSize, float)
	}

	gap_ { |anInt|
		gap = anInt;
		this.setProperty(\gap, anInt)
	}

	// alias for 'gap'
	xOffset_ { |int| this.gap_(int) }
	xOffset { |int| ^this.gap }

	drawLines_ { |bool|
		drawLines = bool;
		this.setProperty(\drawLines, bool)
	}

	drawRects_ { |bool|
		drawRects = bool;
		this.setProperty(\drawRects, bool)
	}

	showIndex_ { |aBool|
		this.setProperty(\highlight, aBool)
	}

	isFilled_ { |aBool|
		this.setProperty(\isFilled, aBool)
	}

	fillColor { ^this.getProperty(\fillColor) }
	fillColor_ { |color| this.setProperty(\fillColor, color) }

	strokeColor { ^this.getProperty(\strokeColor) }
	strokeColor_ { |color| this.setProperty(\strokeColor, color) }

	colors_ { |colorStroke, colorFill|
		this.strokeColor_(colorStroke);
		this.fillColor_ (colorFill)
	}

	metaAction_ { |func|
		this.manageMethodConnection(metaAction, func, 'metaAction()', \doMetaAction);
		metaAction = func
	}

	doMetaAction {
		metaAction.value(this)
	}

	defaultKeyDownAction { |char, mod, uni, keycode, key|
		switch(key,
			QKey.left, { this.index = this.index - 1 },
			QKey.right, { this.index = this.index + 1 },
			QKey.up, { this.currentvalue = this.currentvalue + this.step },
			QKey.down, { this.currentvalue = this.currentvalue - this.step }
		)
	}

	defaultGetDrag {
		var val = this.value;
		var c, i;
		if(val.size < 1) { ^nil };
		c = this.selectionSize;
		if(c > 1) {
			i = this.index;
			^val[i..(i + c - 1)]
		}
		^this.value
	}
	defaultCanReceiveDrag { ^true }
	defaultReceiveDrag { |data = (View.currentDrag)|
		if(data.size > 0) {
			if(data[0].size > 0) {
				this.value = data[0];
				this.reference = data[1]
			} {
				this.value = data
			}
		}
	}
}
