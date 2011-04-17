EZNumber : EZGui{
	var <numberView, <unitView, <>controlSpec,
		 numSize,numberWidth,unitWidth, gap, gap2;
	var <>round = 0.001;

	var scaler=1;  //for swing compatibility
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

		var labelBounds, numBounds, unitBounds;
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

		// calcualate bounds
		# labelBounds,numBounds, unitBounds
				= this.prSubViewBounds(innerBounds, label.notNil, unitWidth>0);

		// insert the views

		label.notNil.if{ //only add a label if desired
				labelView = GUI.staticText.new(view, labelBounds);
			if (layout==\line2)
				{labelView.align = \left;}
				{labelView.align = \right;};
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = GUI.staticText.new(view, unitBounds);
		};

		// set view parameters and actions
		controlSpec = argControlSpec.asSpec;	// let default to nil.asSpec!
		(unitWidth>0).if{ unitView.string = " "++controlSpec.units.asString};
		initVal = initVal ? controlSpec.default;
		action = argAction;

		numberView = GUI.numberBox.new(view, numBounds).resize_(2);

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
			this.valueAction_(numberView.value);
		};

		if (initAction) {
			this.valueAction = initVal;
		}{
			this.value = initVal;
		};

		this.prSetViewParams;
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

	/////// Private methods /////////

	prSetViewParams{ // sets resize and alignment for different layouts

		switch (layout,
		\line2, {
			labelView.notNil.if{labelView.resize_(2)};
			unitView.notNil.if{unitView.resize_(6)};
			numberView.resize_(5);
		},
		\horz, {
			labelView.notNil.if{
				labelView.resize_(4).align_(\right);
				numberView.resize_(5);
				unitView.notNil.if{unitView.resize_(6)};
			}{
				unitView.notNil.if{	unitView.resize_(6)};
				numberView.resize_(5);
			};
		});
			popUp.if{view.resize_(2)};
	}

	prSubViewBounds{arg rect, hasLabel, hasUnit;
		var numBounds,labelBounds,sliderBounds;
		var unitBounds, gap1, gap2, numY;
		gap1 = gap.copy;
		gap2 = gap.copy;
		hasLabel.not.if{ gap1 = 0@0; labelSize=0@0};
		hasUnit.not.if{gap2 = 0@0};

		switch (layout,
			\line2, {

				labelBounds = Rect( // fill the line
						0,
						0,
						rect.width,
						labelSize.y;
						);

				numSize.y=numSize.y-gap1.y;
				numY=labelBounds.height+gap1.y;
				unitBounds = Rect( rect.width - unitWidth, // //adjust to fit
					numY, unitWidth, rect.height-labelSize.y-gap1.y);
				numBounds = Rect(0, numY,
					rect.width-unitWidth-gap2.x, rect.height-labelSize.y-gap1.y); // to right

				},

			 \horz, {
				labelSize.y=rect.height;
				labelBounds = (labelSize.x@labelSize.y).asRect; // to left
				unitBounds = (unitWidth@labelSize.y).asRect.left_(rect.width-unitWidth); // to right
				numBounds  =  Rect( //adjust to fit
					labelBounds.width+gap1.x,
					0,
					rect.width - labelBounds.width - unitBounds.width - gap1.x - gap2.x ,
					labelBounds.height
					);
		});


		^[labelBounds, numBounds, unitBounds].collect{arg v; v.moveBy(margin.x,margin.y)}
	}


}
