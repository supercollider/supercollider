	// 	value can be any object,
	// 	display is asCompileString
EZText : EZGui {
	var <textField;

	*new { arg parent, bounds, label, action, initVal,
			initAction=false, labelWidth=60, textWidth,
			labelHeight=20,  layout=\horz, gap, margin;

		^super.new.init(parent, bounds, label, action,
			initVal, initAction, labelWidth, textWidth,
				labelHeight, layout, gap, margin)
	}

	init { arg parentView, bounds, label, argAction, initVal,
			initAction, labelWidth, argTextWidth,
			labelHeight, argLayout, argGap, argMargin;

		var labelBounds, textBounds, textSize;
		var textWidth = argTextWidth;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);


		layout=argLayout;

		bounds.isNil.if {bounds= 200@20};

		// if no parent, then pop up window
		# view,bounds = this.prMakeView( parentView,bounds);

		textWidth.isNil.if{textWidth=140}{
			labelWidth=bounds.width-textWidth; //override the labelWidth
		//	if (layout==\line2){unitWidth=bounds.width-textWidth}; //override the unitWidth
		};
		labelSize=labelWidth@labelHeight;
		textSize = textWidth@labelHeight;

		// calculate bounds
		# labelBounds,textBounds
				= this.prSubViewBounds(innerBounds, label.notNil);

		// insert the views

		label.notNil.if{ //only add a label if desired
				labelView = GUI.staticText.new(view, labelBounds);
			if (layout==\line2)
				{labelView.align = \left;}
				{labelView.align = \right;};
			labelView.string = label;
		};

		// set view parameters and actions
		initVal = initVal ? "";
		action = argAction;

		textField = TextField(view, textBounds).resize_(2);

		textField.action = {
			var newstr = textField.string;
			var newval = try { newstr.interpret };
			if (newval.notNil or: { newstr == "" }) {
				this.valueAction_(newval);
			} {
			//	"EZText compile failed - reset to prev value.".postln;
				textField.string = this.value.asCompileString;
			}
		};

		if (initAction) {
			this.valueAction = initVal;
		}{
			this.value = initVal;
		};

		this.prSetViewParams;
	}

	value_ { |inval|
		value = inval;
		textField.string = value.asCompileString;
	}

	font_{ arg font;
		labelView.notNil.if{labelView.font=font};
		textField.font=font;
	}

	setColors { arg stringBackground, stringColor, textBackground, textStringColor,
			 textNormalColor, textTypingColor, background ;

		stringBackground.notNil.if{
			labelView.notNil.if{labelView.background_(stringBackground)};
			};
		stringColor.notNil.if{
			labelView.notNil.if{labelView.stringColor_(stringColor)};
			};
		textBackground.notNil.if{
			textField.background_(textBackground);	};
		textNormalColor.notNil.if{
			textField.normalColor_(textNormalColor);};

		textTypingColor.notNil.if{
			textField.typingColor_(textTypingColor);};
		textStringColor.notNil.if{
			textField.stringColor_(textStringColor);};
		background.notNil.if{
			view.background=background;};
	}

	prSetViewParams { // sets resize and alignment for different layouts
        switch (layout,
        \vert, {
            labelView.notNil.if{labelView.resize_(2).align_(\left)};
            textField.resize_(5);
        },
        \horz, {
            labelView.notNil.if{
                labelView.resize_(4).align_(\right);
                textField.resize_(5);
            }{
                textField.resize_(5);
            };
        });
            popUp.if{ view.resize_(2) };
    }
}
