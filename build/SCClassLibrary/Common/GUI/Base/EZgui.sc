EZGui{ // an abstract class
	var <>labelView, widget, <view, <gap, popUp=false,  <layout, labelSize, <alwaysOnTop=false;
			
	visible { ^view.getProperty(\visible) }
	visible_ { |bool|  view.setProperty(\visible,bool)  }
	
	enabled {  ^view.enabled } 
	enabled_ { |bool| view.enabled_(bool) }
	
	remove { view.remove}
	
	
	alwaysOnTop_{arg bool;
		alwaysOnTop=bool;
		this.view.parent.respondsTo(\findWindow).if{
		this.view.parent.findWindow.alwaysOnTop=alwaysOnTop;
		}
	
	}
	font_{ arg font;

			labelView.notNil.if{labelView.font=font};
			widget.font=font;
	}
	

	window{^if(view.parent.respondsTo(\findWindow)){view.parent.findWindow}}
	
	
	prSubViewBounds{arg rect, hasLabel;
		var widgetBounds,labelBounds,tmp;
		hasLabel.not.if{gap=0@0; labelSize=0@0};
		
		if (layout==\vert)
			{ widgetBounds= Rect(  // fit to full width
					0,
					labelSize.y+gap.y,
					rect.width,  
					rect.height-labelSize.y-gap.y
					);
			labelBounds=Rect(0,0,widgetBounds.width,labelSize.y);//fit to full width
		}{ 
			widgetBounds= Rect(   // fit to full remaining
					labelSize.x+gap.x,
					0,
					rect.width-labelSize.x-gap.x,  
					rect.height
					);
			labelBounds=Rect(0,0, labelSize.x ,widgetBounds.height )}; // to left
		
		^[labelBounds, widgetBounds]
	}
	
	prMakeView{arg parentView,bounds; // return a container, or a popUpWindow with a container
		var w, winBounds, view;
		parentView.isNil.if{
			popUp=true;
				// if bounds is a point the place the window on screen
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
				// inset the bounds to make a nice margin
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds)
					.relativeOrigin_(true).resize_(2);
		// normal parent view			
		}{
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};
	^[view,bounds];
	}
}


EZLists : EZGui{  // an abstract class

	var <items, <>globalAction, value; 
	
	*new { arg parentView, bounds, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth,labelHeight=20, layout, gap;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelHeight,layout, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelHeight, layout,  argGap;
			
		// try to use the parent decorator gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		argGap.isNil.if{ 
			gap = decorator.tryPerform(\gap);
			gap = gap ? (2@2)}
			{gap=argGap};
		
		// init the views (handled by subclasses)
		this.initViews(  parentView, bounds, label, labelWidth,labelHeight,layout );
			
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
	
	initViews{}  // override this for your subclass views
	
	value{ ^widget.value }
	
	value_{|val| widget.value_(val)}
	
	valueAction_{|val| this.value_(val); this.doAction}
		
	doAction {items.at(this.value).value.value(this); globalAction.value}

	items_{arg assocArray; 
		items=assocArray;
		widget.items=assocArray.collect({|item| item.key});
	}	
		
	addItem{arg name, action;
		this.insertItem(nil, name, action);
	}
	
	insertItem{ arg index, name, action;
		var temp;
		index = index ? items.size;
		this.items=items.insert(index, name.asSymbol -> action);
	 }
	
	removeItemAt{ arg index;
		var temp;
		items.removeAt(index);
		this.items_(items)
	
	}


}
