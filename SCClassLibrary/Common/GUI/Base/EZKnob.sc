EZKnob : EZGui {

	classvar <>compactRatio = 0.87;

	var <knobView, <numberView, <unitView, <>controlSpec, knobSize, unitWidth;
	var <>round = 0.001;

	*new { |parent, bounds, label, controlSpec, action, initVal,
			initAction = false, labelWidth = 60, knobSize,
			unitWidth = 0, labelHeight = 20, layout = \vert, gap, margin|

		^super.new.init(parent, bounds, label, controlSpec, action,
			initVal, initAction, labelWidth, knobSize,
			unitWidth, labelHeight, layout, gap, margin)
	}

	init { |parentView, bounds, label, argControlSpec, argAction, initVal,
			initAction, labelWidth, argKnobSize, argUnitWidth,
			labelHeight, argLayout, argGap, argMargin|

		var labelBounds, numBounds, unitBounds, knobBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		unitWidth = argUnitWidth;
		layout = argLayout;
		if(bounds.isNil) { bounds = 50 @ 90 };

		knobSize = argKnobSize;

		// if no parent, then pop up window
		# view, bounds = this.prMakeView(parentView, bounds);


		labelSize = labelWidth @ labelHeight;

		// calculate bounds of all subviews
		# labelBounds, numBounds, knobBounds, unitBounds
			= this.prSubViewBounds(innerBounds, label.notNil, unitWidth > 0);

		// instert the views
		if(label.notNil) { //only add a label if desired
			labelView = StaticText.new(view, labelBounds);
			labelView.string = label
		};

		if(unitWidth > 0) { //only add a unitLabel if desired
			unitView = StaticText.new(view, unitBounds)
		};

		knobView = Knob.new(view, knobBounds);
		numberView = NumberBox.new(view, numBounds);

		// set view parameters and actions
		controlSpec = argControlSpec.asSpec;
		if(unitWidth > 0) { unitView.string = " " ++ controlSpec.units.asString };
		initVal = initVal ? controlSpec.default;
		action = argAction;

		numberStep = controlSpec.step;
		if(numberStep == 0) {
			numberStep = controlSpec.guessNumberStep
		} {
			// controlSpec wants a step, so zooming in with alt is disabled.
			numberView.alt_scale = 1.0;
			knobView.alt_scale = 1.0
		};
		numberView.step = numberStep;
		numberView.scroll_step = numberStep;


		if((controlSpec.minval + controlSpec.maxval) == 0) { knobView.centered = true };

		knobView.action = {
			this.valueActionIfChanged_(controlSpec.map(knobView.value))
		};

		if(controlSpec.step != 0) {
			knobView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval))
		};

		knobView.receiveDragHandler = { |slider|
			slider.valueAction = controlSpec.unmap(View.currentDrag)
		};

		knobView.beginDragAction = { |slider|
			controlSpec.map(slider.value)
		};

		numberView.action = { this.valueActionIfChanged_(numberView.value) };

		if(initAction) {
			this.valueAction_(initVal)
		} {
			this.value_(initVal)
		};

		this.prSetViewParams
	}
	// just set, no action
	value_ { |val|
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
		knobView.value = controlSpec.unmap(value)
	}
	// set and do action
	valueAction_ { |val|
		this.value_(val);
		this.doAction
	}

	doAction { action.value(this) }

	set { |label, spec, argAction, initVal, initAction = false|
		if(labelView.notNil) { labelView.string = label.asString };
		if(spec.notNil) { controlSpec = spec.asSpec };
		if(argAction.notNil) { action = argAction };
		initVal = initVal ? value ? controlSpec.default;

		if(initAction) {
			this.valueAction_(initVal)
		} {
			this.value_(initVal)
		}
	}

	centered_ { |bool| knobView.centered_(bool) }

	centered{ ^knobView.centered }


	setColors{ |stringBackground, stringColor, numBackground,
			numStringColor, numNormalColor, numTypingColor, knobColors, background|

		if(stringBackground.notNil) {
			if(labelView.notNil) { labelView.background_(stringBackground) };
			if(unitView.notNil) { unitView.background_(stringBackground) } };
		if(stringColor.notNil) {
			if(labelView.notNil) { labelView.stringColor_(stringColor) };
			if(unitView.notNil) { unitView.stringColor_(stringColor) } };
		if(numBackground.notNil) {
			numberView.background_(numBackground) };
		if(numNormalColor.notNil) {
			numberView.normalColor_(numNormalColor) };
		if(numTypingColor.notNil) {
			numberView.typingColor_(numTypingColor) };
		if(numStringColor.notNil) {
			numberView.stringColor_(numStringColor) };
		if(knobColors.notNil) {
			knobView.color_(knobColors) };
		if(background.notNil) {
			view.background = background };
		numberView.refresh;
		knobView.refresh
	}

	font_{ |font|

		if(labelView.notNil) { labelView.font = font };
		if(unitView.notNil) { unitView.font = font };
		numberView.font = font
	}

	///////Private methods ///////

	prSetViewParams{ // sets resize and alignment for different layouts

		switch(layout)
			{ \line2 } {
				if(labelView.notNil) {
					labelView.resize_(5);
					if(unitView.notNil) { unitView.resize_(9) };
					numberView.resize_(8)
				} {
					if(unitView.notNil) {
						unitView.resize_(6);
						numberView.resize_(5)
					} {
						numberView.resize_(5)
					}
				};
				knobView.resize_(9);
				if(popUp) { view.resize_(2) }
			}
			{ \vert } {
				if(labelView.notNil) { labelView.resize_(2) };
				if(unitView.notNil) { unitView.resize_(6) };
				numberView.resize_(5);
				if(popUp) { view.resize_(2) }
			}
			{ \vert2 } {
				if(labelView.notNil) { labelView.resize_(2).align_(\center) };
				if(unitView.notNil) { unitView.resize_(6) };
				numberView.resize_(5);
				if(popUp) { view.resize_(2) }
			}
			{ \horz } {
				if(labelView.notNil) { labelView.resize_(4).align_(\right) };
				if(unitView.notNil) { unitView.resize_(6) };
				numberView.resize_(5);
				knobView.resize_(9);
				if(popUp) { view.resize_(2) }
			}

	}

	prSubViewBounds{ |rect, hasLabel, hasUnit|  // calculate subview bounds
		var numBounds, labelBounds, knobBounds, unitBounds, knobHeight, numHeight;
		var gap1, gap2, gap3, tmp, labelH, unitH;
		gap1 = gap.copy;
		gap2 = gap.copy;
		gap3 = gap.copy;
		if(rect.height <= (labelSize.y * 2)) { labelSize.y = rect.height / 2 };

		numHeight = labelSize.y;

		switch(layout)
			{ \line2 } {
				if(knobSize.isNil) { knobSize = (((rect.height / compactRatio) - margin.x) @ (rect.height)) };
				knobSize = (knobSize.x - margin.x) @ (knobSize.y.min(rect.height));
				if(hasUnit.not) { gap2 = 0 @ 0; unitWidth = 0 };
				labelBounds = Rect(0, 0, rect.width - knobSize.x - gap3.x, labelSize.y); //to left
				if(hasLabel) {
					unitBounds = Rect(labelBounds.width - unitWidth, labelSize.y + gap1.y,
						unitWidth, rect.height - labelSize.y - gap1.y);

					numBounds = Rect(0, labelSize.y + gap1.y,
						labelBounds.width - gap2.x - unitBounds.width, rect.height - labelSize.y - gap1.y)

				} {
					unitBounds = Rect(labelBounds.width - unitWidth, 0,
						unitWidth, rect.height);

					numBounds = Rect(0, 0,
						labelBounds.width - gap2.x - unitBounds.width, rect.height)

				};
				knobBounds = knobSize.asRect.moveTo(rect.width - knobSize.x, 0)
			}
			{ \horz } {
				if(knobSize.isNil) { knobSize = (((rect.height / compactRatio) - margin.x) @ (rect.height)) };
				knobSize = (knobSize.x - margin.x) @ (knobSize.y.min(rect.height));
				knobBounds = knobSize.asRect.moveTo(rect.width - knobSize.x, 0);
				if(hasUnit.not) { gap2 = 0 @ 0; unitWidth = 0 };
				if(hasLabel.not) { gap1 = 0 @ 0; labelSize.x = 0 };
				labelBounds = (labelSize.x @ rect.height).asRect;
				unitBounds = (unitWidth @ rect.height).asRect
					.moveTo(rect.width - knobBounds.width - gap2.x - unitWidth, 0);
				numBounds = Rect.newSides(labelSize.x + gap1.x, 0, unitBounds.left - gap2.x, rect.height)
			}
			{ \vert } {
				if(hasUnit.not) { gap3 = 0 @ 0; unitWidth = 0 };
				if(hasLabel.not) { gap1 = 0 @ 0; labelSize.y = 0 };

				if(knobSize.isNil) {
					knobSize = (((rect.height - labelSize.y - numHeight - gap1.y - gap2.y) / compactRatio)
							@ (rect.height - labelSize.y - numHeight - gap1.y - gap2.y))
				};
				knobSize = ((knobSize.x).min(rect.width))
					@ (knobSize.y.min(rect.height - labelSize.y - numHeight - gap1.y - gap2.y));

				labelBounds = (rect.width @ labelSize.y).asRect;
				knobBounds = knobSize.asRect.moveTo(0, labelSize.y + gap1.y);

				numBounds = Rect(0, rect.height - numHeight, rect.width - unitWidth - gap3.x, numHeight);

				unitBounds = Rect(rect.width - unitWidth, rect.height - numHeight, unitWidth, numHeight)

			}
			{ \vert2 } {
				if(hasUnit.not) { gap3 = 0 @ 0; unitWidth = 0 };
				if(hasLabel.not) { gap1 = 0 @ 0; labelSize.y = 0 };

				if(knobSize.isNil) {
					knobSize = (((rect.height - labelSize.y - numHeight - gap1.y - gap2.y) / compactRatio)
							@ (rect.height - labelSize.y - numHeight - gap1.y - gap2.y))
				};
				knobSize = ((knobSize.x).min(rect.width))
					@ (knobSize.y.min(rect.height - labelSize.y - numHeight - gap1.y - gap2.y));


				labelBounds = (rect.width @ labelSize.y).asRect;
				knobBounds = knobSize.asRect.moveTo(0, labelSize.y + gap1.y);
				knobBounds = knobBounds.moveBy((rect.width - knobBounds.width) / 2, 0);
				numBounds = Rect(0, rect.height - numHeight, rect.width - unitWidth - gap3.x, numHeight);

				unitBounds = Rect(rect.width - unitWidth, rect.height - numHeight, unitWidth, numHeight)

			};
		if((knobBounds.height < 0) || (knobBounds.width < 0)) { knobBounds = knobBounds.height_(0).width_(0) };

		^[labelBounds, numBounds, knobBounds, unitBounds].collect { |v| v.moveBy(margin.x, margin.y) }
	}
}
