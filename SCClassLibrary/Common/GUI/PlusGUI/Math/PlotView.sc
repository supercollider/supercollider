Plot {
	classvar <initGuiSkin;

	var <>plotter;
	var <value, <bounds, <plotBounds, <>drawGrid;
	var <spec, <domainSpec;
	var <font, <fontColor, <gridColorX, <gridColorY, <plotColor, <>backgroundColor, <plotMode;
	var <labelX, <labelY, <labelFont, <labelFontColor;
	var <gridOnX = true, <gridOnY = true;
	var labelXIsUnits = false, labelYIsUnits = false;
	var <showUnits = true, <unitLocation = \axis; // \ticks or \axis
	var <>labelMargin = 2;  // margin around tick or axis labels
	var <>borderMargin = 3; // margin separating the edge of the view from its inner elements
	var <>hideLabelsHeightRatio = 1.2, <>hideLabelsWidthRatio = 1.5; // hide labels below plot:labels ratio
	var valueCache, resolution;

	*initClass {
		if(Platform.hasQt.not) { ^nil; };	// skip init on Qt-less builds

		initGuiSkin = {
			var palette = QtGUI.palette;
			var hl = palette.highlight;
			var base = palette.base;
			var baseText = palette.baseText;
			var butt = palette.button;
			var gridCol = palette.window;
			var labelCol = butt.blend(baseText, 0.6);

			GUI.skins.put(\plot, (
				gridColorX: gridCol,
				gridColorY: gridCol,
				fontColor: labelCol, // for tick labels
				labelFontColor: labelCol, // for axis labels
				plotColor: [baseText.blend(hl, 0.5)], // should be an array for multichannel support
				background: base,
				gridLinePattern: nil,
				gridLineSmoothing: false,
				labelX: nil, // axis label
				labelY: nil,
				expertMode: false,
				gridFont: Font( Font.defaultSansFace, 9 ), // for tick labels
				labelFont: Font( Font.defaultSansFace, 10 ) // for axis labels
			));
		};
		initGuiSkin.value;
		StartUp.add(this);
	}

	*doOnStartUp {
		initGuiSkin.value;
	}

	*new { |plotter|
		^super.newCopyArgs(plotter).init
	}

	init {
		var skin = GUI.skins.at(\plot);

		drawGrid = DrawGrid(bounds ? Rect(0,0,1,1),nil,nil);
		skin.use {
			font = ~gridFont ?? { Font( Font.defaultSansFace, 9 ) };
			labelFont = ~labelFont ?? { Font( Font.defaultSansFace, 10 ) };
			this.gridColorX = ~gridColorX;
			this.gridColorY = ~gridColorY;
			plotColor = ~plotColor;
			this.fontColor = ~fontColor;
			this.labelFontColor = ~labelFontColor;
			backgroundColor = ~background;
			this.gridLineSmoothing = ~gridLineSmoothing;
			this.gridLinePattern = ~gridLinePattern !? {~gridLinePattern.as(FloatArray)};
			labelX = ~labelX; // axis label
			labelY = ~labelY;
		};
		resolution = plotter.resolution;
	}

	bounds_ { |viewRect|
		var gridRect, sizeAllowsXLabels, sizeAllowsYLabels;

		valueCache = nil;
		bounds = viewRect;

		#gridRect, sizeAllowsXLabels, sizeAllowsYLabels = this.prCalcLabelSpace(viewRect);

		drawGrid.x.labelsHiddenBySize = sizeAllowsXLabels.not;
		drawGrid.y.labelsHiddenBySize = sizeAllowsYLabels.not;
		drawGrid.bounds = gridRect;
		plotBounds = gridRect;
	}

	// Calculate grid offset to accomodate tick and axis labels,
	// return modified gridRect and bools allowing X and Y labels.
	// Currently only supports tick labels that don't extend past
	// the side opposite their respective axis.
	// TODO: this method could be refactored to use the
	// DrawGridX/Y.showLabels instance vars independently to skip
	// corresponding calculations below when false.
	prCalcLabelSpace { |viewRect|
		var gridRect;
		var tkLHang, tkBHang;
		var xTkLHang, xTkTHang, xTkRHang, xTkBHang;
		var yTkLHang, yTkTHang, yTkRHang, yTkBHang;
		var tkBMargin, bottomMargin, totalBottomPad, totalTopPad, totalVertSpace;
		var tkLMargin, leftMargin, xLeftPad, xRightPad, leftPad, rightPad, totalHorizPad, addTopPad;

		var htAllowsXLabels = true, htAllowsYLabels = true;
		var wdAllowsXLabels = true, wdAllowsYLabels = true;
		var sizeAllowsXLabels = true, sizeAllowsYLabels = true;
		var xAxHt = 0, xAxMargin = 0, yAxWd = 0, yAxMargin = 0;

		gridRect = viewRect.insetBy(borderMargin); // default with no labels

		// Tick label overhang of Left, Top, Right, Bottom of gridRect
		// returns 0's if drawGrid.x/y.showLabels is false
		#xTkLHang, xTkTHang, xTkRHang, xTkBHang = drawGrid.x.labelOverhang.max(0);
		#yTkLHang, yTkTHang, yTkRHang, yTkBHang = drawGrid.y.labelOverhang.max(0);

		if(labelX.notNil) {
			xAxHt = labelX.bounds(labelFont).height;
			xAxMargin = labelMargin;
		};

		// First calculate vertical space needed for tick and axis labels
		// to determine if X labels will be hidden based on the view height.
		// (If so, Y labels that overhang vertically may need to also be hidden).

		tkBHang = max(xTkBHang, yTkBHang);
		// if there's label overhang, give it its margin
		tkBMargin = if(tkBHang > 0) { labelMargin } { 0 };
		// only the largest margin of all elements is needed
		bottomMargin = maxItem([xAxMargin, borderMargin, tkBMargin]);

		// add up all elements extending below the grid
		totalBottomPad = tkBHang + xAxMargin + xAxHt + bottomMargin;
		totalTopPad = (yTkTHang + labelMargin).max(borderMargin);
		totalVertSpace = totalTopPad + totalBottomPad;
		htAllowsXLabels = viewRect.height >= (totalVertSpace * (hideLabelsHeightRatio+1));
		sizeAllowsXLabels = htAllowsXLabels;

		// If x labels are still shown calculate horizontal space needed
		// to determine if they will be hidden based on the view width.
		if(htAllowsXLabels) {
			xLeftPad = if(xTkLHang > 0) { xTkLHang + labelMargin } { 0 };
			xRightPad = if(xTkRHang > 0) { xTkRHang + labelMargin } { 0 };
			totalHorizPad = xLeftPad.max(borderMargin) + xRightPad.max(borderMargin);
			wdAllowsXLabels = viewRect.width >= (totalHorizPad * (hideLabelsWidthRatio+1));
			sizeAllowsXLabels = wdAllowsXLabels;
			// if width is too small for x labels, it should be too small for Y labels too
			sizeAllowsYLabels = wdAllowsXLabels;
		};

		// update the grid size if X gets hidden
		if(sizeAllowsXLabels) {
			// show X labels, make space for them
			gridRect.height = viewRect.height - (borderMargin + totalBottomPad);
			gridRect.width = viewRect.width  - totalHorizPad;
			gridRect.left = max(xLeftPad, borderMargin);
		} {
			// if y tick label extends below bottom, they need to be turned off too.
			totalBottomPad = if(yTkBHang > 0) { yTkBHang + labelMargin } { 0 };
			htAllowsYLabels = totalBottomPad <= borderMargin;
			sizeAllowsYLabels = htAllowsYLabels;
		};

		// Y-axis label area
		if (sizeAllowsYLabels) {
			if(labelY.notNil) {
				yAxWd = labelY.bounds(labelFont).height;
				yAxMargin = labelMargin;
			};
			if(sizeAllowsXLabels.not) {
				xTkLHang = xTkRHang = 0;
			};

			// left side
			tkLHang = maxItem([xTkLHang, yTkLHang, 0]);
			tkLMargin = if(tkLHang > 0) { labelMargin } { 0 };
			// only the largest margin of all elements is needed
			leftMargin = maxItem([yAxMargin, borderMargin, tkLMargin]);
			leftPad = leftMargin + yAxWd + yAxMargin + tkLHang;

			rightPad = max(
				if(xTkRHang > 0) { xTkRHang + labelMargin } { 0 },
				borderMargin
			);

			totalHorizPad = leftPad + rightPad;
			wdAllowsYLabels = viewRect.width >= (totalHorizPad * (hideLabelsWidthRatio+1));

			sizeAllowsYLabels = wdAllowsYLabels and: { htAllowsYLabels };

			if(sizeAllowsYLabels) {
				gridRect.width = viewRect.width - (leftPad + rightPad);
				gridRect.left = leftPad;
				// add additional space to top border margin if y label extends past bound
				addTopPad = totalTopPad - borderMargin;
				if (addTopPad > 0) {
					gridRect.height = gridRect.height - addTopPad;
					gridRect.top = gridRect.top + addTopPad;
				};
			};
		};

		^[gridRect, sizeAllowsXLabels, sizeAllowsYLabels]
	}

	value_ { |array|
		value = array;
		valueCache = nil;
	}
	spec_ { |sp|
		spec = sp;
		if(gridOnY and: { spec.notNil }) {
			drawGrid.vertGrid = spec.grid;
			this.unitLocation_(this.unitLocation); // updates unit labels if enabled
		} {
			drawGrid.vertGrid = nil;
		};
	}
	domainSpec_ { |sp|
		domainSpec = sp;
		if(gridOnX and: { domainSpec.notNil }) {
			drawGrid.horzGrid = domainSpec.grid;
			this.unitLocation_(this.unitLocation); // updates unit labels if enabled
		} {
			drawGrid.horzGrid = nil;
		};
	}
	plotColor_ { |c|
		plotColor = c.as(Array);
	}
	gridColorX_ { |c|
		drawGrid.x.gridColor = c;
		gridColorX = c;
	}
	gridColorY_ { |c|
		drawGrid.y.gridColor = c;
		gridColorY = c;
	}
	plotMode_ { |m|
		plotMode = m.asArray
	}
	font_ { |f|
		font = f;
		drawGrid.font = f;
	}
	fontColor_ { |c|
		fontColor = c;
		drawGrid.fontColor = c;
	}
	labelFont_ { |f|
		labelFont = f;
	}
	labelFontColor_ { |c|
		labelFontColor = c;
	}
	labelX_ { |string|
		labelX = string;
		labelXIsUnits = false;
	}
	labelY_ { |string|
		labelY = string;
		labelYIsUnits = false;
	}
	gridLineSmoothing_ { |bool|
		drawGrid.smoothing = bool;
	}
	gridLinePattern_ { |pattern|
		drawGrid.linePattern = pattern;
	}
	gridOnX_ { |bool|
		gridOnX = bool;
		drawGrid.horzGrid = if(gridOnX,{domainSpec.grid},{nil});
	}
	gridOnY_ { |bool|
		gridOnY= bool;
		drawGrid.vertGrid = if(gridOnY,{spec.grid},{nil});
	}
	showUnits_ { |bool|
		showUnits = bool;
		if(showUnits) {
			// sets/updates labels when showUnits = true
			this.unitLocation_(this.unitLocation);
		} {
			drawGrid.x.labelsShowUnits = false;
			drawGrid.y.labelsShowUnits = false;
			if(labelXIsUnits) { this.labelX_(nil) };
			if(labelYIsUnits) { this.labelY_(nil) };
		};
	}
	unitLocation_ { |location|
		var xUnits, yUnits;
		switch(location,
			\ticks, {
				if(showUnits) {
					drawGrid.x.labelsShowUnits = true;
					drawGrid.y.labelsShowUnits = true;
				};
				if(labelXIsUnits) { this.labelX = nil };
				if(labelYIsUnits) { this.labelY = nil };
			},
			\axis, {
				drawGrid.x.labelsShowUnits = false;
				drawGrid.y.labelsShowUnits = false;

				xUnits = drawGrid.x.grid.spec.units;
				yUnits = drawGrid.y.grid.spec.units;

				if(showUnits) {
					// don't update with empty units or overwrite and explictly set label
					if(xUnits.isEmpty.not and: {
						this.labelX.isNil or: { labelXIsUnits }
					}) {
						this.labelX_(xUnits);
						labelXIsUnits = true;
					};

					if(yUnits.isEmpty.not and: {
						this.labelY.isNil or: { labelYIsUnits }
					}) {
						this.labelY_(yUnits);
						labelYIsUnits = true;
					};
				};
			}, {
				"Plot:-unitLocation should be \ticks or \axis".warn;
				^this
			}
		);
		unitLocation = location;
	}

	draw {
		this.drawBackground;
		drawGrid.draw;
		this.drawLabels;
		this.drawData;
		plotter.drawFunc.value(this); // additional elements
	}

	drawBackground {
		Pen.addRect(bounds);
		Pen.fillColor = backgroundColor;
		Pen.fill;
	}

	drawLabels {
		var lbounds, tklabelHang;

		if(labelX.notNil and: { gridOnX and: { drawGrid.x.labelsHiddenBySize.not } }) {
			try {
				lbounds = labelX.bounds(labelFont);
				tklabelHang = max(drawGrid.x.labelOverhang[3], 0);
				Pen.stringCenteredIn(labelX,
					lbounds.center_(
						plotBounds.center.x @ (plotBounds.bottom + tklabelHang + labelMargin + (lbounds.height/2))
					), labelFont, labelFontColor
				);
			};
		};

		if(labelY.notNil and: { gridOnY and: { drawGrid.y.labelsHiddenBySize.not } }) {
			try {
				lbounds = labelY.bounds(labelFont);
				tklabelHang = max(drawGrid.y.labelOverhang[0], 0);
				Pen.push;
				Pen.translate(plotBounds.left - tklabelHang - labelMargin - (lbounds.height/2), plotBounds.center.y);
				Pen.rotateDeg(-90);
				Pen.stringCenteredIn(labelY, lbounds.center_(0@0), labelFont, labelFontColor);
				Pen.pop;
			};
		};
	}

	domainCoordinates { |size|
		var range, vals;

		vals = if (plotter.domain.notNil) {
			domainSpec.unmap(plotter.domain);
		} {
			range = domainSpec.range;
			if (range == 0.0 or: { size == 1 }) {
				0.5.dup(size) // put the values in the middle of the plot
			} {
				domainSpec.unmap(
					Array.interpolation(size, domainSpec.minval, domainSpec.maxval)
				);
			}
		};

		^plotBounds.left + (vals * plotBounds.width);
	}

	dataCoordinates {
		var val = spec.warp.unmap(this.prResampValues);
		^plotBounds.bottom - (val * plotBounds.height); // measures from top left (may be arrays)
	}

	drawData {
		var ycoord = this.dataCoordinates;
		var xcoord = this.domainCoordinates(ycoord.size);

		Pen.use {
			Pen.width = 1.0;
			Pen.joinStyle = 1;
			Pen.addRect(plotBounds);
			Pen.clip; // clip curve to bounds.

			if(ycoord.at(0).isSequenceableCollection) { // multi channel expansion when superposed
				ycoord.flop.do { |y, i|
					var mode = plotMode.wrapAt(i);
					Pen.beginPath;
					this.perform(mode, xcoord, y);
					if (this.needsPenFill(mode)) {
						Pen.fillColor = plotColor.wrapAt(i);
						Pen.fill
					} {
						Pen.strokeColor = plotColor.wrapAt(i);
						Pen.stroke
					}
				}
			} {
				Pen.beginPath;
				Pen.strokeColor = plotColor.at(0);
				Pen.fillColor= plotColor.at(0);
				this.perform(plotMode.at(0), xcoord, ycoord);
				if (this.needsPenFill(plotMode.at(0))) {
					Pen.fill
				} {
					Pen.stroke
				}
			};
			Pen.joinStyle = 0;
		};
	}

	// modes

	linear { |x, y|
		Pen.moveTo(x.first @ y.first);
		y.size.do { |i|
			Pen.lineTo(x[i] @ y[i]);
		}
	}

	points { |x, y|
		var size = min(bounds.width / value.size * 0.25, 4);
		y.size.do { |i|
			Pen.addArc(x[i] @ y[i], 0.5, 0, 2pi);
			if(size > 2) { Pen.addArc(x[i] @ y[i], size, 0, 2pi); };
		}
	}

	plines { |x, y|
		var size = min(bounds.width / value.size * 0.25, 3);
		Pen.moveTo(x.first @ y.first);
		y.size.do { |i|
			var p = x[i] @ y[i];
			Pen.lineTo(p);
			Pen.addArc(p, size, 0, 2pi);
			Pen.moveTo(p);
		}
	}

	levels { |x, y|
		Pen.smoothing_(false);
		y.size.do { |i|
			Pen.moveTo(x[i] @ y[i]);
			Pen.lineTo(x[i + 1] ?? { plotBounds.right } @ y[i]);
		}
	}

	steps { |x, y|
		Pen.smoothing_(false);
		Pen.moveTo(x.first @ y.first);
		y.size.do { |i|
			Pen.lineTo(x[i] @ y[i]);
			Pen.lineTo(x[i + 1] ?? { plotBounds.right } @ y[i]);
		}
	}

	bars { |x, y |
		Pen.smoothing_(false);
		y.size.do { |i|
			var p = x[i] @ y[i];
			var nextx = x[i + 1] ?? {plotBounds.right};
			var centery = 0.linlin(this.spec.minval, this.spec.maxval, plotBounds.bottom, plotBounds.top, clip:nil);
			var rely = y[i] - centery;
			var gap = (nextx - x[i]) * 0.1;
			if (rely < 0) {
				Pen.addRect(Rect(x[i] + gap, centery + rely, nextx- x[i] - (2 * gap), rely.abs))
			} {
				Pen.addRect(Rect(x[i] + gap, centery, nextx - x[i] - ( 2 * gap), rely))
			}
		}
	}

	// editing

	editDataIndex { |index, x, y, plotIndex|
		// WARNING: assuming index is in range!
		var val = this.getRelativePositionY(y);
		plotter.editFunc.value(plotter, plotIndex, index, val, x, y);
		value.put(index, val);
		valueCache = nil;
	}

	editData { |x, y, plotIndex|
		var index = this.getIndex(x);
		this.editDataIndex( index, x, y, plotIndex );
	}

	editDataLine { |pt1, pt2, plotIndex|
		var ptLeft, ptRight, ptLo, ptHi;
		var xSpec, ySpec;
		var i1, i2, iLo, iHi;
		var val;

		// get indexes related to ends of the line
		i1 = this.getIndex(pt1.x);
		i2 = this.getIndex(pt2.x);

		// if both ends at same index, simplify
		if( i1 == i2 ) {
			^this.editDataIndex( i2, pt2.x, pt2.y, plotIndex );
		};

		// order points and indexes
		if( i1 < i2 ) {
			iLo = i1; iHi = i2;
			ptLeft = pt1; ptRight = pt2;
		}{
			iLo = i2; iHi = i1;
			ptLeft = pt2; ptRight = pt1;
		};

		// if same value all over, simplify
		if( ptLeft.y == ptRight.y ) {
			val = this.getRelativePositionY(ptLeft.y);
			while( {iLo <= iHi} ) {
				value.put( iLo, val );
				iLo = iLo + 1;
			};
			// trigger once for second end of the line
			plotter.editFunc.value(plotter, plotIndex, i2, val, pt2.x, pt2.y);
			valueCache = nil;
			^this;
		};

		// get actual points corresponding to indexes
		xSpec = ControlSpec( ptLeft.x, ptRight.x );
		ySpec = ControlSpec( ptLeft.y, ptRight.y );
		ptLo = Point();
		ptHi = Point();
		ptLo.x = domainSpec.unmap(iLo) * plotBounds.width + plotBounds.left;
		ptHi.x = domainSpec.unmap(iHi) * plotBounds.width + plotBounds.left;
		ptLo.y = ySpec.map( xSpec.unmap(ptLo.x) );
		ptHi.y = ySpec.map( xSpec.unmap(ptHi.x) );

		// interpolate and store
		ySpec = ControlSpec( this.getRelativePositionY(ptLo.y), this.getRelativePositionY(ptHi.y) );
		xSpec = ControlSpec( iLo, iHi );
		while( {iLo <= iHi} ) {
			val = ySpec.map( xSpec.unmap(iLo) );
			value.put( iLo, val );
			iLo = iLo+1;
		};

		// trigger once for second end of the line
		plotter.editFunc.value(plotter, plotIndex, i2, val, pt2.x, pt2.y);
		valueCache = nil;
	}

	getRelativePositionX { |x|
		^domainSpec.map((x - plotBounds.left) / plotBounds.width)
	}

	getRelativePositionY { |y|
		^spec.map((plotBounds.bottom - y) / plotBounds.height)
	}

	hasSteplikeDisplay {
		^#[\levels, \steps, \bars].includesAny(plotMode)
	}

	needsPenFill { |pMode|
		^#[\bars].includes(pMode)
	}

	getIndex { |x|
		var ycoord = this.dataCoordinates;
		var xcoord = this.domainCoordinates(ycoord.size);
		var binwidth = 0;
		var offset;

		if (plotter.domain.notNil) {
			if (this.hasSteplikeDisplay) {
				// round down to index
				^plotter.domain.indexInBetween(this.getRelativePositionX(x)).floor.asInteger
			} {
				// round to nearest index
				^plotter.domain.indexIn(this.getRelativePositionX(x))
			};
		} {
			if (xcoord.size > 0) {
				binwidth = (xcoord[1] ?? {plotBounds.right}) - xcoord[0]
			};
			offset = if(this.hasSteplikeDisplay) { binwidth * 0.5 } { 0.0 };

			^(  // domain unspecified, values are evenly distributed between either side of the plot
				((x - offset - plotBounds.left) / plotBounds.width) * (value.size - 1)
			).round.clip(0, value.size-1).asInteger
		}
	}

	getDataPoint { |x|
		var index = this.getIndex(x).clip(0, value.size - 1);
		^[index, value.at(index)]
	}

	// settings

	zoomFont { |val|
		font = font.copy;
		font.size = max(1, font.size + val);
		this.font = font;
		labelFont = labelFont.copy;
		labelFont.size = max(1, labelFont.size + val);
		this.labelFont = labelFont;
		drawGrid.clearCache;
	}
	copy {
		^super.copy.drawGrid_(drawGrid.copy)
	}

	prResampValues {
		var dataRes, numSpecSteps, specStep, sizem1;

		dataRes = plotBounds.width / max((value.size - 1), 1);

		^if (
			(dataRes >= plotter.resolution) or:
			{ plotter.domain.notNil } // don't resample if domain is specified
		) {
			value
		} {
			// resample
			if (valueCache.isNil or: { resolution != plotter.resolution }) {
				resolution = plotter.resolution;

				// domain is nil, so data fills full domain/view width
				numSpecSteps = (plotBounds.width / resolution).floor.asInteger;
				specStep = numSpecSteps.reciprocal;
				sizem1 = value.size - 1;

				valueCache = (numSpecSteps + 1).collect{ |i|
					value.blendAt((specStep * i) * sizem1)  // float index of new value
				}
			} {
				valueCache
			}
		}
	}
}


