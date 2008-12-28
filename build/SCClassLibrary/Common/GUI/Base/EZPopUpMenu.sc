EZPopUpMenu : EZLists{
	
	initViews{ arg parentView, bounds, label, labelWidth,labelHeight,arglayout;
		var labelBounds, listBounds,w, winBounds ;
		
		labelWidth = labelWidth ? 80;
		layout=arglayout ? \horz;
		labelSize=labelWidth@labelHeight;
		
		parentView.isNil.if{
				bounds.isNil.if{bounds= 160@44};
				if (bounds.class==Point){
					bounds = bounds.x@max(bounds.y,bounds.y+24);// window minimum height;
					winBounds=Rect(200, Window.screenBounds.height-bounds.y-100,
					bounds.x,bounds.y)
					}{// window minimum height;
					winBounds = bounds.height_(max(bounds.height,bounds.height+24))
					};
				w = GUI.window.new("",winBounds).alwaysOnTop_(alwaysOnTop);
				parentView=w.asView;
				w.front;
				bounds=bounds.asRect;
				bounds=Rect(4,4,bounds.width-8,bounds.height-24);
				view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true).resize_(2);
			}{
			bounds.isNil.if{bounds= 160@20};
			bounds=bounds.asRect;
			view=GUI.compositeView.new(parentView,bounds).relativeOrigin_(true);
		};

		# labelBounds,listBounds = this.prSubViewBounds(bounds, label.notNil);
		
		label.notNil.if{ //only add a label if desired
			if (layout==\vert){
				labelView = GUI.staticText.new(view, labelBounds).resize_(2);
				labelView.align = \left;
				}{
				labelView = GUI.staticText.new(view, labelBounds);
				labelView.align = \right;
				};
			labelView.string = label;
		};
				
		widget = GUI.popUpMenu.new(view, listBounds).resize_(5);
	}
	
	menu {^ widget}
	
	setColors{arg stringBackground, strColor, menuBackground,  menuStringColor,background ;
			
			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};};
			strColor.notNil.if{	
				labelView.notNil.if{labelView.stringColor_(strColor)};};
			menuBackground.notNil.if{		
				this.menu.background_(menuBackground);};
			menuStringColor.notNil.if{	
				this.menu.stringColor_(menuStringColor);};
			background.notNil.if{	
				view.background=background;};
	}
	
}


