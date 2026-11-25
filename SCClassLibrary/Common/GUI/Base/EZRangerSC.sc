EZRanger : EZGui {

	var <rangeSlider, <hiBox, <loBox, <unitView, <>controlSpec, <lo, <hi;
	var numSize, numberWidth, unitWidth;
	var <>round = 0.001;

	*new { |parent, bounds, label, controlSpec, action, initVal,
			initAction = false, labelWidth = 60, numberWidth = 45,
			unitWidth = 0, labelHeight = 20, layout = \horz, gap, margin|

		^super.new.init(parent, bounds, label, controlSpec, action,
			initVal, initAction, labelWidth, numberWidth,
			unitWidth, labelHeight, layout, gap, margin)
	}

	init { |parentView, bounds, label, argControlSpec, argAction, initVal,
			initAction, labelWidth, argNumberWidth, argUnitWidth,
			labelHeight, argLayout, argGap, argMargin|

		var labelBounds, hiBounds, loBounds, unitBounds, rangerBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		unitWidth = argUnitWidth;
		numberWidth = argNumberWidth;
		layout = argLayout;

		if(bounds.isNil) { bounds = 350 @ 20 };

		// if no parent, then pop up window
		# view, bounds = this.prMakeView(parentView, bounds);

		labelSize = labelWidth @ labelHeight;
		numSize = numberWidth @ labelHeight;

		// calcualate bounds
		# labelBounds, hiBounds, loBounds, rangerBounds, unitBounds
			= this.prSubViewBounds(innerBounds, label.notNil, unitWidth > 0);

		if(label.notNil) { //only add a label if desired
			labelView = StaticText.new(view, labelBounds);
			labelView.string = label
		};

		if(unitWidth > 0) { //only add a unitLabel if desired
			unitView = StaticText.new(view, unitBounds)
		};


		loBox = NumberBox.new(view, loBounds);
		rangeSlider = RangeSlider.new(view, rangerBounds);
		hiBox = NumberBox.new(view, hiBounds);

		controlSpec = argControlSpec.asSpec;
		if(unitWidth > 0) { unitView.string = " " ++ controlSpec.units.asString };
		action = argAction;

		loBox.action_({ |box| this.lo_(box.value).doAction });
		rangeSlider.action_({ |sl|
			this.lo_(controlSpec.map(sl.lo));
			this.hi_(controlSpec.map(sl.hi));
			this.doAction
		});
		hiBox.action_({ |box| this.hi_(box.value).doAction });

		if(initVal.notNil) { this.value_(initVal) };
		if(initAction) { this.doAction };


		//		if (controlSpec.step != 0) {
		//			rangeSlider.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		//		};

		numberStep = controlSpec.step;
		if(numberStep == 0) {
			numberStep = controlSpec.guessNumberStep
		} {
			// controlSpec wants a step, so zooming in with alt is disabled.
			hiBox.alt_scale = 1.0;
			loBox.alt_scale = 1.0;
			rangeSlider.alt_scale = 1.0
		};

		hiBox.step = numberStep;
		loBox.step = numberStep;
		hiBox.scroll_step = numberStep;
		loBox.scroll_step = numberStep;

		rangeSlider.receiveDragHandler = { |slider|
			slider.valueAction = controlSpec.unmap(View.currentDrag)
		};

		rangeSlider.beginDragAction = { |slider|
			controlSpec.map(slider.value)
		};

		this.prSetViewParams

	}

	doAction { action.value(this) }

	set { |label, spec, argAction, initVal, initAction = false|
		if(labelView.notNil) { labelView.string = label.asString };
		if(spec.notNil) { controlSpec = spec.asSpec };
		if(argAction.notNil) { action = argAction };

		initVal = initVal ? this.value ? controlSpec.default.dup(2);

		if(initAction) {
			this.valueAction_(initVal)
		} {
			this.value_(initVal)
		}
	}

	value { ^[lo, hi] }
	value_ { |vals| this.lo_(vals[0]).hi_(vals[1]) }
	valueAction_ { |vals| this.value_(vals).doAction }

	lo_ { |val|
		lo = controlSpec.constrain(val);
		loBox.value_(lo.round(round));
		rangeSlider.lo_(controlSpec.unmap(lo))
	}

	hi_ { |val|
		hi = controlSpec.constrain(val);
		hiBox.value_(hi.round(round));
		rangeSlider.hi_(controlSpec.unmap(hi))
	}
	setColors{ |stringBackground, stringColor, sliderColor, numBackground, numStringColor,
			numNormalColor, numTypingColor, knobColor, background|

		if(stringBackground.notNil) {
			if(labelView.notNil) { labelView.background_(stringBackground) };
			if(unitView.notNil) { unitView.background_(stringBackground) } };
		if(stringColor.notNil) {
			if(labelView.notNil) { labelView.stringColor_(stringColor) };
			if(unitView.notNil) { unitView.stringColor_(stringColor) } };
		if(sliderColor.notNil) {
			rangeSlider.background_(sliderColor) };
		if(numBackground.notNil) {
			hiBox.background_(numBackground);
			loBox.background_(numBackground) };
		if(numNormalColor.notNil) {
			hiBox.normalColor_(numNormalColor);
			loBox.normalColor_(numNormalColor) };
		if(numTypingColor.notNil) {
			hiBox.typingColor_(numTypingColor);
			loBox.typingColor_(numTypingColor) };
		if(numStringColor.notNil) {
			hiBox.stringColor_(numStringColor);
			loBox.stringColor_(numStringColor) };
		if(knobColor.notNil) {
			rangeSlider.knobColor_(knobColor) };
		if(background.notNil) {
			view.background = background };
		hiBox.refresh
	}

	font_{ |font|

		if(labelView.notNil) { labelView.font = font };
		if(unitView.notNil) { unitView.font = font };
		hiBox.font = font;
		loBox.font = font
	}


	prSetViewParams{

		switch(layout)
			{ \line2 } {
				if(labelView.notNil) {
					labelView.resize_(2);
					if(unitView.notNil) { unitView.resize_(3) };
					hiBox.resize_(3);
					loBox.resize_(3)
				} {
					if(unitView.notNil) {
						unitView.resize_(2).align_(\left) };
					hiBox.resize_(1);
					loBox.resize_(1)
				};
				rangeSlider.resize_(5);
				if(popUp) { view.resize_(2) }
			}
			{ \vert } {
				if(labelView.notNil) { labelView.resize_(2) };
				if(unitView.notNil) { unitView.resize_(8) };
				loBox.resize_(8);
				hiBox.resize_(2);
				rangeSlider.resize_(5);
				if(popUp) { view.resize_(4) }
			}
			{ \horz } {
				if(labelView.notNil) { labelView.resize_(4).align_(\right) };
				if(unitView.notNil) { unitView.resize_(6) };
				loBox.resize_(4);
				hiBox.resize_(6);
				rangeSlider.resize_(5);
				if(popUp) { view.resize_(2) }
			}
	}
	prSubViewBounds{ |rect, hasLabel, hasUnit|
		var hiBounds, loBounds, labelBounds, rangerBounds;
		var unitBounds, gap1, gap2, gap3, gap4, tmp, labelH, unitH;
		gap1 = gap.copy; // use copy to make sure these are unique
		gap2 = gap.copy; // since you later set the .x or .y
		gap3 = gap.copy;
		gap4 = gap.copy;
		labelH = labelSize.y;//  needed for \vert
		unitH = labelSize.y; //  needed for \vert

		switch(layout)
			{ \line2 } {
				if(hasLabel.not) { gap2 = 0 @ 0; labelSize.x = 0 };
				if(hasUnit.not) { gap4 = 0 @ 0; unitWidth = 0 };
				if(hasLabel) { // with label
					unitBounds = (unitWidth @ labelSize.y)
						.asRect.left_(rect.width - unitWidth);// view to right
					hiBounds = (numSize.x @ labelSize.y)
						.asRect.left_(rect.width - unitBounds.width - numberWidth - gap4.x); // view to right
					loBounds = (numSize.x @ labelSize.y)
						.asRect.left_(rect.width - unitBounds.width - numberWidth
								- gap3.x - hiBounds.width - gap4.x); // view to right
					labelBounds = (labelSize.x @ labelSize.y)
						.asRect.width_(loBounds.left - gap2.x) //adjust width
				} { // no label
					labelBounds = (0 @ labelSize.y).asRect; //just a dummy
					loBounds = (numberWidth @ labelSize.y).asRect; //view to left
					hiBounds = (numberWidth @ labelSize.y).asRect.moveTo(loBounds.width + gap3.x, 0); //view to left
					if(unitWidth > 0) {
						unitBounds = Rect.newSides (hiBounds.right + gap4.x,
							0, rect.width, labelSize.y) //adjust width
					} {
						unitBounds = Rect (0, 0, 0, 0) //no unitView
					}

				};
				rangerBounds = Rect(
					0,
					labelSize.y + gap1.y,
					rect.width,
					rect.height - numSize.y - gap1.y;
				)
			}

			{ \vert } {
				if(hasLabel.not) { labelH = 0; gap1.y = 0 };
				labelBounds = (rect.width @ labelH).asRect; // to top
				if(hasUnit.not) { unitH = 0; gap4 = 0 @ 0 };
				unitBounds = (rect.width @ unitH)
					.asRect.top_(rect.height - unitH); // to bottom
				hiBounds = (rect.width @ labelSize.y)
					.asRect.top_(labelBounds.height + gap1.y); // to bottom
				loBounds = (rect.width @ labelSize.y)
					.asRect.top_(rect.height - unitBounds.height - numSize.y - gap4.y); // to bottom
				rangerBounds = Rect.newSides(
					0,
					hiBounds.bottom + gap2.y,
					rect.width,
					loBounds.top - gap3.y
				)
			}

			{ \horz } {
				if(hasLabel.not) { gap1 = 0 @ 0; labelSize.x = 0 };
				if(hasUnit.not) { gap4 = 0 @ 0; unitWidth = 0 };
				labelSize.y = rect.height;
				labelBounds = (labelSize.x @ labelSize.y).asRect;
				unitBounds = (unitWidth @ labelSize.y).asRect.left_(rect.width - unitWidth);
				loBounds = (numSize.x @ labelSize.y).asRect
					.left_(labelBounds.width + gap1.x);
				hiBounds = (numSize.x @ labelSize.y).asRect
					.left_(rect.width - unitBounds.width - numSize.x - gap4.x);
				rangerBounds = Rect.newSides(
					loBounds.right + gap2.x, 0,
					rect.width - hiBounds.width - unitBounds.width
						- gap3.x - gap4.x,
					labelSize.y)

			};


		^[labelBounds, hiBounds, loBounds, rangerBounds, unitBounds]
			.collect { |v| v.moveBy(margin.x, margin.y) }
	}


}
