EZNumber : EZGui{
	var <numberView, <unitView, <>controlSpec,
		 numSize,numberWidth,unitWidth, gap2;
	var <>round = 0.001;

	*new { arg parent, bounds, label, controlSpec, action, initVal,
			initAction=false, labelWidth=60, numberWidth,
			unitWidth=0, labelHeight=20,  layout=\horz, gap, margin;

		^super.new.init(parent, bounds, label, controlSpec, action,
			initVal, initAction, labelWidth, numberWidth,
				unitWidth, labelHeight, layout, gap, margin)
	}

	init { arg parentView, bounds, label, argControlSpec, argAction, initVal,
			initAction, labelWidth, argNumberWidth,argUnitWidth,
			labelHeight, argLayout, argGap, argMargin;

		var innerBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);


		unitWidth = argUnitWidth;
		numberWidth = argNumberWidth;
		layout=argLayout;
		bounds.isNil.if {bounds= 160@20};

		// if no parent, then pop up window
		# view,bounds = this.prMakeView( parentView,bounds);

		numberWidth.isNil.if{numberWidth=45}{
			labelWidth=bounds.width-unitWidth-numberWidth; //override the labelWidth
			if (layout==\line2){unitWidth=bounds.width-numberWidth}; //override the unitWidth
		};
		labelSize=labelWidth@labelHeight;
		numSize = numberWidth@labelHeight;

		// insert the views

		label.notNil.if{ //only add a label if desired
				labelView = StaticText.new;
			if (layout==\line2)
				{labelView.align = \left;}
				{labelView.align = \right;};
			labelView.fixedWidth = labelWidth;
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = StaticText.new;
			unitView.fixedWidth = unitWidth;
		};

		// set view parameters and actions
		controlSpec = argControlSpec.asSpec;	// let default to nil.asSpec!
		(unitWidth>0).if{ unitView.string = " "++controlSpec.units.asString};
		initVal = initVal ? controlSpec.default;
		action = argAction;

		numberView = NumberBox.new;

		view.layout = if(argLayout == \horz) { HLayout } { VLayout }
		.new(*[labelView, unitView, numberView].reject(_.isNil))
		.margins_([margin.x, margin.y])
		.spacing_(gap.x);

		numberStep = controlSpec.step;
		if (numberStep == 0) {
			numberStep = controlSpec.guessNumberStep;
		}{
			// controlSpec wants a step, so zooming in with alt is disabled.
			numberView.alt_scale = 1.0
		};

		numberView.step = numberStep;
		numberView.scroll_step = numberStep;
		numberView.scroll=true;

		numberView.action = {
			this.valueActionIfChanged_(numberView.value);
		};

		if (initAction) {
			this.valueAction = initVal;
		}{
			this.value = initVal;
		};
	}

	value_{ arg val;
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
	}
	valueAction_ { arg val;
		this.value_(val);
		this.doAction;
	}
	doAction { action.value(this) }

	set { arg label, spec, argAction, initVal, initAction=false;
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

	setColors{ arg stringBackground, stringColor,numBackground,numStringColor,
			 numNormalColor, numTypingColor, background ;

		stringBackground.notNil.if{
			labelView.notNil.if{labelView.background_(stringBackground)};
			unitView.notNil.if{unitView.background_(stringBackground)};};
		stringColor.notNil.if{
			labelView.notNil.if{labelView.stringColor_(stringColor)};
			unitView.notNil.if{unitView.stringColor_(stringColor)};};
		numBackground.notNil.if{
			numberView.background_(numBackground);	};
		numNormalColor.notNil.if{
			numberView.normalColor_(numNormalColor);};

		numTypingColor.notNil.if{
			numberView.typingColor_(numTypingColor);};
		numStringColor.notNil.if{
			numberView.stringColor_(numStringColor);};
		background.notNil.if{
			view.background=background;};
	}


	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			unitView.notNil.if{unitView.font=font};
			numberView.font=font;
	}

}
