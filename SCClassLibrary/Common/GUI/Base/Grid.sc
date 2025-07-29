DrawGrid {

	var <bounds, <>x, <>y;
	var <>opacity=0.7, <>smoothing=false, <>linePattern;
	var uview; // private, only persists while -preview window is open

	*new { |bounds, horzGrid, vertGrid|
		^super.new.init(bounds, horzGrid, vertGrid)
	}
	init { arg bounds, h, v;
		x = DrawGridX(h, bounds.asRect);
		y = DrawGridY(v, bounds.asRect);
		this.bounds = bounds;
		this.font = Font(Font.defaultSansFace, 9);
		this.fontColor = Color.grey(0.3);
		this.gridColors = [Color.grey(0.7), Color.grey(0.7)];
	}
	bounds_ { arg b;
		bounds = b.asRect;
		x.bounds = bounds;
		y.bounds = bounds;
	}
	draw {
		Pen.push;
		Pen.alpha = opacity;
		Pen.smoothing = smoothing;
		if(linePattern.notNil) {Pen.lineDash_(linePattern)};
		x.commands.do({ arg cmd; Pen.perform(cmd) });
		y.commands.do({ arg cmd; Pen.perform(cmd) });
		Pen.pop;
	}
	font_ { arg f;
		x.font = f;
		y.font = f;
	}
	fontColor_ { arg c;
		x.fontColor = c;
		y.fontColor = c;
	}
	gridColors_ { arg colors;
		colors = colors.as(Array);
		x.gridColor = colors.wrapAt(0);
		y.gridColor = colors.wrapAt(1);
	}
	horzGrid_ { arg g;
		x.grid = g;
	}
	vertGrid_ { arg g;
		y.grid = g;
	}
	tickSpacing_ { arg x, y;
		this.x.tickSpacing = x;
		this.y.tickSpacing = y;
	}
	numTicks_ { arg x, y;
		this.x.numTicks = x;
		this.y.numTicks = y;
	}
	copy {
		^DrawGrid(bounds,x.grid,y.grid).x_(x.copy).y_(y.copy).opacity_(opacity).smoothing_(smoothing).linePattern_(linePattern)
	}
	clearCache {
		x.clearCache;
		y.clearCache;
	}

	// make a Window with a UserView that draws this DrawGrid
	preview {
		var insetH = 30, insetV = 15; // left, bottom margins for labels
		var gridPad = 15;             // right, top margin
		var win, winBounds, font, fcolor;

		// refresh and return the view if it already exists
		uview !? {
			uview.refresh;
			try { uview.parent.findWindow.front };
			^uview
		};

		insetH = insetH + gridPad;
		insetV = insetV + gridPad;
		fcolor = Color.grey(0.3);               // match this.fontColor
		font = Font( Font.defaultSansFace, 9 ); // match this.font

		// bounds of the grid lines, without its labels
		this.bounds = this.bounds ?? { Rect(0, 0, 500, 400) };
		// translate the grid to make room for tick labels
		this.bounds = this.bounds.moveTo(insetH-gridPad, gridPad);

		winBounds = this.bounds + Size(insetH, insetV);
		win = Window("DrawGrid test",
			winBounds.center_(Window.screenBounds.center)
		).front;

		// the view that draws the DrawGrids
		uview = UserView(
			win, winBounds.size.asRect
		)
		.drawFunc_({ |uv|
			this.draw;
		})
		.onResize_({ |uv|
			this.bounds = uv.bounds
			.insetBy(insetH.half, insetV.half)
			.moveTo(insetH - gridPad, gridPad);
		})
		.resize_(5)
		.background_(Color.white)
		.onClose_({ uview = nil })
		;

		^uview
	}
}


