EZPopUpMenu{
	var   <>globalAction, <>labelView, <items, <menu, <view, <gap;
	
	*new { arg parentView, bounds= 160@16, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, argGap;
		
		bounds=bounds.asRect;
		parentView.notNil.if{view = parentView }{view = FlowView.new()};
		view=GUI.compositeView.new(view,bounds).relativeOrigin_(true);
		
		gap=argGap;
				
		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view, labelWidth-gap @ bounds.height);
			labelView.string = label;
			labelView.align = \right;
		}{labelWidth=0};
				
		menu = GUI.popUpMenu.new(view,  Rect(labelWidth,0,bounds.width-labelWidth,  bounds.height));
		
		this.items=argItems ? [];
		
		
		globalAction=argGlobalAction;
		
		menu.action={arg obj;
			items.at(obj.value).value.value(obj);
			globalAction.value(obj);
			};		
			
		this.value_(initVal);
			
		items.notNil.if{
			if(initAction){
					items.at(initVal).value.value(this); // You must do this like this
					globalAction.value(this);	// since menu's array is not accessible yet
				}
			{this.value_(initVal)};
		};
	}	
	
	value{ ^menu.value }
	
	value_{|val| menu.value_(val)}
	
	valueAction_{|val| this.value_(val); this.doAction}
	
	items_{arg assocArray; 
		items=assocArray;
		menu.items=assocArray.collect({|item| item.key});
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
	
	doAction {items.at(this.value).value.value(this); globalAction.value}

	label_{ arg string;
		labelView.isNil.if{
			labelView = GUI.staticText.new(view, 80-gap @ view.bounds.height);
			labelView.string = string;
			labelView.align = \right;
			this.labelWidth_(80);
		}{
	 	labelView.string_(string)
	 	};
	 	
	}
	
	label { arg label, items, globalAction;
	 	labelView.string;
	}
		
	visible { ^menu.visible }
	visible_ { |bool| [menu, labelView].do(_.visible_(bool)) }
	
	enabled {  ^menu.enabled } 
	enabled_ { |bool| menu.enabled_(bool) }
	
	remove { [menu, labelView].do(_.remove) }
	
	bounds{^view.bounds}
	bounds_{arg rect;
		var labelWidth=0;
		labelView.notNil.if{labelWidth= labelView.bounds.width+gap};
		view.bounds=rect.asRect;
		menu.bounds= Rect(
				labelWidth,
				0,
				view.bounds.x-labelWidth,  
				view.bounds.y
				);
	}
	
	labelWidth{
		^ if(labelView.notNil, 
			{labelView.bounds.width+gap}, 
			{"No labelView created".warn; 
			nil})
	}
	
	labelWidth_{arg width;
		if(labelView.notNil, 
		{	labelView.bounds=labelView.bounds.width_(width-gap);
			menu.bounds= Rect(width,0,view.bounds.width-width,  view.bounds.height);},
		{"No labelView created".warn;});
	}
	
	gap_{arg val;
		var oldGap;
		oldGap=gap;
		gap=val;
		labelView.notNil.if{this.labelWidth_(labelView.bounds.width+oldGap)};
		
	}
	
	
	
}


