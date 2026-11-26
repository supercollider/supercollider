// 	value can be any object,
// 	display is asCompileString
EZText : EZGui {
	var <textField;

	*new { |parent, bounds, label, action, initVal,
			initAction = false, labelWidth = 60, textWidth,
			labelHeight = 20, layout = \horz, gap, margin|

		^super.new.init(parent, bounds, label, action,
			initVal, initAction, labelWidth, textWidth,
			labelHeight, layout, gap, margin)
	}

	init { |parentView, bounds, label, argAction, initVal,
			initAction, labelWidth, argTextWidth,
			labelHeight, argLayout, argGap, argMargin|

		var labelBounds, textBounds, textSize;
		var textWidth = argTextWidth;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);


		layout = argLayout;

		if(bounds.isNil) { bounds = 200 @ 20 };

		// if no parent, then pop up window
		# view, bounds = this.prMakeView(parentView, bounds);

		if(textWidth.isNil) { textWidth = 140 } {
			labelWidth = bounds.width - textWidth //override the labelWidth
			//	if (layout==\line2){unitWidth=bounds.width-textWidth}; //override the unitWidth
		};
		labelSize = labelWidth @ labelHeight;
		textSize = textWidth @ labelHeight;

		// calculate bounds
		# labelBounds, textBounds
			= this.prSubViewBounds(innerBounds, label.notNil);

		// insert the views

		if(label.notNil) { //only add a label if desired
			labelView = StaticText.new(view, labelBounds);
			if(layout == \line2)
				{ labelView.align = \left }
				{ labelView.align = \right };
			labelView.string = label
		};

		// set view parameters and actions
		initVal = initVal ? "";
		action = argAction;

		textField = TextField(view, textBounds).resize_(2);

		textField.action = {
			var newstr = textField.string;
			var newval = try { newstr.interpret };
			if(newval.notNil or: { newstr == "" }) {
				this.valueActionIfChanged_(newval)
			} {
				//	"EZText compile failed - reset to prev value.".postln;
				textField.string = this.value.asCompileString
			}
		};

		if(initAction) {
			this.valueAction = initVal
		} {
			this.value = initVal
		};

		this.prSetViewParams
	}

	value_ { |inval|
		value = inval;
		textField.string = value.asCompileString
	}

	font_{ |font|
		if(labelView.notNil) { labelView.font = font };
		textField.font = font
	}

	setColors { |stringBackground, stringColor, textBackground, textStringColor,
			textNormalColor, textTypingColor, background|

		if(stringBackground.notNil) {
			if(labelView.notNil) { labelView.background_(stringBackground) }
		};
		if(stringColor.notNil) {
			if(labelView.notNil) { labelView.stringColor_(stringColor) }
		};
		if(textBackground.notNil) {
			textField.background_(textBackground) };
		if(textNormalColor.notNil) {
			textField.normalColor_(textNormalColor) };

		if(textTypingColor.notNil) {
			textField.typingColor_(textTypingColor) };
		if(textStringColor.notNil) {
			textField.stringColor_(textStringColor) };
		if(background.notNil) {
			view.background = background }
	}

	prSetViewParams { // sets resize and alignment for different layouts
		switch(layout)
			{ \vert } {
				if(labelView.notNil) { labelView.resize_(2).align_(\left) };
				textField.resize_(5)
			}
			{ \horz } {
				if(labelView.notNil) {
					labelView.resize_(4).align_(\right);
					textField.resize_(5)
				} {
					textField.resize_(5)
				}
			};
		if(popUp) { view.resize_(2) }
	}
}