DrawGridX {

	var <grid,<>bounds;
	var <>range;
	var <font,<fontColor,<gridColor;
	var <labelOffset, <labelAlign, <labelAnchor, <constrainLabelExtents;
	var commands, cacheKey, lastLabelSize;
	var <tickSpacing = 64;
	var <numTicks = nil; // nil for dynamic numTicks with view size
	var <>showLabels = true; // tick labels
	var <>labelsHiddenBySize = false;
	var <labelAppendString = nil, <labelsShowUnits = false;
	var <drawBoundingRect = true, <drawBaseLine = false, <drawBoundingLines = false;


	*new { arg grid, bounds;
		^super.newCopyArgs(grid.asGrid, bounds).init
	}

	init {
		range = [grid.spec.minval, grid.spec.maxval];
		labelAnchor = \top;
		labelOffset = 0 @ 2;
		labelAlign = \center;
		constrainLabelExtents = false;
		font = Font(Font.defaultSansFace, 9);
		fontColor = Color.grey(0.3);
		gridColor = Color.grey(0.7);
		lastLabelSize = "000".bounds(font).asSize;
	}
	grid_ { arg g;
		grid = g.asGrid;
		range = [grid.spec.minval, grid.spec.maxval];
		this.clearCache;
	}
	setZoom { arg min,max;
		range = [min, max];
	}
	tickSpacing_ { |px|
		px !? {
			tickSpacing = px;
			this.clearCache;
		};
	}
	numTicks_ { |num|
		numTicks = num;
		this.clearCache;
	}
	font_ { |afont|
		font = afont;
		this.clearCache;
	}
	fontColor_ { |color|
		fontColor = color;
		this.clearCache;
	}
	gridColor_ { |color|
		gridColor = color;
		this.clearCache;
	}
	labelOffset_ { |pnt|
		try {
			labelOffset = pnt.asPoint;
		} {
			Error("[DrawGridX/Y:labelOffset_] "
				"pt argument needs to be able to respond to the .asPoint method, "
				"e.g. a Point, Size, Rect, or SimpleNumber.").throw
		};
		this.clearCache;
	}
	labelAlign_ { |align|
		var alignSym = align.asSymbol;
		var alignList = #[\center, \left, \right];

		if(alignList.includes(alignSym)) {
			labelAlign = alignSym
		} {
			warn("[DrawGridX/Y:labelAlign_] "
				"Invalid align argument, options are: %".format(alignList))
		};
		this.clearCache;
	}
	labelAnchor_ { |anchor|
		var anchorSym = anchor.asSymbol;
		var anchorList = #[ // these need to match keys in Rect:-anchorTo
			\center, \top, \bottom, \left, \right,
			\topLeft, \topRight, \bottomLeft, \bottomRight,
			\leftTop, \rightTop, \leftBottom, \rightBottom,
		];

		if(anchorList.includes(anchorSym)) {
			labelAnchor = anchorSym
		} {
			warn("[DrawGridX/Y:labelAnchor_] "
				"Invalid anchor argument, options are: %".format(anchorList))
		};
		this.clearCache;
	}
	labelAppendString_ { |str|
		this.grid.appendLabel_(str);
		labelAppendString = str;
		labelsShowUnits = false;
		this.clearCache;
	}
	labelsShowUnits_ { |bool|
		var specUnits;
		if (bool) {
			specUnits = grid.spec.units;
			if(specUnits.isEmpty.not and: {
				labelsShowUnits or: { this.labelAppendString.isNil }
			}) {
				this.labelAppendString = " " ++ specUnits;
				labelsShowUnits = true;
			};
		} {
			if(labelsShowUnits) {
				this.labelAppendString = nil;
				labelsShowUnits = false;
			}
		};
	}
	constrainLabelExtents_ { |bool|
		constrainLabelExtents = bool;
		this.clearCache;
	}
	drawBaseLine_ { |bool|
		drawBaseLine = bool;
		if(bool) {
			drawBoundingLines = false;
			drawBoundingRect = false;
		};
		this.clearCache;
	}
	drawBoundingLines_ { |bool|
		drawBoundingLines = bool;
		if(bool) {
			drawBaseLine = false;
			drawBoundingRect = false;
		};
		this.clearCache;
	}
	drawBoundingRect_ { |bool|
		drawBoundingRect = bool;
		if(bool) {
			drawBoundingLines = false;
			drawBaseLine = false;
		};
		this.clearCache;
	}

	commands {
		var p, labelSz, valNorm, lineColor;
		var x, labelRect, anchorPoint, alignSym;
		var rightBound = bounds.right.asInteger;
		var leftBound = bounds.left.asInteger;
		var bottomBound = bounds.bottom;
		var constrainedPad = 1;

		if(cacheKey != [range,bounds],{ commands = nil });

		^commands ?? {
			cacheKey = [range,bounds];
			commands = [];
			p = grid.getParams(range[0], range[1], leftBound, rightBound, numTicks, tickSpacing);

			// Lines
			p['lines'].do { arg val, i;
				val = val.asArray; // value, [color]
				valNorm = grid.spec.unmap(val[0]);
				x = valNorm.linlin(0, 1, leftBound, rightBound);
				lineColor = val[1];

				commands = this.prAddLineCmds(commands, x, lineColor);
			};
			commands = commands.add( ['stroke'] );

			// Bounding lines
			// This check ensures it obeys Plot:-gridOnX (e.g. toggling grids on/off)
			if(grid.class != BlankGridLines) {
				// Could add different line characteristics (color, weight) to boundinglines here
				if(drawBoundingRect) {
					commands = commands.addAll([
						['strokeColor_', lineColor ? gridColor],
						['addRect', bounds],
						['stroke']
					]);
				} {
					if(drawBoundingLines) {
						commands = this.prAddLineCmds(commands, leftBound, lineColor);
						commands = this.prAddLineCmds(commands, rightBound, lineColor);
						commands = commands.add(['stroke']);
					} {
						if(drawBaseLine) {
							commands = this.prAddLineCmds(commands, leftBound, lineColor);
							commands = commands.add(['stroke']);
						}
					}
				};
			};

			// Tick labels
			if(showLabels and: { labelsHiddenBySize.not and: { p['labels'].notNil } }) {
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );

				labelSz = this.labelSize(p['labels']);

				p['labels'].do { arg val, i; // [value, label, (color, font, dropIfNeeded)]
					if(val[4].asBoolean.not) { // dropIfNeeded = false
						if(val[2].notNil) {
							commands = commands.add( ['color_',val[2] ] );
						};
						if(val[3].notNil) {
							commands = commands.add( ['font_',val[3] ] );
						};

						x = grid.spec.unmap(val[0]).linlin(0, 1, leftBound, rightBound);
						anchorPoint = Point(x, bottomBound);
						labelRect = labelSz.asRect.anchorTo(anchorPoint + labelOffset, labelAnchor);

						if(constrainLabelExtents, {
							// left and rightmost xlabels are constrained to grid bounds
							switch(x.asInteger,
								rightBound, {
									labelRect = labelRect.right_(
										min(labelRect.right, rightBound - constrainedPad)
									);
									alignSym = this.prGetJustifySymbol(\right);
								},
								leftBound, {
									labelRect = labelRect.left_(
										max(labelRect.left, leftBound + constrainedPad)
									);
									alignSym = this.prGetJustifySymbol(\left);
								}, {
									alignSym = this.prGetJustifySymbol(labelAlign)
								}
							);
						}, {
							alignSym = this.prGetJustifySymbol(labelAlign);
						});

						commands = commands.add([alignSym, val[1], labelRect]);
					}
				}
			};

			commands // return
		}
	}

	// Get the amount that a tick label will overhang each bounding edge of the
	// grid, given its size, anchoring, and offset. Userful when laying out
	// this grid in a UserView to allowing room for labels.
	labelOverhang { |labelStr|
		var strRect, rect;
		var overhang = [0, 0, 0, 0]; // left, top, right, bottom

		if (showLabels.not) {
			^overhang
		};
		strRect = if(labelStr.notNil) {
			labelStr.bounds(font);
		} {
			this.labelSize.asRect
		};
		rect = strRect.anchorTo(labelOffset, labelAnchor);

		// top side: assumes label doesn't extend past top bound
		overhang[3] = abs(max(rect.bottom, 0)); // bottom
		if (constrainLabelExtents) {
			overhang[[0,2]] = 0; // left, right
		} {
			overhang[0] = abs(min(rect.left, 0));  // left
			overhang[2] = abs(max(rect.right, 0)); // right
		};

		^overhang
	}

	// Get the Size of the largest tick label on this axis.
	// Labels will be calculated if not provided.
	labelSize { |labels|
		var protoLabel, labelSz;
		var axisBounds = this.prGetAxisBounds;

		if (labels.isNil) {
			labels = this.grid.getParams(
				this.range[0], this.range[1], axisBounds[0], axisBounds[1]
			).labels;
		};

		if (labels.isNil) {
			// There are currently no labels, return last used size
			^lastLabelSize
		} {
			protoLabel = "0".dup(
				try { labels.flatten.collect(_.size).maxItem } { 4 }
			).join;

			labelSz = try {
				protoLabel.bounds(font).asSize
			} {
				protoLabel.bounds(Font(Font.defaultSansFace, 9)).asSize
			};
			labelSz.width = labelSz.width * 1.3;
			lastLabelSize = labelSz;

			^labelSz
		};
	}

	clearCache { cacheKey = nil; }
	copy { ^super.copy.clearCache }

	prAddLineCmds { |cmds, val, color|
		cmds = cmds.add(['strokeColor_', color ? gridColor]);
		cmds = if (this.class == DrawGridX) {
			cmds.add(['line', Point(val, bounds.top), Point(val, bounds.bottom)]);
		} { // DrawGridY
			cmds.add(['line', Point(bounds.left, val), Point(bounds.right, val)]);
		};
		^cmds
	}

	prGetJustifySymbol { |alignSym|
		^switch(alignSym,
			\center, { 'stringCenteredIn' },
			\left,   { 'stringLeftJustIn' },
			\right,  { 'stringRightJustIn' }
		);
	}

	prGetAxisBounds { ^[this.bounds.left, this.bounds.right] }
}


