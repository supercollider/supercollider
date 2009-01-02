
EZRanger : EZGui {

	var <rangeSlider, <hiBox,<loBox, <unitView, <>controlSpec, 
		 <>action,<lo, <hi, popUp=false, numSize,numberWidth,unitWidth, gap;
	var <>round = 0.001;
	
	*new { arg parent, bounds, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=60, numberWidth=45, 
			unitWidth=0, labelHeight=20,  layout=\horz, gap;
			
		^super.new.init(parent, bounds, label, controlSpec, action, 
			initVal, initAction, labelWidth, numberWidth, 
				unitWidth, labelHeight, layout, gap)
	}
	
	init { arg parentView, bounds, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, argNumberWidth,argUnitWidth, 
			labelHeight, argLayout, argGap;
			
		var labelBounds, hiBounds,loBounds, unitBounds,rangerBounds;
				
		// try to use the parent decorator gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		argGap.isNil.if{ 
			gap = decorator.tryPerform(\gap);
			gap = gap ? (2@2)}
			{gap=argGap};
			
		unitWidth = argUnitWidth;
		numberWidth = argNumberWidth;
		layout=argLayout;
		
		bounds.isNil.if{bounds = 350@20};
		
		// if no parent, then pop up window 
		# view,bounds = this.prMakeView( parentView,bounds);	
		
		labelSize=labelWidth@labelHeight;
		numSize = numberWidth@labelHeight;
		
		// calcualate bounds
		# labelBounds,hiBounds,loBounds,rangerBounds, unitBounds 
				= this.prSubViewBounds(bounds, label.notNil, unitWidth>0);

		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view, labelBounds);
			labelView.string = label;
		};

		(unitWidth>0).if{ //only add a unitLabel if desired
			unitView = GUI.staticText.new(view, unitBounds);
		};


		hiBox = GUI.numberBox.new(view, hiBounds);
		loBox = GUI.numberBox.new(view, loBounds);
		rangeSlider = GUI.rangeSlider.new(view, rangerBounds);
		
		
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
		unitView.notNil.if{unitView.string_(controlSpec.units ? "")};
		
		if (initVal.notNil) { this.value_(initVal) };
		if (initAction) { this.doAction };
		
		
		if (controlSpec.step == 0) {
			rangeSlider.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		rangeSlider.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(GUI.view.currentDrag);
		};
		
		rangeSlider.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

		
		this.prSetViewParams;
		
	}
	
	doAction { action.value(this); } 
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
	
		
	prSetViewParams{
	
		switch (layout,
		\line2, {
			labelView.notNil.if{
				labelView.resize_(2);
				unitView.notNil.if{unitView.resize_(3)};
				hiBox.resize_(3);
				loBox.resize_(3);
				}{
				unitView.notNil.if{
					unitView.resize_(2).align_(\left)};
				hiBox.resize_(1);
				loBox.resize_(1);
				};
			rangeSlider.resize_(5);
			popUp.if{view.resize_(2)};
		},
		\vert, {
			labelView.notNil.if{labelView.resize_(2)};
			unitView.notNil.if{unitView.resize_(8)};
			loBox.resize_(8);
			hiBox.resize_(2);
			rangeSlider.resize_(5);
			popUp.if{view.resize_(4)};
		},
		\horz, {
			labelView.notNil.if{labelView.resize_(4).align_(\right)};
			unitView.notNil.if{unitView.resize_(6)};
			loBox.resize_(4);
			hiBox.resize_(6);
			rangeSlider.resize_(5);
			popUp.if{view.resize_(2)};
		});
	}
	prSubViewBounds{arg rect, hasLabel, hasUnit;
		var hiBounds,loBounds,labelBounds,rangerBounds;
		var unitBounds, gap1, gap2, gap3,gap4, tmp, labelH, unitH;
		gap1 = gap;	
		gap2 = gap;
		gap3 = gap;
		gap4 = gap;
		labelH=labelSize.y;//  needed for \vert
		unitH=labelSize.y; //  needed for \vert
		hasLabel.not.if{ gap2 = 0@0; labelSize.x = 0 ;};
		hasUnit.not.if{ gap4 = 0@0; unitWidth = 0};
		
		switch (layout,
			\line2, {
			
				hasLabel.if{ // with label
					unitBounds = (unitWidth@labelSize.y)
						.asRect.left_(rect.width-unitWidth);// view to right
					hiBounds = (numSize.x@labelSize.y)
						.asRect.left_(rect.width-unitBounds.width-numberWidth-gap4.x); // view to right
					loBounds = (numSize.x@labelSize.y)
						.asRect.left_(rect.width-unitBounds.width-numberWidth
							-gap3.x-hiBounds.width-gap4.x); // view to right
					labelBounds = (labelSize.x@labelSize.y)
						.asRect.width_(hiBounds.left-gap2.x); //adjust width
				}{ // no label
				labelBounds = (0@labelSize.y).asRect; //just a dummy
				loBounds = (numberWidth@labelSize.y).asRect; //view to left
				hiBounds = (numberWidth@labelSize.y).asRect.moveTo(loBounds.width+gap2.x,0); //view to left
				(unitWidth>0).if{
					unitBounds = Rect.newSides (hiBounds.right+gap4.x,
					0,rect.width, labelSize.y); //adjust width
						}{
					unitBounds = Rect (0, 0,0,0); //no unitView
						};
						
				};
				rangerBounds = Rect(
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
				hiBounds = (rect.width@labelSize.y)
					.asRect.top_(labelBounds.height-gap1.y); // to bottom
				loBounds = (rect.width@labelSize.y)
					.asRect.top_(rect.height-unitBounds.height-numSize.y-gap4.y); // to bottom
				rangerBounds = Rect.newSides(
					0,
					hiBounds.bottom+gap2.y, 
					rect.width,
					loBounds.top-gap3.y-gap4.y
					);
				},
				
			 \horz, {
				labelSize.y=view.bounds.height;
				labelBounds = (labelSize.x@labelSize.y).asRect;
				unitBounds = (unitWidth@labelSize.y).asRect.left_(rect.width-unitWidth);
				loBounds = (numSize.x@labelSize.y).asRect
					.left_(labelBounds.width+gap1.x);
				hiBounds = (numSize.x@labelSize.y).asRect
					.left_(rect.width-unitBounds.width-numSize.x-gap4.x);
				rangerBounds  =  Rect.newSides(
					loBounds.right+gap2.x , 0, 
					rect.width-hiBounds.width-unitBounds.width
						-gap3.x-gap4.x, 
					labelSize.y);
				
			});
		
		
		^[labelBounds, hiBounds, loBounds, rangerBounds, unitBounds]
	}
	
	setColors{arg stringBackground, strColor, sliderColor,  boxColor,boxStringColor,
			 boxNormalColor, boxTypingColor, knobColor,background ;
			
			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};
				unitView.notNil.if{unitView.background_(stringBackground)};};
			strColor.notNil.if{	
				labelView.notNil.if{labelView.stringColor_(strColor)};
				unitView.notNil.if{unitView.stringColor_(strColor)};};
			sliderColor.notNil.if{	
				rangeSlider.background_(sliderColor);};
			boxColor.notNil.if{		
				hiBox.boxColor_(boxColor);
				loBox.boxColor_(boxColor);	};
			boxNormalColor.notNil.if{	
				hiBox.normalColor_(boxNormalColor);
				loBox.normalColor_(boxNormalColor);};
			boxTypingColor.notNil.if{	
				hiBox.typingColor_(boxTypingColor);
				loBox.typingColor_(boxTypingColor);};
			boxStringColor.notNil.if{	
				hiBox.stringColor_(boxStringColor);
				loBox.stringColor_(boxStringColor);};
			knobColor.notNil.if{	
				rangeSlider.setProperty(\knobColor,knobColor);};
			background.notNil.if{	
				view.background=background;};
			hiBox.refresh;
	}
	
	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			unitView.notNil.if{unitView.font=font};
			hiBox.font=font;
	}
	
	
}
			