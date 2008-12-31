EZListView : EZLists{
	
	initViews{ arg parentView, bounds, label, labelWidth,labelHeight,arglayout;
		var labelBounds, listBounds;
		
		labelWidth = labelWidth ? 80;
		layout=arglayout ? \vert;
		labelSize=labelWidth@labelHeight;
		
		bounds.isNil.if{bounds= 160@200};
		
		// if no parent, then pop up window 
		# view,bounds = this.prPopUpWindow( parentView,bounds);	
		// calcualate bounds
		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		// insert the views	
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