DrawGridY : DrawGridX {

	init {
		var rangeStrings, nfrac, res;
		range = [grid.spec.minval, grid.spec.maxval];
		labelAnchor = \right;
		labelOffset = -3 @ 0;
		labelAlign = \right;
		constrainLabelExtents = true;
		font = Font(Font.defaultSansFace, 9);
		fontColor = Color.grey(0.3);
		gridColor = Color.grey(0.7);
		lastLabelSize = "000".bounds(font).asSize;
	}

	commands {
		var p;
		var labelSz, labelRect, anchorPoint, alignSym;
		var y, valNorm, lineColor;
		var bottomBound = bounds.bottom.asInteger;
		var topBound = bounds.top.asInteger;
		var constrainedPad = 1;

		if(cacheKey != [range,bounds],{ commands = nil });

		^commands ?? {
			commands = [];
			p = grid.getParams(range[0], range[1], topBound, bottomBound, numTicks, tickSpacing);

			// Lines
			p['lines'].do { arg val, i; // value, [color]
				val = val.asArray;
				valNorm = grid.spec.unmap(val[0]);
				lineColor = val[1];
				y = valNorm.linlin(0, 1, bottomBound, topBound);

				commands = this.prAddLineCmds(commands, y, lineColor);
			};
			commands = commands.add( ['stroke'] );

			// Bounding lines
			// This check ensures it obeys Plot:-gridOnX (e.g. toggling grids on/off)
			if(grid.class != BlankGridLines) {
				// Could add different line characteristics (color, weight) to boundinglines here
				if(drawBoundingRect) {
					commands = commands.addAll([
						['strokeColor_', lineColor ? gridColor],
						['addRect', bounds],
						['stroke']
					]);
				} {
					if(drawBoundingLines) {
						commands = this.prAddLineCmds(commands, topBound, lineColor);
						commands = this.prAddLineCmds(commands, bottomBound, lineColor);
						commands = commands.add(['stroke']);
					} {
						if(drawBaseLine) {
							commands = this.prAddLineCmds(commands, bottomBound, lineColor);
							commands = commands.add(['stroke']);
						}
					}
				};
			};

			if(showLabels and: { labelsHiddenBySize.not and: { p['labels'].notNil } }) {
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );

				labelSz = this.labelSize(p['labels']);

				p['labels'].do { arg val, i;
					y = grid.spec.unmap(val[0]).linlin(0, 1, bottomBound, topBound);

					if(val[2].notNil) {
						commands = commands.add( ['color_', val[2]] );
					};
					if(val[3].notNil) {
						commands = commands.add( ['font_', val[3]] );
					};

					anchorPoint = Point(bounds.left, y);
					labelRect = labelSz.asRect.anchorTo(anchorPoint + labelOffset, labelAnchor);

					if(constrainLabelExtents) {
						// bottom and top ylabels are constrained to grid bounds
						// note: downward direction is positive
						switch(y.asInteger,
							bottomBound, {
								labelRect = labelRect.bottom_(
									min(labelRect.bottom, bottomBound - constrainedPad)
								)
							},
							topBound, {
								labelRect = labelRect.top_(
									max(labelRect.top, topBound + constrainedPad)
								)
							}
						);
					};

					alignSym = this.prGetJustifySymbol(labelAlign);
					commands = commands.add([alignSym, val[1], labelRect]);
				};
			};

			commands // return
		}
	}

	// See description in DrawGridX:-labelOverhang
	labelOverhang { |labelStr|
		var strRect, rect;
		var overhang = [0, 0, 0, 0]; // left, top, right, bottom

		if (showLabels.not) {
			^overhang
		};

		strRect = if(labelStr.notNil) { labelStr.bounds(font) } { this.labelSize.asRect };
		rect = strRect.anchorTo(labelOffset, labelAnchor);

		// right side: assume label doesn't extend past right bound
		overhang[0] = abs(min(rect.left, 0)); // left
		if (constrainLabelExtents) {
			overhang[[1,3]] = 0; // top, bottom
		} {
			overhang[1] = abs(min(rect.top, 0)); // top
			overhang[3] = abs(max(rect.bottom, 0)); // bottom
		};

		^overhang
	}

	prGetAxisBounds { ^[this.bounds.top, this.bounds.bottom] }
}

