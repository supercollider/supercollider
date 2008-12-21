EZListView{
	var   <>globalAction, <>labelView, <items, widget, <view, <gap, <labelPosition, labelSize;
	
	*new { arg parentView, bounds, label,items, globalAction, initVal=0, 
			initAction=false, labelWidth=80, labelHeight=20, labelPosition=\top, gap=4;
			
		^super.new.init(parentView, bounds, label, items, globalAction, initVal, 
			initAction, labelWidth,labelHeight,labelPosition, gap);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal, 
			initAction, labelWidth, labelHeight, arglabelPosition,  argGap;
		var labelBounds, listBounds,w, winBounds;
		bounds.isNil.if{bounds= 160@200};
		labelPosition=arglabelPosition;
		labelSize=labelWidth@labelHeight;
		gap=argGap;
		
		parentView.isNil.if{
				if (bounds.class==Point){
					winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
					bounds.x,bounds.y)
					}{winBounds=bounds};
				w = GUI.window.new("",winBounds);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true).resize_(5);
			}{
			
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};

		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		label.notNil.if{ //only add a label if desired
			if (labelPosition==\top){
				labelView = GUI.staticText.new(view, labelBounds).resize_(2);
				labelView.align = \left;
				}{
				labelView = GUI.staticText.new(view, labelBounds).resize_(4);
				labelView.align = \right;
				};
			labelView.string = label;
		};
				
		widget = GUI.listView.new(view, listBounds).resize_(5);
		
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
	
	listView{^widget}
	
	value{ ^widget.value }
	
	value_{|val| widget.value_(val)}
	
	valueAction_{|val| this.value_(val); this.doAction}
	
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
	 		this.bounds_(view.bounds); //recalculate bounds
		}{
		
	 	labelView.string_(string)
	 	};
	 	
	}
	
	label { arg label, items, globalAction;
	 	labelView.string;
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