Plotter {

	var <>name;
	var <>bounds, <>parent;
	var <value, <data, <domain;
	var <plots, <specs, <domainSpecs, plotColor;
	var <cursorPos, plotMode, <>editMode = false, <>normalized = false;
	var <>resolution = 1, <>findSpecs = true, <superpose = false;
	var modes, <interactionView;
	var <editPlotIndex, <editPos;
	var <>drawFunc, <>editFunc;
	var <showUnits = true, <unitLocation = \axis; // \ticks or \axis
	var axisLabelX, axisLabelY;

	*new { |name, bounds, parent|
		^super.newCopyArgs(name).makeWindow(parent, bounds)
	}

	makeWindow { |argParent, argBounds|
		parent = argParent ? parent;
		bounds = argBounds ? bounds;

		if(parent.isNil) {
			parent = Window.new(name ? "Plot", bounds ? Rect(100, 200, 400, 300));
			bounds = parent.view.bounds.insetBy(5);
			parent.background_(QtGUI.palette.window);
			interactionView = UserView.new(parent, bounds);

			interactionView.drawFunc = { this.draw };
			parent.front;
			parent.onClose = { parent = nil };
		} {
			bounds = bounds ?? { parent.bounds.moveTo(0, 0) };
			interactionView = UserView.new(parent, bounds);
			interactionView.drawFunc = { this.draw };
		};
		this.prSetUpInteractionView;

		modes = [\points, \levels, \linear, \plines, \steps, \bars].iter.loop;
		this.plotMode = \linear;
		this.plotColor = GUI.skins.plot.plotColor;
		// at this point no values are set, so no Plots have been created
	}

	prSetUpInteractionView {
		// set up interactionView
		interactionView
		.background_(Color.clear)
		.focusColor_(Color.clear)
		.resize_(5)
		.focus(true)
		.mouseDownAction_({ |v, x, y, modifiers|
			cursorPos = x @ y;
			if(superpose.not) {
				editPlotIndex = this.pointIsInWhichPlot(cursorPos);
				editPlotIndex !? {
					editPos = x @ y; // new Point instead of cursorPos!
					if(editMode) {
						plots.at(editPlotIndex).editData(x, y, editPlotIndex);
						if(this.numFrames < 200) { this.refresh };
					};
				}
			};
			if(modifiers.isAlt) { this.postCurrentValue(x, y) };
		})
		.mouseMoveAction_({ |v, x, y, modifiers|
			cursorPos = x @ y;
			if(superpose.not && editPlotIndex.notNil) {
				if(editMode) {
					plots.at(editPlotIndex).editDataLine(editPos, cursorPos, editPlotIndex);
					if(this.numFrames < 200) { this.refresh };
				};
				editPos = x @ y;  // new Point instead of cursorPos!
			};
			if(modifiers.isAlt) { this.postCurrentValue(x, y) };
		})
		.mouseUpAction_({
			cursorPos = nil;
			editPlotIndex = nil;
			if(editMode && superpose.not) { this.refresh };
		})
		.keyDownAction_({ |view, char, modifiers, unicode, keycode|
			if(modifiers.isCmd.not) {
				switch(char,
					// y zoom out / font zoom
					$-, {
						if(modifiers.isCtrl) {
							plots.do(_.zoomFont(-2));
						} {
							this.specs = specs.collect(_.zoom(7/5));
							normalized = false;
						}
					},
					// y zoom in / font zoom
					$+, {
						if(modifiers.isCtrl) {
							plots.do(_.zoomFont(2));
						} {
							this.specs = specs.collect(_.zoom(5/7));
							normalized = false;
						}
					},
					// compare plots
					$=, {
						this.calcSpecs(separately: false);
						this.updatePlotSpecs;
						normalized = false;
					},

					/*// x zoom out (doesn't work yet)
					$*, {
					this.domainSpecs = domainSpecs.collect(_.zoom(3/2));
					},
					// x zoom in (doesn't work yet)
					$_, {
					this.domainSpecs = domainSpecs.collect(_.zoom(2/3))
					},*/

					// normalize
					$n, {
						if(normalized) {
							this.specs = specs.collect(_.normalize)
						} {
							this.calcSpecs;
							this.updatePlotSpecs;
						};
						normalized = normalized.not;
					},
					// toggle grid
					$g, {
						plots.do { |x| x.gridOnY = x.gridOnY.not }
					},
					// toggle domain grid
					$G, {
						plots.do { |x| x.gridOnX = x.gridOnX.not };
					},
					// toggle plot mode
					$m, {
						this.plotMode = modes.next;
					},
					// toggle editing
					$e, {
						editMode = editMode.not;
						"plot edit mode %\n".postf(if(editMode) { "on" } { "off" });
					},
					// toggle superposition
					$s, {
						this.superpose = this.superpose.not;
					},
					// print
					$p, {
						this.print
					}
				);
				parent.refresh;
			};
		}) // end keyDownAction_
		;
	}

	value_ { |arrays|
		this.setValue(arrays, findSpecs, true)
	}

	setValue { |arrays, findSpecs = true, refresh = true, separately = true, minval, maxval, defaultRange|
		value = arrays;
		domain = nil;  // for now require user to re-set domain after setting new value
		data = this.prReshape(arrays);
		if(findSpecs) {
			this.calcDomainSpecs;
			this.calcSpecs(separately, minval, maxval, defaultRange);
		};
		this.updatePlots;
		this.updatePlotBounds;
		if(refresh) { this.refresh };
	}

	// TODO: currently domain is constrained to being identical across all channels
	// domain values are (un)mapped within the domainSpec
	domain_ { |domainArray|
		var dataSize, sizes;

		domainArray ?? {
			domain = nil;
			^this
		};

		dataSize = if (this.value.rank > 1) {
			sizes = this.value.collect(_.size);
			if (sizes.any(_ != this.value[0].size)) {
				Error(format(
					"[Plotter:-domain_] Setting the domain values isn't supported "
					"for multichannel data of different sizes %.", sizes
				)).throw;
			};
			this.value[0].size;
		} {
			this.value.size
		};


		if (domainArray.size != dataSize) {
			Error(format(
				"[Plotter:-domain_] Domain array size [%] does not "
				"match data array size [%]", domainArray.size, dataSize
			)).throw;
		} {
			domain = domainArray
		}
	}

	superpose_ { |flag|
		var dom, domSpecs;
		dom = domain.copy;
		domSpecs = domainSpecs.copy;

		superpose = flag;
		if ( value.notNil ){
			this.setValue(value, false, false);
		};

		// for individual plots, restore previous domain state
		this.domain_(dom);
		if (superpose.not) {
			this.domainSpecs_(domSpecs);
		};
		this.axisLabelX !? { this.axisLabelX_(this.axisLabelX) };
		this.axisLabelY !? { this.axisLabelY_(this.axisLabelY) };
		this.refresh;
	}

	numChannels {
		^value.size
	}

	numFrames {
		if(value.isNil) { ^0 };
		^value.first.size
	}

	draw {
		var b = this.interactionView.bounds;
		if(b != bounds) {
			bounds = b;
			this.updatePlotBounds;
		};
		Pen.use {
			plots.do { |plot| plot.draw };
		}
	}

	drawBounds {
		^bounds.moveTo(0, 0);
	}

	print {
		var printer = QPenPrinter.new;
		printer.showDialog {
			printer.print { this.draw }
		} { "printing canceled".postln }
	}

	// subviews

	updatePlotBounds {
		var bounds = this.drawBounds;
		var deltaY = if(data.size > 1 ) { 4.0 } { 0.0 };
		var distY = bounds.height / data.size;
		var height = distY - deltaY;

		plots.do { |plot, i|
			plot.bounds_(
				Rect(bounds.left, distY * i + bounds.top, bounds.width, height)
			);
		};
		this.refresh;
	}

	makePlots {
		var template = if(plots.isNil) { Plot(this) } { plots.last };
		plots !? { plots = plots.keep(data.size.neg) };
		plots = plots ++ template.dup(data.size - plots.size);
		plots.do { |plot, i| plot.value = data.at(i) };
		this.plotColor_(plotColor);
		this.plotMode_(plotMode);
		this.updatePlotSpecs;
	}

	updatePlots {
		if(plots.size != data.size) {
			this.makePlots;
		} {
			plots.do { |plot, i|
				plot.value = data.at(i)
			}
		};
	}

	updatePlotSpecs {
		var template, smin, smax;

		specs !? {
			if (superpose) {
				// NOTE: for superpose, all spec properties except
				// minval and maxval are inherited from first spec
				template = specs[0].copy;
				smin = specs.collect(_.minval).minItem;
				smax = specs.collect(_.maxval).maxItem;
				plots[0].spec = template.minval_(smin).maxval_(smax);
			} {
				plots.do { |plot, i|
					plot.spec = specs.clipAt(i)
				}
			}
		};

		domainSpecs !? {
			if (superpose) {
				template = domainSpecs[0].copy;
				smin = domainSpecs.collect(_.minval).minItem;
				smax = domainSpecs.collect(_.maxval).maxItem;
				plots[0].domainSpec = template.minval_(smin).maxval_(smax);
			} {
				plots.do { |plot, i|
					plot.domainSpec = domainSpecs.clipAt(i)
				}
			}
		};

		this.updatePlotBounds;
	}

	setProperties { |... pairs|
		pairs.pairsDo { |selector, value|
			selector = selector.asSetter;
			plots.do { |x| x.perform(selector, value) }
		};
		this.updatePlotBounds;
	}

	plotColor_ { |colors|
		plotColor = colors.as(Array);
		plots.do { |plot, i|
			// rotate colors to ensure proper behavior with superpose
			// (so this Plot's color is first in its color array)
			plot.plotColor_(plotColor.rotate(i.neg))
		}
	}

	plotColor {
		var first = plotColor.first;
		^if (plotColor.every(_ == first)) { first } { plotColor };
	}

	plotMode_ { |modes|
		plotMode = modes.asArray;
		plots.do { |plot, i|
			// rotate to ensure proper behavior with superpose
			plot.plotMode_(plotMode.rotate(i.neg))
		}
	}

	plotMode {
		var first = plotMode.first;
		^if (plotMode.every(_ == first)) { first } { plotMode };
	}

	axisLabelX_ { |labels|
		// need to store as Plotter inst var to restore from array after superpose
		axisLabelX = if(labels.isKindOf(Array)) {
			labels.collect(_ !? (_.asString))
		} { [ labels !? (_.asString) ] };
		plots.do { |plot, i|
			// rotate to ensure proper behavior with superpose
			plot.labelX_(axisLabelX.rotate(i.neg)[0])
		};
		this.updatePlotBounds;
	}

	axisLabelX {
		^axisLabelX !? {
			if (axisLabelX.every(_ == axisLabelX.first)) { axisLabelX.first } { axisLabelX };
		};
	}

	axisLabelY_ { |labels|
		axisLabelY = if(labels.isKindOf(Array)) {
			labels.collect(_ !? (_.asString))
		} { [ labels !? (_.asString) ] };
		plots.do { |plot, i|
			// rotate to ensure proper behavior with superpose
			plot.labelY_(axisLabelY.rotate(i.neg)[0])
		};
		this.updatePlotBounds;
	}

	axisLabelY {
		^axisLabelY !? {
			if (axisLabelY.every(_ == axisLabelY.first)) { axisLabelY.first } { axisLabelY };
		};
	}

	showUnits_ { |bool|
		showUnits = bool;
		this.setProperties(\showUnits, bool);
	}

	unitLocation_ { |location|
		if(location == \ticks or: { location == \axis }) {
			this.setProperties(\unitLocation, location);
			unitLocation = location;
		} {
			"Plot:-unitLocation should be \ticks or \axis".warn;
		};
	}

	// grids

	// Assume all subplot grids have the same properties for a given axis
	getGridProperty { |axis, property|
		^plots[0].drawGrid.perform(axis).perform(property)
	}

	setGridProperties { |axis ... propertyPairs|
		var drawGrid;
		plots.do { |plot|
			drawGrid = plot.drawGrid.perform(axis);
			drawGrid.clearCache;
			propertyPairs.pairsDo { |property, value|
				drawGrid.perform(property.asSetter, value)
			};
		};
		this.updatePlotBounds;
	}

	// Convenience methods for setting grid axes' properties together
	drawGridBoundingRect_ { |bool|
		if(bool) {
			// bounding lines are mutually exclusive - setting one to true sets others to false.
			[\x, \y].do{ |ax| this.setGridProperties(ax, \drawBoundingRect, true) };
		} { // false implies no bounding lines
			[\x, \y].do{ |ax|
				this.setGridProperties(ax,
					\drawBoundingRect, false, \drawBoundingLines, false, \drawBaseLine, false
				);
			};
		}
	}
	drawGridBaseLines_ { |bool|
		if(bool) {
			// bounding lines are mutually exclusive - setting one to true sets others to false.
			[\x, \y].do{ |ax| this.setGridProperties(ax, \drawBaseLine, true) };
		} { // false implies no bounding lines
			[\x, \y].do{ |ax|
				this.setGridProperties(ax,
					\drawBoundingRect, false, \drawBoundingLines, false, \drawBaseLine, false
				);
			};
		}
	}

	// specs

	specs_ { |argSpecs|
		specs = argSpecs.asArray.clipExtend(data.size).collect(_.asSpec);
		this.updatePlotSpecs;
	}

	domainSpecs_ { |argSpecs|
		domainSpecs = argSpecs.asArray.clipExtend(data.size).collect(_.asSpec);
		this.updatePlotSpecs;
	}

	minval_ { |val|
		val = val.asArray;
		specs.do { |x, i| x.minval = val.wrapAt(i) };
		this.updatePlotSpecs;
	}

	maxval_ { |val|
		val = val.asArray;
		specs.do { |x, i| x.maxval = val.wrapAt(i) };
		this.updatePlotSpecs;
	}


	calcSpecs { |separately = true, minval, maxval, defaultRange|
		var ranges = [minval, maxval].flop;
		var newSpecs = ranges.collect(_.asSpec).clipExtend(data.size);
		if(separately) {
			newSpecs = newSpecs.collect { |spec, i|
				var list = data.at(i);
				if(list.notNil) {
					spec = spec.looseRange(list, defaultRange, *ranges.wrapAt(i));
				} {
					spec
				};
			}
		} {
			newSpecs = newSpecs.first.looseRange(data.flat, defaultRange, *ranges.at(0));
		};
		this.specs = newSpecs;
	}

	calcDomainSpecs {
		// for now, a simple version
		domainSpecs = data.collect { |val|
			[0, val.size - 1, \lin].asSpec
		}
	}


	// interaction
	pointIsInWhichPlot { |point|
		var res;
		if(plots.isNil) { ^nil };
		res = plots.detectIndex { |plot|
			point.y.exclusivelyBetween(plot.bounds.top, plot.bounds.bottom)
		};
		^res ?? {
			if(point.y < bounds.center.y) { 0 } { plots.size - 1 }
		}
	}

	getDataPoint { |x, y|
		var plotIndex = this.pointIsInWhichPlot(x @ y);
		^plotIndex !? {
			plots.at(plotIndex).getDataPoint(x)
		}
	}

	postCurrentValue { |x, y|
		this.getDataPoint(x, y).postln
	}

	editData { |x, y|
		var plotIndex = this.pointIsInWhichPlot(x @ y);
		plotIndex !? {
			plots.at(plotIndex).editData(x, y, plotIndex);
		};
	}

	refresh {
		parent !? { parent.refresh }
	}

	prReshape { |item|
		var size, array = item.asArray;
		if(item.first.isSequenceableCollection.not) {
			^array.bubble;
		};
		if(superpose) {
			if(array.first.first.isSequenceableCollection) { ^array };
			size = array.maxItem { |x| x.size }.size;
			// for now, just extend data:
			^array.collect { |x| x.asArray.clipExtend(size) }.flop.bubble };
		^array
	}
}


