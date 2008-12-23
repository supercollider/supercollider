EZListView : EZLists{
	
	initViews{ arg parentView, bounds, label, labelWidth,argLabelPosition;
		var labelBounds, listBounds,w, winBounds, labelHeight;
		
		bounds.isNil.if{bounds= 160@200};
		labelWidth = labelWidth ? 80;
		labelHeight =  20;
		labelPosition=argLabelPosition ? \top;
		labelSize=labelWidth@labelHeight;

		parentView.isNil.if{
				if (bounds.class==Point){
					winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
					bounds.x,bounds.y)
					}{winBounds=bounds};
				w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).resize_(5).relativeOrigin_(true);
			}{
			
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};
		view.decorator=FlowLayout(view.bounds,0@0,0@0);

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
		}

	listView{^widget}
	
}


