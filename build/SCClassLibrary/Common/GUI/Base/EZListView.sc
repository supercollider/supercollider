EZListView{
	var   <>globalAction, <>labelView, <items, <listView, <view, <gap, <labelPosition;
	
	*new { arg parentView, bounds= 160@160, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, labelHeight=20, labelPosition=\top, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelHeight,labelPosition, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelHeight, arglabelPosition,  argGap;
		var listBounds;
		bounds=bounds.asRect;
		labelPosition=arglabelPosition;
		parentView.notNil.if{view = parentView }{view = FlowView.new()};
		view=GUI.compositeView.new(view,bounds).relativeOrigin_(true);
		
		gap=argGap;
				
		label.notNil.if{ //only add a label if desired
			if (labelPosition==\top){
				labelView = GUI.staticText.new(view, bounds.width @ labelHeight-gap);
				labelView.align = \left;
				}{
				labelView = GUI.staticText.new(view, labelWidth-gap @ bounds.height);
				labelView.align = \right;
				};
			labelView.string = label;
		}{labelWidth=0; labelHeight=0};
				
		if (labelPosition==\top){
		 	listBounds= Rect(0,labelHeight,bounds.width,  bounds.height-labelHeight);
		 	}{
		 	listBounds= Rect(labelWidth,0,bounds.width-labelWidth,  bounds.height);
		 	};
		
		listView = GUI.listView.new(view, listBounds);
		
		this.items=argItems ? [];
		
		
		globalAction=argGlobalAction;
		
		listView.action={arg obj;
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
	
	value{ ^listView.value }
	
	value_{|val| listView.value_(val)}
	
	valueAction_{|val| this.value_(val); this.doAction}
	
	items_{arg assocArray; 
		items=assocArray;
		listView.items=assocArray.collect({|item| item.key});
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
			if(labelPosition==\top, {
				labelView = GUI.staticText.new(view, view.bounds.width@ 20-gap);
				this.labelWidth_(this.bounds.width);
				this.labelHeight_(20);
				labelView.align = \left;
			},{
				labelView = GUI.staticText.new(view, 80-gap @ view.bounds.height);
				this.labelWidth_(80);
				this.labelHeight_(this.bounds.height);
				labelView.align = \right;
			});
			labelView.string = string;
		}{
	 	labelView.string_(string)
	 	};
	 	
	}
	
	label { arg label, items, globalAction;
	 	labelView.string;
	}
		
	visible { ^listView.visible }
	visible_ { |bool| [listView, labelView].do(_.visible_(bool)) }
	
	enabled {  ^listView.enabled } 
	enabled_ { |bool| listView.enabled_(bool) }
	
	remove { [listView, labelView].do(_.remove) }
	
	bounds{^view.bounds}
	bounds_{arg rect;
		var labelWidth=0,labelHeight=0;
		view.bounds=rect.asRect;
		
		if (labelPosition==\top,{
		
			labelView.notNil.if{
				labelHeight=labelView.bounds.height;
				};
			listView.bounds= Rect(
					0,
					labelHeight+gap,
					view.bounds.width,  
					view.bounds.height-labelHeight-gap
					);
		},{
			labelView.notNil.if{
				labelWidth= labelView.bounds.width;
				};
			listView.bounds= Rect(
					labelWidth+gap,
					0,
					view.bounds.width-labelWidth-gap,  
					view.bounds.height
					);
		});
	}
	
	labelWidth{
		^ if(labelView.notNil, {
			if(labelPosition==\top,
				{labelView.bounds.width}, 
				{labelView.bounds.width+gap} 
			)},
			{"No labelView created".warn; 
			nil})
	}
	
	labelWidth_{arg width;
		if(labelView.notNil, 
			if(labelPosition==\top,
				{"labelPosition is top, so label width is automatic".warn;},
				{labelView.bounds=labelView.bounds.width_(width-gap);
					listView.bounds= Rect(width,0,view.bounds.width-width,  view.bounds.height);
				}),
		{"No labelView created".warn;});
	}
	labelHeight{
		^ if(labelView.notNil, {
			if(labelPosition==\top,
				{labelView.bounds.height+gap}, 
				{labelView.bounds.height}
			)}, 
			{"No labelView created".warn; 
			nil})
	}
	
	labelHeight_{arg height;
		if(labelView.notNil, 
			if(labelPosition==\top,
				{labelView.bounds=labelView.bounds.height_(height-gap);
					listView.bounds= Rect(0, height,view.bounds.width,view.bounds.height-height);
				},
				{"labelPosition is left, so label height is automatic".warn;}),

		{"No labelView created".warn;});
	}
	
	labelPosition_{arg pos;
		if (pos !=labelPosition,{
			labelPosition=pos;
			if (labelView.notNil,{
				if(labelPosition==\top,{
					 labelView.bounds= (view.bounds.width@20).asRect; 
					this.bounds=view.bounds;
					this.labelView.align_(\left);
				},{
					labelView.bounds=(80@view.bounds.height).asRect; 
					this.bounds=view.bounds;						this.labelView.align_(\right);
				});
			});
		});
		
	}
	
	gap_{arg val;
		gap=val;
		this.bounds_(this.view.bounds);
		
	}
	
	
	
}


