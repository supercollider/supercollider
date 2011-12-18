EZGui{ // an abstract class
	var <>labelView, <widget, <view, gap, popUp=false, innerBounds,
	<>action,   <layout, <value, labelSize, <alwaysOnTop=false, margin;
	var scaler=1;  //for swing compatibility
	// general stuff for all EZ classes. override as needed
	visible { ^view.getProperty(\visible) }
	visible_ { |bool|  view.setProperty(\visible,bool)  }

	enabled {  ^view.enabled }
	enabled_ { |bool| view.enabled_(bool) }

	remove { view.remove}

	alwaysOnTop_{arg bool;
		alwaysOnTop=bool;
		popUp.if{this.window.alwaysOnTop=alwaysOnTop};

	}
	font_{ arg font;
		labelView.notNil.if{labelView.font=font};
		widget.font=font;
	}

	window{^ popUp.if{view.parent.findWindow};}

	onClose{^ popUp.if{this.window.onClose}{view.onClose};}

	onClose_{|func| popUp.if{this.window.onClose_(func)}{view.onClose_(func)};}


	//// value stuff. override as needed


	value_{|val| value=val; widget.value_(val)}

	valueAction_{|val| this.value_(val); this.doAction}

	doAction {this.action.value(this)}

	bounds{^view.bounds}
	///////// private methods. You can still override these in subclasses

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

		^[labelBounds, widgetBounds].collect{arg v; v.moveBy(margin.x,margin.y)}
	}


	prMakeView{arg parentView,bounds; // return a container, or a popUpWindow with a container
		var w, winBounds, view, extraHeight=24;
		(GUI.id==\swing).if{extraHeight=36};
		parentView.isNil.if{
			popUp=true;
				// if bounds is a point the place the window on screen
				if (bounds.class==Point)
					{ bounds = bounds.x@max(bounds.y,bounds.y+extraHeight);// window minimum height;
					 winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
								bounds.x,bounds.y)
					}{// window minimum height;
					winBounds = bounds.height_(max(bounds.height,bounds.height+extraHeight))
					};
				w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				// inset the bounds to make a nice margin
				bounds=Rect(4,4,bounds.width-8,bounds.height-extraHeight);
				view=GUI.compositeView.new(parentView,bounds).resize_(2);
				w.bounds=w.bounds; // swing needs this for some reason, or bounds are too high

		}{// normal parent view
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds);
		};

		innerBounds=view.bounds.insetBy(margin.x,margin.y);
	^[view,bounds];
	}

	prMakeMarginGap{arg parentView, argMargin, argGap;
		//try to use the parent decorator gap
		var	decorator = parentView.asView.tryPerform(\decorator);
		argGap.isNil.if{
			gap = gap ? decorator.tryPerform(\gap).copy; // use copy to protect the parent gap
			gap = gap ? (2@2)}
			{gap=argGap};

		argMargin.isNil.if{margin=0@0}{margin=argMargin};
	}

}


EZLists : EZGui{  // an abstract class

	var <items, <>globalAction;

	*new { arg parentView, bounds, label,items, globalAction, initVal=0,
			initAction=false, labelWidth,labelHeight=20, layout, gap, margin;

		^super.new.init(parentView, bounds, label, items, globalAction, initVal,
			initAction, labelWidth,labelHeight,layout, gap, margin);
			}

	init { arg parentView, bounds, label, argItems, argGlobalAction, initVal,
			initAction, labelWidth, labelHeight, layout,  argGap, argMargin;

		// try to use the parent decorator gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		// init the views (handled by subclasses)
		this.initViews(  parentView, bounds, label, labelWidth,labelHeight,layout );

		this.items=argItems ? [];

		globalAction=argGlobalAction;

		widget.action={arg obj;
			items.at(obj.value).value.value(this);
			globalAction.value(this);
			};

		this.value_(initVal);

		items.notNil.if{
			if(initAction){
					items.at(initVal).value.value(this); // You must do this like this
					globalAction.value(this);	// since listView's array is not accessible yet
				};
			this.value_(initVal);
		};

	}

	initViews{}  // override this for your subclass views

	value{ ^widget.value}
	value_{|val| widget.value=val}

	valueAction_{|val| widget.value_(val); this.doAction}

	doAction {widget.doAction;}

	items_{ arg assocArray;
		assocArray = assocArray.collect({ |it| if (it.isKindOf(Association), { it }, { it -> nil }) });
		items=assocArray;
		widget.items=assocArray.collect({|item| item.key});
	}

	item {^items.at(this.value).key}
	itemFunc {^items.at(this.value).value}

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

	replaceItemAt{ arg index, name, action;
		var temp;
		name = name ? items.at(index).key;
		action = action ? items.at(index).value;
		this.removeItemAt(index);
		this.insertItem(index, name, action);

	}

}
