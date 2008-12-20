EZListView{
	var   <>globalAction, <>labelView, <items, <listView, <view, <gap, <labelPosition, labelSize;
	
	*new { arg parentView, bounds= 160@200, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, labelHeight=20, labelPosition=\top, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelHeight,labelPosition, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelHeight, arglabelPosition,  argGap;
		var labelBounds, listBounds;
		
		bounds=bounds.asRect;
		labelPosition=arglabelPosition;
		labelSize=labelWidth@labelHeight;
		gap=argGap;
		
		parentView.notNil.if{	
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
			}{
				view = FlowView.new;
				view.parent.bounds=Rect(200,Window.screenBounds.height-bounds.height-100,
					bounds.width+4,bounds.height+4);
			};
		
		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		label.notNil.if{ //only add a label if desired
			if (labelPosition==\top){
				labelView = GUI.staticText.new(view, labelBounds);
				labelView.align = \left;
				}{
				labelView = GUI.staticText.new(view, labelBounds);
				labelView.align = \right;
				};
			labelView.string = label;
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
			labelSize=80@20;
			if(labelPosition==\top, {
				labelView = GUI.staticText.new(view, view.bounds.width@ labelSize.y);
				labelView.align = \left;
			},{
				labelView = GUI.staticText.new(view, labelSize.x @ view.bounds.height);
				labelView.align = \right;
			});
			labelView.string = string;
		}{
		
	 	labelView.string_(string)
	 	};
	 	
	 	this.bounds_(view.bounds); //recalculate bounds
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
		var labelBounds, listBounds;
		view.bounds=rect;
		# labelBounds,listBounds = this.prSubViewBounds(view.bounds, labelView.notNil);
		
		labelView.notNil.if{labelView.bounds=labelBounds};
		listView.bounds=listBounds;
	
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
			}{
			labelView.align=\right;
			};
		};
		
		this.bounds_(view.bounds); 	
	}
	
	gap_{arg val;
		gap=val;
		this.bounds_(this.view.bounds);
		
	}
	
	prSubViewBounds{arg rect, hasLabel=true;
		var listBounds,labelBounds, tempGap;
		tempGap=gap;	
		hasLabel.not.if{tempGap=0; labelSize=0@0};
		
		if (labelPosition==\top,{
			listBounds= Rect(
					0,
					labelSize.y+tempGap,
					view.bounds.width,  
					view.bounds.height-labelSize.y-tempGap
					);
			labelBounds=Rect(0,0,listBounds.width,labelSize.y);
		},{
			listBounds= Rect(
					labelSize.x+tempGap,
					0,
					view.bounds.width-labelSize.x-tempGap,  
					view.bounds.height
					);
			labelBounds=Rect(0,0, labelSize.x ,listBounds.height );
		});
		
		^[labelBounds, listBounds]
	}
	
	
	
	
}


