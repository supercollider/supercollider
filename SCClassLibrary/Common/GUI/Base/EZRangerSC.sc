EZRanger : EZGui {

	var <rangeSlider, <hiBox,<loBox, <unitView, <>controlSpec, <lo, <hi;
	var numSize, numberWidth, unitWidth;
	var <>round = 0.001;

	*new { arg parent, bounds, label, controlSpec, action, initVal,
			initAction=false, labelWidth=60, numberWidth=45,
			unitWidth=0, labelHeight=20,  layout=\horz, gap,margin;

		^super.new.init(parent, bounds, label, controlSpec, action,
			initVal, initAction, labelWidth, numberWidth,
				unitWidth, labelHeight, layout, gap, margin)
	}

	init { arg parentView, bounds, label, argControlSpec, argAction, initVal,
			initAction, labelWidth, argNumberWidth,argUnitWidth,
			labelHeight, argLayout, argGap,argMargin;

		var innerBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		unitWidth = argUnitWidth;
		numberWidth = argNumberWidth;
		layout=argLayout;

		bounds.isNil.if{bounds = 350@20};

		// if no parent, then pop up window
		# view,bounds = this.prMakeView( parentView,bounds);

		innerBounds = bounds.insetBy(margin.x, margin.y);

		if(argLayout == \horz) {
			labelSize = labelWidth@innerBounds.height;
			numSize = numberWidth@innerBounds.height;
		} {
			labelSize = innerBounds.width@labelHeight;
			numSize = innerBounds.width@labelHeight;
		};

		label.notNil.if{ //only add a label if desired
			labelView = StaticText.new;
			labelView.fixedWidth = labelWidth;
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = StaticText.new;
			unitView.fixedWidth = unitWidth;
		};


		loBox = NumberBox.new;
		hiBox = NumberBox.new;
		if(argLayout == \horz) {
			loBox.fixedWidth_(numSize.x);
			hiBox.fixedWidth_(numSize.x);
		} {
			loBox.fixedHeight_(labelHeight);
			hiBox.fixedHeight_(labelHeight);
		};
		rangeSlider = RangeSlider.new;
		if(argLayout == \horz) {
			rangeSlider.orientation = \horizontal;
		} {
			rangeSlider.orientation = \vertical;
		};

		if(argLayout == \horz) {
			view.layout = HLayout.new(*[labelView, unitView, loBox, rangeSlider, hiBox].reject(_.isNil));
		} {
			view.layout = VLayout.new(*[labelView, unitView, hiBox, rangeSlider, loBox].reject(_.isNil));
		};
		view.layout.margins_([margin.x, margin.y])
		.spacing_(gap.x);

		controlSpec = argControlSpec.asSpec;
		(unitWidth>0).if{unitView.string = " "++controlSpec.units.asString};
		action = argAction;

		loBox.action_({ |box| this.lo_(box.value).doAction; });
		rangeSlider.action_({ |sl|
				this.lo_(controlSpec.map(sl.lo));
				this.hi_(controlSpec.map(sl.hi));
				this.doAction;
			});
		hiBox.action_({ |box| this.hi_(box.value).doAction; });

		if (initVal.notNil) { this.value_(initVal) };
		if (initAction) { this.doAction };


//		if (controlSpec.step != 0) {
//			rangeSlider.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
//		};

		numberStep = controlSpec.step;
		if (numberStep == 0) {
			numberStep = controlSpec.guessNumberStep;
		}{
			// controlSpec wants a step, so zooming in with alt is disabled.
			hiBox.alt_scale = 1.0;
			loBox.alt_scale = 1.0;
			rangeSlider.alt_scale = 1.0;
		};

		hiBox.step=numberStep;
		loBox.step=numberStep;
		hiBox.scroll_step=numberStep;
		loBox.scroll_step=numberStep;

		rangeSlider.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(View.currentDrag);
		};

		rangeSlider.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

	}

	doAction { action.value(this); }

	set { arg label, spec, argAction, initVal, initAction = false;
		labelView.notNil.if { labelView.string = label.asString };
		spec.notNil.if { controlSpec = spec.asSpec };
		argAction.notNil.if { action = argAction };

		initVal = initVal ? this.value ? controlSpec.default.dup(2);

		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};
	}

	value { ^[lo, hi] }
	value_ { |vals| this.lo_(vals[0]).hi_(vals[1]) }
	valueAction_ { |vals| this.value_(vals).doAction }

	lo_ { |val|
		lo = controlSpec.constrain(val);
		loBox.value_(lo.round(round));
		rangeSlider.lo_(controlSpec.unmap(lo));
	}

	hi_ { |val|
		hi = controlSpec.constrain(val);
		hiBox.value_(hi.round(round));
		rangeSlider.hi_(controlSpec.unmap(hi));
	}
	setColors{arg stringBackground, stringColor, sliderColor,  numBackground,numStringColor,
			 numNormalColor, numTypingColor, knobColor,background ;

			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};
				unitView.notNil.if{unitView.background_(stringBackground)};};
			stringColor.notNil.if{
				labelView.notNil.if{labelView.stringColor_(stringColor)};
				unitView.notNil.if{unitView.stringColor_(stringColor)};};
			sliderColor.notNil.if{
				rangeSlider.background_(sliderColor);};
			numBackground.notNil.if{
				hiBox.background_(numBackground);
				loBox.background_(numBackground);};
			numNormalColor.notNil.if{
				hiBox.normalColor_(numNormalColor);
				loBox.normalColor_(numNormalColor);};
			numTypingColor.notNil.if{
				hiBox.typingColor_(numTypingColor);
				loBox.typingColor_(numTypingColor);};
			numStringColor.notNil.if{
				hiBox.stringColor_(numStringColor);
				loBox.stringColor_(numStringColor);};
			knobColor.notNil.if{
				rangeSlider.knobColor_(knobColor);};
			background.notNil.if{
				view.background=background;};
			hiBox.refresh;
	}

	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			unitView.notNil.if{unitView.font=font};
			hiBox.font=font;
			loBox.font=font;
	}

}