// "Factory" class to return appropriate AbstractGridLines subclass based on the spec
GridLines {

	*new { arg spec;
		^spec.gridClass.new(spec.asSpec);
	}
}

AbstractGridLines {

	var <>spec, <appendLabel;

	*new { arg spec;
		^super.newCopyArgs(spec.asSpec).prCheckWarp;
	}

	prCheckWarp {
		if(this.class != this.spec.gridClass) {
			"The ControlSpec 'warp' expected by this % does not match 'warp' of the supplied ControlSpec (%).".format(
				this.class, this.spec.warp.class
			).warn;
		};
	}

	asGrid { ^this }
	niceNum { arg val,round;
		// http://books.google.de/books?id=fvA7zLEFWZgC&pg=PA61&lpg=PA61
		var exp,f,nf,rf;
		exp = floor(log10(val));
		rf = 10.pow(exp);
		f = val / rf;
		if(round,{
			if(f < 1.5,{
				^rf *  1.0
			});
			if(f < 3.0,{
				^rf *  2.0
			});
			if( f < 7.0,{
				^rf *  5.0
			});
			^rf *  10.0
		},{
			if(f <= 1.0,{
				^rf *  1.0;
			});
			if(f <= 2,{
				^rf *  2.0
			});
			if(f <= 5,{
				^rf *  5.0;
			});
			^rf *  10.0
		});
	}
	ideals { arg min,max,ntick=5;
		var nfrac,d,graphmin,graphmax,range,x;
		range = this.niceNum(max - min,false);
		d = this.niceNum(range / (ntick - 1),true);
		graphmin = floor(min / d) * d;
		graphmax = ceil(max / d) * d;
		nfrac = max( floor(log10(d)).neg, 0 );
		^[graphmin,graphmax,nfrac,d];
	}
	looseRange { arg min,max,ntick=5;
		^this.ideals(min,max,ntick).at( [ 0,1] )
	}
	getParams {
		^this.subclassResponsibility
	}
	formatLabel { arg val, numDecimalPlaces;
		var str;
		str = if (numDecimalPlaces == 0) {
			val.asInteger.asString
		} {
			val.round( (10**numDecimalPlaces).reciprocal).asString
		};
		^if(appendLabel.notNil) { str = str ++ appendLabel } { str }
	}
	appendLabel_ { |str| appendLabel = str !? { str.asString } }
}

