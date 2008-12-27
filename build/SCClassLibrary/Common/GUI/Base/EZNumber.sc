
EZNumberSC : EZGui{
	var   <>controlSpec, <>action, <value;
	var   <>round = 0.001;
	
	*new { arg parent, bounds, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=80, numberWidth;
		^super.new.init(parent, bounds, label, controlSpec, action, initVal, 
			initAction, labelWidth, numberWidth);
	}
	
	init { arg parentView, bounds, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, numberWidth;
			
		var labelBounds, numBounds,w, winBounds, viewBounds, sliderbounds;
		
		// get parent gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		gap = decorator.tryPerform(\gap).tryPerform(\x);
		gap = gap ?  2;
		
		labelPosition.isNil.if{labelPosition = \left};
		
		if( labelPosition==\top){
			numberWidth = numberWidth ? 80;
		};
		
		// pop up window
		parentView.isNil.if{
				// if bounds is a point the place the window on screen
				bounds.isNil.if {bounds= 160@44};
				if (bounds.class==Point)
					{ bounds = bounds.x@max(bounds.y,bounds.y+24);// window minimum height;
					 winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
								bounds.x,bounds.y)
					}{// window minimum height;
					winBounds = bounds.height_(max(bounds.height,bounds.height+24))
					};
				w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				// If numberWidth is set, then adjust the parent view size  both subviews
				numberWidth.notNil.if{ bounds=bounds.width_(labelWidth+gap+numberWidth)};
				// inset the bounds to make a nice margin
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true).resize_(2);
			}{
			bounds.isNil.if{bounds= 160@20};
			bounds=bounds.asRect;
				// If numberWidth is set, then adjust the parent view size  both subviews
			numberWidth.notNil.if{ bounds=bounds.width_(labelWidth+gap+numberWidth)};
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};
		
		labelSize=labelWidth@bounds.height;
		
		// calcualate bounds
		# labelBounds,numBounds = this.prSubViewBounds(bounds, label.notNil);

		label.notNil.if{ //only add a label if desired
				labelView = GUI.staticText.new(view, labelBounds);
			if (labelPosition==\top)
				{labelView.align = \left;}
				{labelView.align = \right;};
			labelView.string = label;
		};
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		widget = GUI.numberBox.new(view, numBounds).resize_(2);
		widget.action = {
			this.valueAction_(widget.value);
		};
		
		if (initAction) {
			this.valueAction = initVal;
		}{
			this.value = initVal;
		};
	}
	numberView{^widget}
	
	value_{ arg val; 
		value = controlSpec.constrain(val);
		widget.value = value.round(round);
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
			widget.value = value.round(round);
		};
	}
		
	visible { ^widget.visible }
	visible_ { |bool| [labelView, widget].do(_.visible_(bool)) }
	
	enabled {  ^widget.enabled } 
	enabled_ { |bool| widget.enabled_(bool) }
	
	remove { [labelView, widget].do(_.remove) }
	
	prSubViewBounds{arg rect, hasLabel=true;
		var numBounds,labelBounds, tempGap,tmp;
		
		tempGap=gap;	
		hasLabel.not.if{tempGap=0; labelSize=0@0};

		if (labelPosition==\top)
			{ numBounds= Rect(
					0,
					labelSize.y+tempGap,
					view.bounds.width,  
					view.bounds.height-labelSize.y-tempGap
					);
			if (view.parent.respondsTo(\findWindow)){
			 tmp = view.parent.findWindow.bounds;
			 view.parent.findWindow.bounds = tmp.height_(max(tmp.height,62+gap));
			 numBounds = numBounds.height_(max(numBounds.height,16));
			};
			labelBounds=Rect(0,0,numBounds.width,labelSize.y);}
			{ numBounds= Rect(
					labelSize.x+tempGap,
					0,
					view.bounds.width-labelSize.x-tempGap,  
					view.bounds.height
					);
					
			labelBounds=Rect(0,0, labelSize.x ,numBounds.height )};
		
		^[labelBounds, numBounds]
	}
	

}

