EZPopUpMenu : EZLists{

	initViews{ arg parentView, bounds, label, labelWidth,labelHeight,arglayout;
		var labelBounds, listBounds;

		labelWidth = labelWidth ? 80;
		layout=arglayout ? \horz;
		labelSize=labelWidth@labelHeight;

		bounds.isNil.if{bounds= 160@20};

		// if no parent, then pop up window
		# view,bounds = this.prMakeView( parentView,bounds);

		// calcualate bounds
		# labelBounds,listBounds = this.prSubViewBounds(innerBounds, label.notNil);

		// insert the views

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

	setColors{arg stringBackground, stringColor, menuBackground,  menuStringColor,background ;

			stringBackground.notNil.if{
				labelView.notNil.if{labelView.background_(stringBackground)};};
			stringColor.notNil.if{
				labelView.notNil.if{labelView.stringColor_(stringColor)};};
			menuBackground.notNil.if{
				this.menu.background_(menuBackground);};
			menuStringColor.notNil.if{
				this.menu.stringColor_(menuStringColor);};
			background.notNil.if{
				view.background=background;};
	}

}
