EnvelopeView : View {
	var <editable, <step, <grid, <gridOn = false;
	var <fillColor;
	var <drawLines = true, <drawRects = true;
	var <metaAction;

	*qtClass { ^'QcGraph' }

	editable_ { |aBool|
		editable = aBool;
		this.setProperty(\editable, aBool)
	}

	setEditable { |index, flag|
		this.invokeMethod(\setEditableAt, [index, flag])
	}

	step_ { |aFloat|
		step = aFloat;
		this.setProperty(\step, aFloat)
	}

	keepHorizontalOrder {
		^this.getProperty(\horizontalOrder) != 0
	}

	keepHorizontalOrder_ { |bool|
		this.setProperty(\horizontalOrder, if(bool) { 1 } { 0 })
	}

	elasticSelection {
		^this.getProperty(\selectionForm) == 0
	}

	elasticSelection_ { |bool|
		this.setProperty(\selectionForm, if(bool) { 0 } { 1 })
	}

	value {
		^this.getProperty(\value)
	}

	value_ { |anArray|
		this.setProperty(\value, anArray)
	}

	valueAction_ { |anArray|
		this.setProperty(\value, anArray);
		this.doAction
	}

	index {
		^this.getProperty(\index)
	}

	index_ { |index|
		^this.setProperty(\index, index)
	}

	lastIndex { ^this.getProperty(\lastIndex) }

	selection { ^this.getProperty(\selectionIndexes) }

	selectIndex { |index|
		if(index < 0) {
			this.invokeMethod(\deselectAll)
		} {
			this.invokeMethod(\select, [index, false])
		};
		this.index = index
	}

	deselectIndex { |index|
		this.invokeMethod(\deselect, index)
	}

	x {
		^this.getProperty(\x)
	}

	x_ { |aFloat|
		this.setProperty(\x, aFloat)
	}

	y {
		^this.getProperty(\y)
	}

	y_ { |aFloat|
		this.setProperty(\y, aFloat)
	}

	currentvalue { ^this.y }

	currentvalue_ { |aFloat|
		this.y_(aFloat)
	}

	setString { |index, string|
		this.invokeMethod(\setStringAt, [index, string])
	}

	strings_ { |anArray|
		this.setProperty(\strings, anArray)
	}

	curves { this.nonimpl("curves") }

	curves_ { |curves|
		this.invokeMethod(\setCurves,
			if(curves.size > 0) { [curves.collect { |c| QCurve(c) }] } { QCurve(curves) }
		)
	}

	setEnv { |env|
		var times = [0] ++ env.times.integrate;
		if(times.last > 0) { times = times / times.last };
		this.value = [times, env.levels];
		this.curves = env.curves
	}

	grid_ { |aPoint|
		grid = aPoint;
		this.setProperty(\grid, aPoint)
	}

	gridOn_ { |aBool|
		gridOn = aBool;
		this.setProperty(\gridOn, aBool)
	}

	connect { |source, targets|
		this.invokeMethod(\connectElements, [source, targets])
	}

	gridColor { ^this.getProperty(\gridColor) }
	gridColor_ { |color| this.setProperty(\gridColor, color) }

	selectionColor { ^this.getProperty(\selectionColor) }
	selectionColor_ { |color| this.setProperty(\selectionColor, color) }

	strokeColor { ^this.getProperty(\strokeColor) }
	strokeColor_ { |color| this.setProperty(\strokeColor, color) }

	fillColor_ { |aColor|
		fillColor = aColor;
		this.setProperty(\fillColor, aColor)
	}

	setFillColor { |index, color|
		this.invokeMethod(\setFillColorAt, [index, color])
	}

	colors_ { |strokeColor, fillColor|
		this.strokeColor_(strokeColor);
		this.fillColor_(fillColor)
	}

	drawLines_ { |aBool|
		drawLines = aBool;
		this.setProperty(\drawLines, aBool)
	}

	drawRects_ { |aBool|
		drawRects = aBool;
		this.setProperty(\drawRects, aBool)
	}

	style { ^this.getProperty(\style) }

	style_ { |style|
		if(style.isNumber.not) {
			style = switch(style,
				\dots, 0,
				\rects, 1,
				0
			)
		};
		style = style.clip(0, 1).asInteger;
		this.setProperty(\style, style)
	}

	thumbWidth_ { |width|
		this.setProperty(\thumbWidth, width.asInteger;)
	}

	thumbHeight_ { |height|
		this.setProperty(\thumbHeight, height.asInteger;)
	}

	thumbSize_ { |size|
		this.setProperty(\thumbSize, size.asInteger;)
	}

	setThumbWidth { |index, width|
		this.invokeMethod(\setThumbWidthAt, [index, width.asInteger])
	}

	setThumbHeight { |index, height|
		this.invokeMethod(\setThumbHeightAt, [index, height.asInteger])
	}

	setThumbSize { |index, size|
		this.invokeMethod(\setThumbSizeAt, [index, size.asInteger])
	}

	metaAction_ { |function|
		this.manageMethodConnection(metaAction, function, 'metaAction()', \doMetaAction);
		metaAction = function
	}

	doMetaAction {
		metaAction.value(this)
	}

	defaultGetDrag {
		^this.value
	}
	defaultCanReceiveDrag { ^true }
	defaultReceiveDrag {
		this.value = View.currentDrag
	}
}
