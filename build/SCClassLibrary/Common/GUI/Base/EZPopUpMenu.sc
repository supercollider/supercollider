EZPopUpMenu : EZlists{

	*new { arg parentView, bounds, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, labelHeight=20, labelPosition=\left, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelHeight,labelPosition, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelHeight, arglabelPosition,  argGap;
		var labelBounds, listBounds,w, winBounds;
		labelPosition=arglabelPosition;
		labelSize=labelWidth@labelHeight;
		gap=argGap;
		
		parentView.isNil.if{
				bounds.isNil.if{bounds= 160@44};
				if (bounds.class==Point){
					winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
					bounds.x,bounds.y)
					}{winBounds=bounds};
				w = GUI.window.new("",winBounds);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true).resize_(2);
			}{
			bounds.isNil.if{bounds= 160@20};
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};

		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		label.notNil.if{ //only add a label if desired
			if (labelPosition==\top){
				labelView = GUI.staticText.new(view, labelBounds).resize_(2);
				labelView.align = \left;
				}{
				labelView = GUI.staticText.new(view, labelBounds);
				labelView.align = \right;
				};
			labelView.string = label;
		};
				
		widget = GUI.popUpMenu.new(view, listBounds).resize_(5);
		
		this.items=argItems ? [];
		
		globalAction=argGlobalAction;
		
		widget.action={arg obj;
			items.at(obj.value).value.value(obj);
			globalAction.value(obj);
			};		
			
		this.value_(initVal);
			
		items.notNil.if{
			if(initAction){
					items.at(initVal).value.value(this); // You must do this like this
					globalAction.value(this);	// since listView's array is not accessible yet
				}
			{this.value_(initVal)};
		};
	}	
	
	menu {^ widget}
}


