EZPopUpMenu : EZLists{

	initViews{ |parentView, bounds, label, labelWidth, labelHeight, arglayout|
		var labelBounds, listBounds;

		labelWidth = labelWidth ? 80;
		layout = arglayout ? \horz;
		labelSize = labelWidth @ labelHeight;

		if(bounds.isNil) { bounds = 160 @ 20 };

		// if no parent, then pop up window
		# view, bounds = this.prMakeView(parentView, bounds);

		// calcualate bounds
		# labelBounds, listBounds = this.prSubViewBounds(innerBounds, label.notNil);

		// insert the views

		if(label.notNil) { //only add a label if desired
			if(layout == \vert) {
				labelView = StaticText.new(view, labelBounds).resize_(2);
				labelView.align = \left
			} {
				labelView = StaticText.new(view, labelBounds);
				labelView.align = \right
			};
			labelView.string = label
		};

		widget = PopUpMenu.new(view, listBounds).resize_(5)
	}

	menu { ^ widget }

	setColors{ |stringBackground, stringColor, menuBackground, menuStringColor, background|

		if(stringBackground.notNil) {
			if(labelView.notNil) { labelView.background_(stringBackground) } };
		if(stringColor.notNil) {
			if(labelView.notNil) { labelView.stringColor_(stringColor) } };
		if(menuBackground.notNil) {
			this.menu.background_(menuBackground) };
		if(menuStringColor.notNil) {
			this.menu.stringColor_(menuStringColor) };
		if(background.notNil) {
			view.background = background }
	}

}
