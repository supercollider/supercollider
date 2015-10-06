EZKnob : EZGui {

	classvar <>compactRatio=0.87;

	var <knobView, <numberView, <unitView, <>controlSpec, knobSize, unitWidth;
	var numSize,numberWidth,unitWidth;
	var <>round = 0.001;

	*new { arg parent, bounds, label, controlSpec, action, initVal,
			initAction=false, labelWidth=60, knobSize,
			unitWidth=0, labelHeight=20,  layout=\vert, gap, margin;

		^super.new.init(parent, bounds, label, controlSpec, action,
			initVal, initAction, labelWidth, knobSize,
				unitWidth, labelHeight, layout, gap, margin)
	}

	init { arg parentView, bounds, label, argControlSpec, argAction, initVal,
			initAction, labelWidth, argKnobSize,argUnitWidth,
			labelHeight, argLayout, argGap, argMargin;

		var innerBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		unitWidth = argUnitWidth;
		layout=argLayout;
		bounds.isNil.if{bounds = 50@90};

		knobSize = argKnobSize ;

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

		// instert the views
		label.notNil.if{ //only add a label if desired
			labelView = StaticText.new;
			labelView.fixedWidth = labelWidth;
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = StaticText.new;
			unitView.fixedWidth = unitWidth;
		};

		knobView = Knob.new;
		numberView = NumberBox.new;
		if(argLayout == \horz) {
			numberView.fixedWidth_(numSize.x)
		} {
			numberView.fixedHeight_(labelHeight);
		};

		view.layout = if(argLayout == \horz) { HLayout } { VLayout }
		.new(*[labelView, unitView, numberView, knobView].reject(_.isNil))
		.margins_([margin.x, margin.y])
		.spacing_(gap.x);

		// set view parameters and actions
		controlSpec = argControlSpec.asSpec;
		(unitWidth>0).if{unitView.string = " "++controlSpec.units.asString};
		initVal = initVal ? controlSpec.default;
		action = argAction;

		numberStep = controlSpec.step;
		if (numberStep == 0) {
			numberStep = controlSpec.guessNumberStep
		}{
			// controlSpec wants a step, so zooming in with alt is disabled.
			numberView.alt_scale = 1.0;
			knobView.alt_scale = 1.0;
		};
		numberView.step = numberStep;
		numberView.scroll_step = numberStep;


		if((controlSpec.minval + controlSpec.maxval)==0){knobView.centered=true};

		knobView.action = {
			this.valueActionIfChanged_(controlSpec.map(knobView.value));
		};

		if (controlSpec.step != 0) {
			knobView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		knobView.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(View.currentDrag);
		};

		knobView.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

		numberView.action = { this.valueActionIfChanged_(numberView.value) };

		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};

	}
		// just set, no action
	value_ { arg val;
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
		knobView.value = controlSpec.unmap(value);
	}
		// set and do action
	valueAction_ { arg val;
		this.value_(val);
		this.doAction;
	}

	doAction { action.value(this) }

	set { arg label, spec, argAction, initVal, initAction = false;
		labelView.notNil.if { labelView.string = label.asString };
		spec.notNil.if { controlSpec = spec.asSpec };
		argAction.notNil.if { action = argAction };
		initVal = initVal ? value ? controlSpec.default;

		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};
	}

	centered_ { arg bool; knobView.centered_(bool) }

	centered{ ^knobView.centered }


	setColors{arg stringBackground,stringColor,numBackground,
		numStringColor,numNormalColor,numTypingColor,knobColors,background;

			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};
				unitView.notNil.if{unitView.background_(stringBackground)};};
			stringColor.notNil.if{
				labelView.notNil.if{labelView.stringColor_(stringColor)};
				unitView.notNil.if{unitView.stringColor_(stringColor)};};
			numBackground.notNil.if{
				numberView.background_(numBackground);};
			numNormalColor.notNil.if{
				numberView.normalColor_(numNormalColor);};
			numTypingColor.notNil.if{
				numberView.typingColor_(numTypingColor);};
			numStringColor.notNil.if{
				numberView.stringColor_(numStringColor);};
			knobColors.notNil.if{
				knobView.color_(knobColors);};
			background.notNil.if{
				view.background=background;};
			numberView.refresh;
			knobView.refresh;
	}

	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			unitView.notNil.if{unitView.font=font};
			numberView.font=font;
	}

}
