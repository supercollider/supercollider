EZGui{ // an abstract class
	var <>labelView, widget, <view, <gap,  <layout, <labelSize, <alwaysOnTop=false;
	
		
	visible { ^view.getProperty(\visible) }
	visible_ { |bool|  view.setProperty(\visible,bool)  }
	
	enabled {  ^widget.enabled } 
	enabled_ { |bool| widget.enabled_(bool) }
	
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
	

	
	
	layout_{arg pos;
		layout=pos;
		labelSize=80@20;
		labelView.notNil.if{
			(layout==\vert).if{
			labelView.align=\left;
			labelView.resize_(2);
			}{
			labelView.align=\right;
			labelView.resize_(4);
			};
		};
		
		this.bounds_(view.bounds); 	
	}
	
	
	prSubViewBounds{arg rect, hasLabel;
		var widgetBounds,labelBounds,tmp;
		hasLabel.not.if{gap=0@0; labelSize=0@0};
		
		if (layout==\vert)
			{ widgetBounds= Rect(
					0,
					labelSize.y+gap.y,
					rect.width,  
					rect.height-labelSize.y-gap.y
					);
			if (view.parent.respondsTo(\findWindow)){
			 tmp = view.parent.findWindow.bounds;
			 view.parent.findWindow.bounds = tmp.height_(max(tmp.height,62+gap.y));
			 widgetBounds = widgetBounds.height_(max(widgetBounds.height,16));
			};
			labelBounds=Rect(0,0,widgetBounds.width,labelSize.y);}
			{ widgetBounds= Rect(
					labelSize.x+gap.x,
					0,
					rect.width-labelSize.x-gap.x,  
					rect.height
					);
			labelBounds=Rect(0,0, labelSize.x ,widgetBounds.height )};
		
		^[labelBounds, widgetBounds]
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
			
		var	decorator = parentView.asView.tryPerform(\decorator);
		
		argGap.isNil.if{ 
			gap = decorator.tryPerform(\gap);
			gap = gap ? (2@2)}
			{gap=argGap};
		
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
	
	initViews{}  // override this for your view
	
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