+ ArrayedCollection {
	plot { |name, bounds, discrete = false, numChannels, minval, maxval, separately = true|
		var array, plotter;
		array = this.as(Array);

		if(array.maxDepth > 3) {
			"Cannot currently plot an array with more than 3 dimensions".warn;
			^nil
		};
		plotter = Plotter(name, bounds);
		if(discrete) { plotter.plotMode = \points };

		numChannels !? { array = array.unlace(numChannels) };
		array = array.collect {|elem, i|
			if (elem.isKindOf(Env)) {
				elem.asMultichannelSignal.flop
			} {
				if(elem.isNil) {
					Error("Cannot plot array: non-numeric value at index %".format(i)).throw
				};
				elem
			}
		};

		plotter.setValue(
			array,
			findSpecs: true,
			refresh: true,
			separately: separately,
			minval: minval,
			maxval: maxval
		);

		^plotter
	}
}

+ Collection {
	plotHisto { arg steps = 100, min, max;
		var histo = this.histo(steps, min, max);
		var plotter = histo.plot;
		var minmax = [min ?? { this.minItem }, max ?? { this.maxItem }];
		var binwidth = minmax[1] - minmax[0] / steps;

		^plotter
		.domainSpecs_([minmax.asSpec])
		.specs_([[0, histo.maxItem * 1.05, \linear, 1].asSpec])
		.plotMode_(\steps)
		.axisLabelY_("Occurrences")
		.axisLabelX_("Bins")
		.domain_(binwidth * (0..steps-1) + minmax[0])
		;
	}
}


