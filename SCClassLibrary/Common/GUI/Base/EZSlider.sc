EZSlider : EZGui {

	var <sliderView, <numberView, <unitView, <>controlSpec;
	var numSize,numberWidth,unitWidth;
	var <>round = 0.001;

	*new { arg parent, bounds, label, controlSpec, action, initVal,
			initAction=false, labelWidth=60, numberWidth=45,
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

		sliderView = Slider.new;
		if(argLayout == \horz) {
			sliderView.orientation = \horizontal;
		} {
			sliderView.orientation = \vertical;
		};
		numberView = NumberBox.new;
		if(argLayout == \horz) {
			numberView.fixedWidth_(numSize.x)
		} {
			numberView.fixedHeight_(labelHeight);
		};

		view.layout = if(argLayout == \horz) { HLayout } { VLayout }
		.new(*[labelView, unitView, numberView, sliderView].reject(_.isNil))
		.margins_([margin.x, margin.y])
		.spacing_(gap.x);

		// set view parameters and actions

		controlSpec = argControlSpec.asSpec;
		controlSpec.addDependant(this);
		this.onClose = { controlSpec.removeDependant(this) };
		(unitWidth>0).if{unitView.string = " "++controlSpec.units.asString};
		initVal = initVal ? controlSpec.default;
		action = argAction;

		sliderView.action = {
			this.valueActionIfChanged_(controlSpec.map(sliderView.value));
		};


		sliderView.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(View.currentDrag);
		};

		sliderView.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

		numberView.action = { this.valueActionIfChanged_(numberView.value) };

		numberStep = controlSpec.step;
		if (numberStep == 0) {
			numberStep = controlSpec.guessNumberStep
		}{
			// controlSpec wants a step, so zooming in with alt is disabled.
			numberView.alt_scale = 1.0;
			sliderView.alt_scale = 1.0;
		};

		numberView.step = numberStep;
		numberView.scroll_step = numberStep;

		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};

		if (labelView.notNil) {
			labelView.mouseDownAction = {|view, x, y, modifiers, buttonNumber, clickCount|
				if(clickCount == 2, {this.editSpec});
			}
		};

		// this.prSetViewParams;

	}

	value_ { arg val;
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
		sliderView.value = controlSpec.unmap(value);
	}

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


	setColors{arg stringBackground,stringColor,sliderBackground,numBackground,
		numStringColor,numNormalColor,numTypingColor,knobColor,background;

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
			sliderBackground.notNil.if{
				sliderView.background_(sliderBackground);};
			knobColor.notNil.if{
				sliderView.knobColor_(knobColor);};
			background.notNil.if{
				view.background=background;};
			numberView.refresh;
	}

	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			unitView.notNil.if{unitView.font=font};
			numberView.font=font;
	}

	///////Private methods ///////

	update {arg changer, what ...moreArgs;
		var oldValue;
		if(changer === controlSpec, {
			oldValue = this.value;
			this.value = oldValue;
			if(this.value != oldValue, { this.doAction });
		});
	}

	editSpec {
		var ezspec, val;
		val = this.value;
		[labelView, sliderView, numberView, unitView].do({|view|
			view.notNil.if({ view.enabled_(false).visible_(false)});
		});
		ezspec = EZControlSpecEditor(view, view.bounds.moveTo(0,0), controlSpec: controlSpec.copy, layout: layout);
		ezspec.labelView.mouseDownAction = {|view, x, y, modifiers, buttonNumber, clickCount|
			if(clickCount == 2, {
				controlSpec = ezspec.controlSpec;
				ezspec.remove;
				[labelView, sliderView, numberView, unitView].do({|view|
					view.notNil.if({ view.enabled_(true).visible_(true)});
				});
				this.value = val;
			});
		};
	}

}
