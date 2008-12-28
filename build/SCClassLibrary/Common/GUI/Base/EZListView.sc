EZListView : EZLists{
	
	initViews{ arg parentView, bounds, label, labelWidth,labelHeight,arglayout;
		var labelBounds, listBounds,w, winBounds;
		
		bounds.isNil.if{bounds= 160@200};
		labelWidth = labelWidth ? 80;
		layout=arglayout ? \vert;
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

		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		label.notNil.if{ //only add a label if desired
			if (layout==\vert){
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
	
	setColors{arg stringBackground, strColor, listBackground,  listStringColor,
	 selectedStringColor,hiliteColor, background ;
			
			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};};
			strColor.notNil.if{	
				labelView.notNil.if{labelView.stringColor_(strColor)};};
			listBackground.notNil.if{		
				this.listview.background_(listBackground);};
			listStringColor.notNil.if{	
				this.listview.stringColor_(listStringColor);};
			selectedStringColor.notNil.if{	
				this.listview.selectedStringColor_(selectedStringColor);};
			hiliteColor.notNil.if{	
				this.listview.hiliteColor_(hiliteColor);};
				
			background.notNil.if{	
				view.background=background;};
	}
}


