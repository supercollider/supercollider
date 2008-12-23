

EZSliderSC : EZGui {

	var  <>sliderView, <>numberView, <>controlSpec, <>action, <value, numSize;
	var <>round = 0.001;
	
	*new { arg parent, bounds, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=80, numberWidth = 80;
		^super.new.init(parent, bounds, label, controlSpec, action, initVal, 
			initAction, labelWidth, numberWidth);
	}
	init { arg parentView, bounds, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, numberWidth;
			
		var labelBounds, numBounds,w, winBounds, viewBounds, sliderBounds;
		
		// get parent gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		gap = decorator.tryPerform(\gap).tryPerform(\x);
		gap = gap ?  2;
		
		labelPosition.isNil.if{labelPosition = \left};
		
		// pop up window
		parentView.isNil.if{
				bounds.isNil.if{bounds= 160@44};
					//if its a point, then place the Window on the screen
				if (bounds.class==Point){
					winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
					bounds.x,bounds.y)
					}{winBounds=bounds};
				w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				// inset the bounds to make a nice margin
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true).resize_(5);
			}{
			bounds.isNil.if{bounds= 160@20};
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};
		
		labelSize=labelWidth@bounds.height;
		numSize=numberWidth@bounds.height;
		
		// calcualate bounds
		# labelBounds,numBounds,sliderBounds = this.prSubViewBounds(bounds, label.notNil);

		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view, labelBounds);
			labelView.string = label;
		};
		
		numberView = GUI.numberBox.new(view, numBounds);
		sliderView = GUI.slider.new(view, sliderBounds);
		this.labelPosition_(\left);
		
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		sliderView.action = {
			this.valueAction_(controlSpec.map(sliderView.value));
		};
		if (controlSpec.step != 0) {
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

	set { arg label, spec, argAction, initVal, initAction=false;
		labelView.string = label;
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
	
	visible { ^sliderView.visible }
	visible_ { |bool| [labelView, sliderView, numberView].do(_.visible_(bool)) }
	
	enabled {  ^sliderView.enabled } 
	enabled_ { |bool| [sliderView, numberView].do(_.enabled_(bool)) }
	
	
	bounds_{arg rect;
		var labelBounds,numBounds,sliderBounds;
		view.bounds=rect.asRect;
		# labelBounds,numBounds,sliderBounds = this.prSubViewBounds(view.bounds, labelView.notNil);
		
		labelView.notNil.if{labelView.bounds=labelBounds};
		sliderView.bounds=sliderBounds;
		numberView.bounds=numBounds;
	}
	
	labelPosition_{arg pos;
		labelPosition=pos;
		if (labelPosition==\top){
			numSize=numSize.x@20;
			labelView.notNil.if{
				labelSize=80@20;
				this.bounds_(view.bounds);
				labelView.align=\left; 
				labelView.resize_(2);
				};
			numberView.resize_(3);
			sliderView.resize_(2);
			this.bounds_(view.bounds);
			^this;
		};
		if (labelPosition==\left){
			labelView.notNil.if{labelView.align=\rigth; labelView.resize_(1)};
			numberView.resize_(3);
			sliderView.resize_(2);
			this.bounds_(view.bounds);
			^this;
		};
		if (labelPosition==\stack){
			numSize=view.bounds.width@20;
			labelView.notNil.if{
				labelSize=numSize;
				this.bounds_(view.bounds);
				labelView.align=\center; 
				labelView.resize_(1);
				};
			numberView.resize_(7);
			sliderView.resize_(4);
			this.bounds_(view.bounds);
			^this;
		};
		
	}
	
	prSubViewBounds{arg rect, hasLabel=true;
		var numBounds,labelBounds,sliderBounds,  tempGap, tempGap2;
		tempGap=gap;	
		tempGap2=tempGap;
		hasLabel.not.if{tempGap=0; labelSize=0@0};
		
		if (labelPosition==\top){
			sliderBounds= Rect(
					0,
					numSize.y+tempGap,
					view.bounds.width,  
					view.bounds.height-numSize.y-tempGap
					);
			hasLabel.if{numBounds=Rect(view.bounds.width-numSize.x,0, numSize.x, numSize.y)}
					{numBounds=Rect(0,0,view.bounds.width, numSize.y)};
					
			labelBounds=Rect(0,0,view.bounds.width-numSize.x-tempGap,labelSize.y);
		};
		if (labelPosition==\left){
			labelBounds=Rect(0,0, labelSize.x,view.bounds.height);
			numBounds=Rect(view.bounds.width-numSize.x,0, numSize.x,view.bounds.height);
			sliderBounds= Rect(
					labelSize.x+tempGap,
					0,
					view.bounds.width-labelSize.x-tempGap-numBounds.width-tempGap2,  
					view.bounds.height
					);
		};
		
		if (labelPosition==\stack){
			labelBounds=Rect(0,0, view.bounds.width,labelSize.y);
			numBounds=Rect(0,view.bounds.height-numSize.y, view.bounds.width,numSize.y);
			sliderBounds= Rect(
					0,
					labelSize.y+tempGap,
					view.bounds.width,  
					view.bounds.height-labelSize.y-tempGap-numBounds.height-tempGap2
					);
					
		};
		
		^[labelBounds, numBounds, sliderBounds]
	}
	
	label_{ arg string;
		// if no label view exists, add one
		labelView.isNil.if{
			labelSize=80@20;
			if(labelPosition==\top)
			 	{labelView = GUI.staticText.new(view, view.bounds.width@ labelSize.y);
				labelView.align = \left};
			if(labelPosition==\left)
				{ labelView = GUI.staticText.new(view, labelSize.x @ view.bounds.height);
				labelView.align = \right};
			if(labelPosition==\stack)
				{ labelSize=view.bounds.widht@numberView.bounds.height;
				 labelView = GUI.staticText.new(view, labelSize.x @ view.bounds.height);
				labelView.align = \center};
			labelView.string = string;
	 		this.bounds_(view.bounds); //recalculate bounds
		}{
		
	 	labelView.string_(string)
	 	};
	 	
	}
	
}
			