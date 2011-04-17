EZKnob : EZGui {

	classvar <>compactRatio=0.87;

	var <knobView, <numberView, <unitView, <>controlSpec,
		  popUp=false, knobSize,unitWidth, gap;
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

		var labelBounds, numBounds, unitBounds,knobBounds;
		var numberStep;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		unitWidth = argUnitWidth;
		layout=argLayout;
		bounds.isNil.if{bounds = 50@90};

		knobSize = argKnobSize ;

		// if no parent, then pop up window
		# view,bounds = this.prMakeView( parentView,bounds);


		labelSize=labelWidth@labelHeight;

		// calculate bounds of all subviews
		# labelBounds,numBounds,knobBounds, unitBounds
				= this.prSubViewBounds(innerBounds, label.notNil, unitWidth>0);

		// instert the views
		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view, labelBounds);
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = GUI.staticText.new(view, unitBounds);
		};

		knobView = GUI.knob.new(view, knobBounds);
		numberView = GUI.numberBox.new(view, numBounds);

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
			this.valueAction_(controlSpec.map(knobView.value));
		};

		if (controlSpec.step != 0) {
			knobView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		knobView.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(GUI.view.currentDrag);
		};

		knobView.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

		numberView.action = { this.valueAction_(numberView.value) };

		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};

		this.prSetViewParams;
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

	///////Private methods ///////

	prSetViewParams{ // sets resize and alignment for different layouts

		switch (layout,
		\line2, {
			labelView.notNil.if{
				labelView.resize_(5);
				unitView.notNil.if{unitView.resize_(9)};
				numberView.resize_(8);
				}{
				unitView.notNil.if{
					unitView.resize_(6);
					numberView.resize_(5);
					}{
					numberView.resize_(5);
					};
				};
			knobView.resize_(9);
			popUp.if{view.resize_(2)};
		},
		\vert, {
			labelView.notNil.if{labelView.resize_(2)};
			unitView.notNil.if{unitView.resize_(6)};
			numberView.resize_(5);
			popUp.if{view.resize_(2)};
		},
		\vert2, {
			labelView.notNil.if{labelView.resize_(2).align_(\center)};
			unitView.notNil.if{unitView.resize_(6)};
			numberView.resize_(5);
			popUp.if{view.resize_(2)};
		},
		\horz, {
			labelView.notNil.if{labelView.resize_(4).align_(\right)};
			unitView.notNil.if{unitView.resize_(6)};
			numberView.resize_(5);
			knobView.resize_(9);
			popUp.if{view.resize_(2)};
		});

	}

	prSubViewBounds{arg rect, hasLabel, hasUnit;  // calculate subview bounds
		var numBounds,labelBounds,knobBounds, unitBounds,knobHeight, numHeight;
		var gap1, gap2, gap3, tmp, labelH, unitH;
		gap1 = gap.copy;
		gap2 = gap.copy;
		gap3 = gap.copy;
		(rect.height<= (labelSize.y*2)).if{labelSize.y=rect.height/2};

		numHeight=labelSize.y;

		switch (layout,
			 \line2, {
				knobSize.isNil.if{knobSize=( ((rect.height/compactRatio)-margin.x)@(rect.height))};
				knobSize=(knobSize.x-margin.x)@(knobSize.y.min(rect.height));
				hasUnit.not.if{ gap2 = 0@0; unitWidth = 0};
				labelBounds = Rect(0,0,rect.width-knobSize.x-gap3.x,labelSize.y); //to left
				hasLabel.if{
					unitBounds = Rect(labelBounds.width-unitWidth,labelSize.y+gap1.y,
						unitWidth, rect.height-labelSize.y-gap1.y);

					numBounds = Rect(0,labelSize.y+gap1.y,
						labelBounds.width-gap2.x-unitBounds.width,rect.height-labelSize.y-gap1.y);

				}{
					unitBounds = Rect(labelBounds.width-unitWidth,0,
							unitWidth, rect.height);

					numBounds = Rect(0,0,
						labelBounds.width-gap2.x-unitBounds.width,rect.height);

				};
				knobBounds=knobSize.asRect.moveTo(rect.width-knobSize.x,0);
			},
			 \horz, {
				knobSize.isNil.if{knobSize=( ((rect.height/compactRatio)-margin.x)@(rect.height))};
				knobSize=(knobSize.x-margin.x)@(knobSize.y.min(rect.height));
				knobBounds=knobSize.asRect.moveTo(rect.width-knobSize.x,0);
				hasUnit.not.if{ gap2 = 0@0; unitWidth = 0};
				hasLabel.not.if{ gap1 = 0@0; labelSize.x = 0};
			 	labelBounds = (labelSize.x@rect.height).asRect;
			 	unitBounds = (unitWidth@rect.height).asRect
			 		.moveTo(rect.width-knobBounds.width-gap2.x-unitWidth,0);
			 	numBounds = Rect.newSides(labelSize.x+gap1.x,0,unitBounds.left-gap2.x,rect.height);
			 },
			 \vert , {
				hasUnit.not.if{ gap3 = 0@0; unitWidth = 0};
				hasLabel.not.if{ gap1 = 0@0; labelSize.y = 0};

				knobSize.isNil.if{
					knobSize=( ((rect.height-labelSize.y-numHeight-gap1.y-gap2.y)/compactRatio)
						@(rect.height-labelSize.y-numHeight-gap1.y-gap2.y))
				};
				knobSize=((knobSize.x).min(rect.width))
					@(knobSize.y.min(rect.height-labelSize.y-numHeight-gap1.y-gap2.y));

			 	labelBounds = (rect.width@labelSize.y).asRect;
				knobBounds=knobSize.asRect.moveTo(0,labelSize.y+gap1.y);

			 	numBounds = Rect(0,rect.height-numHeight,rect.width-unitWidth-gap3.x, numHeight);

			 	unitBounds = Rect(rect.width-unitWidth,rect.height-numHeight,unitWidth,numHeight);

			 },
			 \vert2 , {
				hasUnit.not.if{ gap3 = 0@0; unitWidth = 0};
				hasLabel.not.if{ gap1 = 0@0; labelSize.y = 0};

				knobSize.isNil.if{
					knobSize=( ((rect.height-labelSize.y-numHeight-gap1.y-gap2.y)/compactRatio)
						@(rect.height-labelSize.y-numHeight-gap1.y-gap2.y))
				};
				knobSize=((knobSize.x).min(rect.width))
					@(knobSize.y.min(rect.height-labelSize.y-numHeight-gap1.y-gap2.y));


			 	labelBounds = (rect.width@labelSize.y).asRect;
				knobBounds=knobSize.asRect.moveTo(0,labelSize.y+gap1.y);
				knobBounds=knobBounds.moveBy((rect.width-knobBounds.width)/2,0);
			 	numBounds = Rect(0,rect.height-numHeight,rect.width-unitWidth-gap3.x, numHeight);

			 	unitBounds = Rect(rect.width-unitWidth,rect.height-numHeight,unitWidth,numHeight);

			 }
		);
		((knobBounds.height<0)||(knobBounds.width<0)).if{knobBounds=knobBounds.height_(0).width_(0)};

		^[labelBounds, numBounds, knobBounds, unitBounds].collect{arg v; v.moveBy(margin.x,margin.y)}
	}

}
