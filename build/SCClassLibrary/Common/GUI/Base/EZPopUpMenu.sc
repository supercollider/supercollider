EZPopUpMenu{
	var   <>globalAction, <>labelView, <items, <menu, <view, <gap, labelSize;
	
	*new { arg parentView, bounds= 160@20, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, argGap;
			
		var labelBounds,menuBounds,w;
		
		bounds=bounds.asRect;
		labelSize=labelWidth@bounds.height;
		gap=argGap;
		
		parentView.isNil.if{	
			
				w = GUI.window.new("",Rect(200,Window.screenBounds.height-bounds.height-100,
					bounds.width+8,bounds.height+28));
				w.addFlowLayout;
				parentView=w.asView;
				w.front;
			};
			
		view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		
		# labelBounds,menuBounds = this.prSubViewBounds(bounds, label.notNil);

		label.notNil.if{ //only add a label if desired
			labelView = GUI.staticText.new(view,labelBounds);
			labelView.string = label;
			labelView.align = \right;
		};
				
		menu = GUI.popUpMenu.new(view, menuBounds);
		
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
			labelSize=80@view.bounds.height;
			labelView = GUI.staticText.new(view, 80 @ labelSize.x);
			labelView.string = string;
			labelView.align = \right;
	 		this.bounds=view.bounds;
		}{
	 	labelView.string_(string)
	 	};
	 	
	}
	
	label { arg label, items, globalAction;
	 	labelView.string;
	}
		
	visible { ^view.getProperty(\visible) }
	visible_ { |bool|  view.setProperty(\visible,bool)  }
	
	enabled {  ^menu.enabled } 
	enabled_ { |bool| menu.enabled_(bool) }
	
	remove { [menu, labelView].do(_.remove) }
	
	bounds{^view.bounds}
	bounds_{arg rect;
		var labelBounds, menuBounds;
		view.bounds=rect;
		# labelBounds,menuBounds = this.prSubViewBounds(view.bounds, labelView.notNil);
		
		labelView.notNil.if{labelView.bounds=labelBounds};
		menu.bounds=menuBounds;
	
	}
	
	labelWidth{
		^ labelSize.x
	}
	
	labelWidth_{arg width;
		labelSize.x=width;
		this.bounds_(view.bounds); 	
	}
	
	gap_{arg val;
		gap=val;
		this.bounds_(this.view.bounds);
		
	}
	
	prSubViewBounds{arg rect, hasLabel=true;
		var menuBounds,labelBounds, tempGap;
		tempGap=gap;	
		
		hasLabel.not.if{tempGap=0; labelSize=0@0};
		
		menuBounds= Rect(
				labelSize.x+tempGap,
				0,
				view.bounds.width-labelSize.x-tempGap,  
				view.bounds.height
				);
		labelBounds=Rect(0,0, labelSize.x ,menuBounds.height );
		
		^[labelBounds, menuBounds]
	}
	
}


