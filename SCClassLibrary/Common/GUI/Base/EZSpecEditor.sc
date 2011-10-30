EZControlSpecEditor : EZGui {
	
	var <minView, <maxView, <warpView, <stepView, <controlSpec;
	
	*new { arg parent, bounds, label = "Mn/Mx/Wp/Stp", controlSpec, labelWidth=100, labelHeight=20, layout=\horz, gap, margin;

		^super.new.init(parent, bounds, label, controlSpec, labelWidth, labelHeight, layout, gap, margin)
	}
	
	init { arg parentView, bounds, label, argControlSpec, labelWidth, labelHeight, argLayout, argGap, argMargin;

		var labelBounds, minBounds, maxBounds, warpBounds, stepBounds;

		// Set Margin and Gap
		this.prMakeMarginGap(parentView, argMargin, argGap);

		layout=argLayout;
		bounds.isNil.if{bounds = 350@20};


		// if no parent, then pop up window
		#view,bounds = this.prMakeView(parentView, bounds);


		labelSize=labelWidth@labelHeight;

		// calculate bounds of all subviews
		#labelBounds, minBounds, maxBounds, warpBounds, stepBounds = this.prSubViewBounds(innerBounds);

		// instert the views
		labelView = GUI.staticText.new(view, labelBounds);
		labelView.string = label;

//		(unitWidth>0).if{ //only add a unitLabel if desired
//			unitView = GUI.staticText.new(view, unitBounds);
//		};

		minView = GUI.numberBox.new(view, minBounds);
		maxView = GUI.numberBox.new(view, maxBounds);
		warpView = GUI.textField.new(view, warpBounds);
		stepView = GUI.numberBox.new(view, stepBounds);

		// set view parameters and actions

		controlSpec = argControlSpec.asSpec;
		
		minView.value = controlSpec.minval;
		maxView.value = controlSpec.maxval;
		warpView.value = controlSpec.warp.asSpecifier.asCompileString;
		stepView.value = controlSpec.step;
		
		minView.action = { controlSpec.minval = minView.value };
		maxView.action = { controlSpec.maxval = maxView.value };
		warpView.action = { try { controlSpec.warp = warpView.value.interpret.asWarp(controlSpec) } };
		stepView.action = { controlSpec.step = stepView.value };

		//this.prSetViewParams;

	}
	
	prSubViewBounds{arg rect;  // calculate subview bounds
		var labelBounds, minBounds, maxBounds, warpBounds, stepBounds;
		var gap1, gap2, gap3, tmp, labelH, componentSize;
		gap1 = gap.copy;
		gap2 = gap.copy;
		gap3 = gap.copy;
		labelH=labelSize.y;//  needed for \vert

		switch (layout,
			\line2, {
				componentSize = ((rect.width - (3 * gap.x) - labelSize.x) / 4)@labelH;
				stepBounds = componentSize.asRect.left_(rect.width - componentSize.x);
				warpBounds = componentSize.asRect.left_(stepBounds.left-componentSize.x-gap.x);
				maxBounds = componentSize.asRect.left_(warpBounds.left-componentSize.x-gap.x);
				minBounds = componentSize.asRect.left_(maxBounds.left-componentSize.x-gap.x);
				labelBounds = (labelSize.x@labelSize.y).asRect.width_(minBounds.left-gap.x); //adjust width
			},

			 \vert, {
				componentSize = labelSize.x@((rect.height - (3 * gap.y) - labelH) / 4);
				labelBounds = (rect.width@labelH).asRect; // to top
				minBounds = (rect.width@componentSize.y)
					.asRect.top_(labelBounds.bottom + gap.y);
				maxBounds = (rect.width@componentSize.y)
					.asRect.top_(minBounds.bottom + gap.y);
				warpBounds = (rect.width@componentSize.y)
					.asRect.top_(maxBounds.bottom + gap.y);
				stepBounds = (rect.width@componentSize.y)
					.asRect.top_(warpBounds.bottom + gap.y);
			},

			 \horz, {
				componentSize = ((rect.width - (3 * gap.x) - labelSize.x) / 4)@labelH;
				stepBounds = componentSize.asRect.left_(rect.width - componentSize.x);
				warpBounds = componentSize.asRect.left_(stepBounds.left-componentSize.x-gap.x);
				maxBounds = componentSize.asRect.left_(warpBounds.left-componentSize.x-gap.x);
				minBounds = componentSize.asRect.left_(maxBounds.left-componentSize.x-gap.x);
				labelBounds = (labelSize.x@labelSize.y).asRect.width_(minBounds.left-gap.x); //adjust width
			 }
		);


		^[labelBounds, minBounds, maxBounds, warpBounds, stepBounds].collect{arg v; v.moveBy(margin.x,margin.y)}
	}
	
}