+ Function {
	plot { |duration = 0.01, target, bounds, minval, maxval, separately = false|
		var server, plotter, action;
		var name = this.asCompileString;

		if(name.size > 50 or: { name.includes(Char.nl) }) { name = "Function" };

		plotter = Plotter(name, bounds);
		// init data in case function data is delayed (e.g. server booting)
		plotter.value = [0.0];

		target = target.asTarget;
		server = target.server;
		action = { |array, buf|
			var numChan = buf.numChannels;
			var numFrames = buf.numFrames;
			var frameDur;

			defer {
				plotter.setValue(
					array.unlace(numChan),
					findSpecs: true,
					refresh: false,
					separately: separately,
					minval: minval,
					maxval: maxval
				);

				plotter.domainSpecs = ControlSpec(0, duration, units: "sec");

				// If individual values might be separated by at least 2.5 px
				// (based on a plot at full screen width), set the x values (domain)
				// explicitly for accurate time alignment with grid lines.
				if(numFrames < (Window.screenBounds.width / 2.5)) {
					frameDur = if(this.value.rate == \control) {
						server.options.blockSize / server.sampleRate
					} {
						1 / server.sampleRate
					};
					plotter.domain = numFrames.collect(_ * frameDur);
				};
				// save vertical space with highly multichannel plots
				if(numChan > 4) { plotter.axisLabelX_(nil) };
			};
		};

		if(server.isLocal) {
			this.loadToFloatArray(duration, target, action: action)
		} {
			this.getToFloatArray(duration, target, action: action)
		};

		^plotter
	}

	plotAudio { |duration = 0.01, minval = -1, maxval = 1, server, bounds|
		^this.plot(duration, server, bounds, minval, maxval)
	}
}

