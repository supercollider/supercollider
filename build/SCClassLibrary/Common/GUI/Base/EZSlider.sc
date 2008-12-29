
EZSliderSC : EZGui {

	var <sliderView, <numberView, <unitView, <>controlSpec, 
		 <>action,<value, popUp=false, numSize,numberWidth,unitWidth, gap;
	var <>round = 0.001;
	
	*new { arg parent, bounds, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=60, numberWidth=40, 
			unitWidth=0, labelHeight=20,  layout=\horz, gap;
			
		^super.new.init(parent, bounds, label, controlSpec, action, 
			initVal, initAction, labelWidth, numberWidth, 
				unitWidth, labelHeight, layout, gap)
	}
	
	init { arg parentView, bounds, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, argNumberWidth,argUnitWidth, 
			labelHeight, argLayout, argGap;
			
		var labelBounds, numBounds,w, winBounds, 
				viewBounds, unitBounds,sliderBounds;
				
		// try to use the parent decorator gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		argGap.isNil.if{ 
			gap = decorator.tryPerform(\gap);
			gap = gap ? (2@2)}
			{gap=argGap};
			
		unitWidth = argUnitWidth;
		numberWidth = argNumberWidth;
		layout=argLayout;
		
		// if no parent, then pop up window 
		parentView.isNil.if{
			popUp=true;
			bounds.isNil.if{bounds = 350@20};
				//if its a point, then place the Window on the screen
			if (bounds.class == Point){
				bounds = bounds.x@max(bounds.y,bounds.y+24);// window minimum height;
				winBounds = Rect(200, Window.screenBounds.height-bounds.y-100,
				bounds.x,bounds.y)
				}{// window minimum height;
				winBounds = bounds.height_(max(bounds.height,bounds.height+24))
				};
			w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
			parentView = w.asView;
			w.front;
			bounds = bounds.asRect;
			// inset the bounds to make a nice margin
			bounds = Rect(4,4,bounds.width-8,bounds.height-24);
			view = GUI.compositeView.new(parentView,bounds)
					.relativeOrigin_(true).resize_(5);
					
		// normal parent view			
		}{ 
			bounds.isNil.if{bounds = 160@20};
			bounds = bounds.asRect;
			view = GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};
		
		labelSize=labelWidth@labelHeight;
		numSize = numberWidth@labelHeight;
		
		// calculate bounds of all subviews
		# labelBounds,numBounds,sliderBounds, unitBounds 
				= this.prSubViewBounds(bounds, label.notNil, unitWidth>0);
		
		// instert the views
		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view, labelBounds);
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = GUI.staticText.new(view, unitBounds);
		};

		numberView = GUI.numberBox.new(view, numBounds);
		sliderView = GUI.slider.new(view, sliderBounds);
		
		
		// set view parameters and actions
		
		controlSpec = argControlSpec.asSpec;
		(unitWidth>0).if{unitView.string = " "++controlSpec.units.asString};
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		sliderView.action = {
			this.valueAction_(controlSpec.map(sliderView.value));
		};
		
		if (controlSpec.step == 0) {
			sliderView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		sliderView.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(GUI.view.currentDrag);
		};
		
		sliderView.beginDragAction = { arg slider;
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
		labelView.notNil.if{labelView.string=label};
		controlSpec = spec.asSpec;
		action = argAction;
		initVal = initVal ? controlSpec.default;
		if (initAction) {
			this.value = initVal;
		}{
			value = initVal;
			sliderView.value = controlSpec.unmap(value);
			numberView.value = value.round(round);
		};
	}
		
	prSetViewParams{ // sets resize and alignment for different layouts
		
		switch (layout,
		\line2, {
			labelView.notNil.if{
				labelView.resize_(2);
				unitView.notNil.if{unitView.resize_(3)};
				numberView.resize_(3);
				}{
				unitView.notNil.if{
					unitView.resize_(2);
					numberView.resize_(1);
					}{
					numberView.resize_(2);
					};
				};
			sliderView.resize_(5);
			popUp.if{view.resize_(2)};
		},
		\vert, {
			labelView.notNil.if{labelView.resize_(2)};
			unitView.notNil.if{unitView.resize_(8)};
			numberView.resize_(8);
			sliderView.resize_(5);
			popUp.if{view.resize_(4)};
		},
		\horz, {
			labelView.notNil.if{labelView.resize_(4).align_(\right)};
			unitView.notNil.if{unitView.resize_(6)};
			numberView.resize_(6);
			sliderView.resize_(5);
			popUp.if{view.resize_(2)};
		});
	
	}
	
	prSubViewBounds{arg rect, hasLabel, hasUnit;  // calculate subview bounds
		var numBounds,labelBounds,sliderBounds, unitBounds;
		var gap1, gap2, gap3, tmp, labelH, unitH;
		gap1 = gap;	
		gap2 = gap1;
		gap3 = gap1;
		labelH=labelSize.y;//  needed for \vert
		unitH=labelSize.y; //  needed for \vert
		hasLabel.not.if{ gap2 = 0@0; labelSize.x = 0 ;};
		hasUnit.not.if{ gap3 = 0@0; unitWidth = 0};
		
		switch (layout,
			\line2, {
			
				hasLabel.if{ // with label
					unitBounds = (unitWidth@labelSize.y)
						.asRect.left_(rect.width-unitWidth);// view to right
					numBounds = (numSize.x@labelSize.y)
						.asRect.left_(rect.width-unitBounds.width-numberWidth-gap3.x); // view to right
					labelBounds = (labelSize.x@labelSize.y)
						.asRect.width_(numBounds.left-gap2.x); //adjust width
				}{ // no label
				labelBounds = (0@labelSize.y).asRect; //just a dummy
				numBounds = (numberWidth@labelSize.y).asRect; //view to left
				(unitWidth>0).if{
					unitBounds = Rect (numBounds.width+gap3.x, 0,
						rect.width-numBounds.width-gap3.x,labelSize.y); //adjust to fit
						}{
					unitBounds = Rect (0, 0,0,0); //no unitView
						numBounds = (rect.width@labelSize.y).asRect; //view to left
						};
						
				};
				sliderBounds = Rect( //adjust to fit
						0,
						labelSize.y+gap1.y,
						rect.width, 
						rect.height-numSize.y-gap1.y;
						);
				},
			
			 \vert, { 
				hasLabel.not.if{labelH=0};
				labelBounds = (rect.width@labelH).asRect; // to top
				hasUnit.not.if{unitH=0};
				unitBounds = (rect.width@unitH)
					.asRect.top_(rect.height-labelSize.y); // to bottom
				numBounds = (rect.width@labelSize.y)
					.asRect.top_(rect.height-unitBounds.height-numSize.y-gap3.y); // to bottom
				
				sliderBounds = Rect( //adjust to fit
					0,
					labelBounds.height+gap1.y, 
					rect.width,
					rect.height - labelBounds.height - unitBounds.height 
							- numBounds.height - gap1.y - gap2.y - gap3.y
					);
				},
				
			 \horz, {
				labelSize.y=view.bounds.height;
				labelBounds = (labelSize.x@labelSize.y).asRect; //to left
				unitBounds = (unitWidth@labelSize.y).asRect.left_(rect.width-unitWidth); // to right 
				numBounds = (numSize.x@labelSize.y).asRect
					.left_(rect.width-unitBounds.width-numSize.x-gap3.x);// to right
				sliderBounds  =  Rect( // adjust to fit
					labelBounds.width+gap1.x,
					0,
					rect.width - labelBounds.width - unitBounds.width 
							- numBounds.width - gap1.x - gap2.x - gap3.x, 
					labelBounds.height
					);
		});
		
		
		^[labelBounds, numBounds, sliderBounds, unitBounds]
	}
	
	setColors{arg stringBackground, strColor, sliderColor,  boxColor,boxStringColor,
			 boxNormalColor, boxTypingColor, knobColor,background ;
			
			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};
				unitView.notNil.if{unitView.background_(stringBackground)};};
			strColor.notNil.if{	
				labelView.notNil.if{labelView.stringColor_(strColor)};
				unitView.notNil.if{unitView.stringColor_(strColor)};};
			boxColor.notNil.if{		
				numberView.boxColor_(boxColor);	};
			boxNormalColor.notNil.if{	
				numberView.normalColor_(boxNormalColor);};
			boxTypingColor.notNil.if{	
				numberView.typingColor_(boxTypingColor);};
			boxStringColor.notNil.if{	
				numberView.stringColor_(boxStringColor);};
			sliderColor.notNil.if{	
				sliderView.background_(sliderColor);};
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
	
	
}
			