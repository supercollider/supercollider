EZListView : EZLists{

	initViews{ |parentView, bounds, label, labelWidth, labelHeight, arglayout|
		var labelBounds, listBounds;

		labelWidth = labelWidth ? 80;
		layout = arglayout ? \vert;
		labelSize = labelWidth @ labelHeight;

		if(bounds.isNil) { bounds = 160 @ 200 };

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
				labelView = StaticText.new(view, labelBounds).resize_(4);
				labelView.align = \right
			};
			labelView.string = label
		};

		widget = ListView.new(view, listBounds).resize_(5)
	}

	listView{ ^widget }

	setColors{ |stringBackground, stringColor, listBackground, listStringColor,
			selectedStringColor, hiliteColor, background|

		if(stringBackground.notNil) {
			if(labelView.notNil) { labelView.background_(stringBackground) } };
		if(stringColor.notNil) {
			if(labelView.notNil) { labelView.stringColor_(stringColor) } };
		if(listBackground.notNil) {
			this.listview.background_(listBackground) };
		if(listStringColor.notNil) {
			this.listview.stringColor_(listStringColor) };
		if(selectedStringColor.notNil) {
			this.listview.selectedStringColor_(selectedStringColor) };
		if(hiliteColor.notNil) {
			this.listview.hiliteColor_(hiliteColor) };

		if(background.notNil) {
			view.background = background }
	}
}