LinearGridLines : AbstractGridLines {
	getParams { |valueMin, valueMax, pixelMin, pixelMax, numTicks, tickSpacing = 64|
		var lines,p,pixRange;
		var nfrac,d,graphmin,graphmax,range;
		pixRange = pixelMax - pixelMin;
		if(numTicks.isNil,{
			numTicks = (pixRange / tickSpacing);
			numTicks = numTicks.max(3).round(1);
		});
		# graphmin,graphmax,nfrac,d = this.ideals(valueMin,valueMax,numTicks);
		lines = [];
		if(d != inf,{
			forBy(graphmin,graphmax + (0.5*d),d,{ arg tick;
				if(tick.inclusivelyBetween(valueMin,valueMax),{
					lines = lines.add( tick );
				})
			});
		});
		p = ();
		p['lines'] = lines;
		if(pixRange / numTicks > 9) {
			if (sum(lines % 1) == 0) { nfrac = 0 };
			p['labels'] = lines.collect({ arg val; [val, this.formatLabel(val, nfrac)] });
		};
		^p
	}
}

ExponentialGridLines : AbstractGridLines {

	getParams { |valueMin, valueMax, pixelMin, pixelMax, numTicks, tickSpacing = 64|
		var lines,p,pixRange;
		var nfrac,d,graphmin,graphmax,range, nfracarr;
		var nDecades, first, step, tick, expRangeIsValid, expRangeIsPositive, roundFactor;
		var drawLabel = true, maxNumTicks;

		pixRange = pixelMax - pixelMin;
		lines = [];
		nfracarr = [];

		expRangeIsValid = ((valueMin > 0) and: { valueMax > 0 }) or: {(valueMin < 0) and: { valueMax < 0 } };

		if(expRangeIsValid) {
			expRangeIsPositive = valueMin > 0;
			if(expRangeIsPositive) {
				nDecades = log10(valueMax/valueMin);
				first = step = 10**(valueMin.abs.log10.trunc);
				roundFactor = step;
			} {
				nDecades = log10(valueMin/valueMax);
				step = 10**(valueMin.abs.log10.trunc - 1);
				first = 10 * step.neg;
				roundFactor = 10**(valueMax.abs.log10.trunc);
			};
			//workaround for small ranges
			if(nDecades < 1) {
				step = step * 0.1;
				roundFactor = roundFactor * 0.1;
				nfrac = valueMin.abs.log10.floor.neg + 1;
			};
			numTicks ?? {numTicks = (pixRange / (tickSpacing * nDecades))};
			tick = first;
			while ({tick <= (valueMax + step)}) {
				if(round(tick, roundFactor).inclusivelyBetween(valueMin, valueMax)) {
					if(
						(numTicks > 4) or:
						{ ((numTicks > 2.5).and(tick.abs.round(1).asInteger == this.niceNum(tick.abs, true).round(1).asInteger)).and(tick >= 1) } or:
						{ ((numTicks > 2).and((tick - this.niceNum(tick, true)).abs < 1e-15)) } or:
						{ (tick.abs.round(roundFactor).log10.frac < 0.01) } or:
						{ (tick.absdif(valueMax) < 1e-15) } or:
						{ (tick.absdif(valueMin) < 1e-15) }
					) {
						maxNumTicks = tickSpacing.linlin(32, 64, 8, 5, nil);
						maxNumTicks = maxNumTicks * tick.asFloat.asString.bounds.width.linlin(24, 40, 0.7, 1.5); // 10.0.asString.bounds.width to 1000.0.asString.bounds.width
						if(
							(numTicks < maxNumTicks) and:
							{ ((tick.abs.round(1).asInteger == this.niceNum(tick.abs, true).round(1).asInteger)).and(tick >= 1).not } and:
							{ (((tick - this.niceNum(tick, true)).abs < 1e-15)).not } and:
							{ (tick.abs.log10.frac > numTicks.linlin(4, maxNumTicks, 0.7, 0.93)) }
						) {
							drawLabel = false // drop labels for tightly spaced upper area of the decade
						};
						lines = lines.add([tick, drawLabel])
					};
				};
				if(tick >= (step * 9.9999)) { step = (step * 10) };
				if(expRangeIsPositive) {
					if((round(tick,roundFactor) >= (round(step*10,roundFactor))) and: { (nDecades > 1) }) { step = (step*10) };
				} {
					if((round(tick.abs,roundFactor) <= (round(step,roundFactor))) and: { (nDecades > 1) }) { step = (step*0.1) };
				};
				tick = (tick+step);
			};
			nfracarr = lines.collect({ arg arr;
				var val = arr[0];
				val.abs.log10.floor.neg.max(0)
			});

		} {
			format("Unable to get exponential GridLines for values between % and %", valueMin, valueMax).warn;
			numTicks ?? {
				numTicks = (pixRange / tickSpacing);
				numTicks = numTicks.max(3).round(1);
			}; // set numTicks regardless to avoid errors
		};
		p = ();
		p['lines'] = lines.flop.first;
		if(pixRange / numTicks > 9) {
			if (sum(p['lines'] % 1) == 0) { nfrac = 0 };
			p['labels'] = lines.collect({ arg arr, inc;
				var val, drawLabel, thisLabel;
				#val, drawLabel = arr;
				[val, this.formatLabel(val, nfrac ? nfracarr[inc] ? 1), nil, nil, drawLabel.not] });
		};
		^p
	}
}

BlankGridLines : AbstractGridLines {

	getParams { ^() }

	prCheckWarp { }
}


+ Nil {
	asGrid { ^BlankGridLines.new }
	gridClass { ^BlankGridLines }
}
