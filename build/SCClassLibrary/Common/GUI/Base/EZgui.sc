EZGui{ // an abstract class
	var <>labelView, <widget, <view, <gap, <labelPosition, labelSize, <alwaysOnTop=false;
	
	label_{ arg string;
	
		labelView.isNil.if{
			labelSize=80@20;
			if(labelPosition==\top, {
				labelView = GUI.staticText.new(view, view.bounds.width@ labelSize.y);
				labelView.align = \left;
			},{
				labelView = GUI.staticText.new(view, labelSize.x @ view.bounds.height);
				labelView.align = \right;
			});
			labelView.string = string;
	 		this.bounds_(view.bounds); //recalculate bounds
		}{
		
	 	labelView.string_(string)
	 	};
	 	
	}
	
	label {
	 	^labelView.string;
	}
		
	visible { ^view.getProperty(\visible) }
	visible_ { |bool|  view.setProperty(\visible,bool)  }
	
	enabled {  ^widget.enabled } 
	enabled_ { |bool| widget.enabled_(bool) }
	
	remove { [widget, labelView].do(_.remove) }
	
	bounds{^view.bounds}
	
	bounds_{arg rect;
		var labelBounds, listBounds;
		view.bounds=rect;
		# labelBounds,listBounds = this.prSubViewBounds(view.bounds, labelView.notNil);
		
		labelView.notNil.if{labelView.bounds=labelBounds};
		widget.bounds=listBounds;
	
	}
	
	labelHeight{
		^ labelSize.y
	}
	
	labelHeight_{arg height;
		labelSize.y=height;
		this.bounds_(view.bounds); 	
	}
	labelWidth{
		^ labelSize.x
	}
	
	alwaysOnTop_{arg bool;
		alwaysOnTop=bool;
		this.view.parent.respondsTo(\findWindow).if{
		this.view.parent.findWindow.alwaysOnTop=alwaysOnTop;
		}
	
	}
	
	labelWidth_{arg width;
		labelSize.x=width;
		this.bounds_(view.bounds); 	
	}
	
	labelPosition_{arg pos;
		labelPosition=pos;
		labelSize=80@20;
		labelView.notNil.if{
			(labelPosition==\top).if{
			labelView.align=\left;
			labelView.resize_(2);
			}{
			labelView.align=\right;
			labelView.resize_(4);
			};
		};
		
		this.bounds_(view.bounds); 	
	}
	
	gap_{arg val;
		gap=val;
		this.bounds_(this.view.bounds);
		
	}
	
	prSubViewBounds{arg rect, hasLabel=true;
		var widgetBounds,labelBounds, tempGap,tmp;
		tempGap=gap;	
		hasLabel.not.if{tempGap=0; labelSize=0@0};
		
		if (labelPosition==\top)
			{ widgetBounds= Rect(
					0,
					labelSize.y+tempGap,
					rect.width,  
					rect.height-labelSize.y-tempGap
					);
			if (view.parent.respondsTo(\findWindow)){
			 tmp = view.parent.findWindow.bounds;
			 view.parent.findWindow.bounds = tmp.height_(max(tmp.height,62+gap));
			 widgetBounds = widgetBounds.height_(max(widgetBounds.height,16));
			};
			labelBounds=Rect(0,0,widgetBounds.width,labelSize.y);}
			{ widgetBounds= Rect(
					labelSize.x+tempGap,
					0,
					rect.width-labelSize.x-tempGap,  
					rect.height
					);
			labelBounds=Rect(0,0, labelSize.x ,widgetBounds.height )};
		
		^[labelBounds, widgetBounds]
	}
	
	
}


EZLists : EZGui{  // an abstract class

	var <items, <>globalAction, value; 
	
	*new { arg parentView, bounds, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth, labelPosition, gap;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelPosition, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelPosition,  argGap;
			
		var	decorator = parentView.asView.tryPerform(\decorator);
		
		argGap.isNil.if
			{gap = decorator.tryPerform(\gap).tryPerform(\x)}
			{gap = argGap};
		gap  = gap ? 2;
		
		this.initViews(  parentView, bounds, label, labelWidth,labelPosition );
			
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
	
	initViews{ arg parentView, bounds, label, labelWidth, labelHeight,argLabelPosition;
			var labelBounds, listBounds,w, winBounds;
	}
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