+ Bus {
	plot { |duration = 0.01, bounds, minval, maxval, separately = false|
		if (this.rate == \audio, {
			^{ InFeedback.ar(this.index, this.numChannels) }.plot(duration, this.server, bounds, minval, maxval, separately);
		},{
			^{ In.kr(this.index, this.numChannels) }.plot(duration, this.server, bounds, minval, maxval, separately);
		});
	}

	plotAudio { |duration = 0.01, minval = -1, maxval = 1, bounds|
		^this.plot(duration, bounds, minval, maxval)
	}
}

+ Wavetable {
	plot { |name, bounds, minval, maxval|
		^this.asSignal.plot(name, bounds, minval: minval, maxval: maxval)
	}
}

+ Buffer {
	plot { |name, bounds, minval, maxval, separately = false|
		var plotter, action;
		if(server.serverRunning.not) { "Server % not running".format(server).warn; ^nil };
		if(numFrames.isNil) { "Buffer not allocated, can't plot data".warn; ^nil };

		plotter = [0].plot(
			name ? "Buffer plot (bufnum: %)".format(this.bufnum),
			bounds, minval: minval, maxval: maxval
		);

		action = { |array, buf|
			var unitStr = if (buf.numChannels == 1) { "samples" } { "frames" };
			{
				plotter.setValue(
					array.unlace(buf.numChannels),
					findSpecs: true,
					refresh: false,
					separately: separately,
					minval: minval,
					maxval: maxval
				);
				plotter.domainSpecs = ControlSpec(0.0, buf.numFrames, units: unitStr);
				// save vertical space with highly multichannel buffer plots
				if(buf.numChannels > 4) { plotter.axisLabelX_(nil) };
			}.defer
		};

		if(server.isLocal) {
			this.loadToFloatArray(action:action)
		} {
			this.getToFloatArray(action:action)
		};

		^plotter
	}
}


+ Env {
	plot { |size = 400, bounds, minval, maxval, name|
		var plotLabel = if (name.isNil) { "Envelope" } { name };
		var plotter = [this.asMultichannelSignal(size).flop]
		.plot(name, bounds, minval: minval, maxval: maxval);

		var duration = this.duration.asArray;
		var channelCount = duration.size;

		var totalDuration = if (channelCount == 1) { duration } { duration.maxItem ! channelCount };

		plotter.domainSpecs = totalDuration.collect(ControlSpec(0, _));
		plotter.refresh;
		^plotter
	}
}

+ AbstractFunction {
	plotGraph { arg n=500, from = 0.0, to = 1.0, name, bounds, discrete = false,
		numChannels, minval, maxval, separately = true;
		var array = Array.interpolation(n, from, to);
		var res = array.collect { |x| this.value(x) };
		^res.plot(name, bounds, discrete, numChannels, minval, maxval, separately)
	}
}
