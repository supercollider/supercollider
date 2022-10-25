DrawGrid {

	var <bounds, <>x, <>y;
	var <>opacity=0.7, <>smoothing=false, <>linePattern;

	*new { |bounds, horzGrid, vertGrid|
		^super.new.init(bounds, horzGrid, vertGrid)
	}

	*test { arg horzGrid, vertGrid, bounds = (500@400);
		^DrawGrid(bounds.asRect, horzGrid, vertGrid).test
	}

	test { ^DrawGridTest(this) }

	init { arg bounds, h, v;
		x = DrawGridX(h);
		y = DrawGridY(v);
		this.bounds = bounds;
		this.font = Font(Font.defaultSansFace, 9);
		this.fontColor = Color.grey(0.3);
		this.gridColors = [Color.grey(0.7), Color.grey(0.7)];
	}
	bounds_ { arg b;
		bounds = b;
		x.bounds = b;
		y.bounds = b;
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
		x.gridColor = colors[0];
		y.gridColor = colors[1];
	}
	horzGrid_ { arg g;
		x.grid = g;
	}
	vertGrid_ { arg g;
		y.grid = g;
	}
	tickSpacing_ { arg xpx, ypx;
		x.tickSpacing = xpx;
		y.tickSpacing = ypx;
	}
	numTicks_ { arg numx, numy;
		x.numTicks = numx;
		y.numTicks = numy;
	}
	copy {
		^DrawGrid(bounds,x.grid,y.grid).x_(x.copy).y_(y.copy).opacity_(opacity).smoothing_(smoothing).linePattern_(linePattern)
	}
	clearCache {
		x.clearCache;
		y.clearCache;
	}
}


DrawGridX {

	var <grid,<>range,<>bounds;
	var <font,<fontColor,<gridColor,<labelOffset;
	var commands,cacheKey;
	var txtPad = 2; // match with Plot:txtPad
	var <tickSpacing = 64;
	var <numTicks = nil; // nil for dynamic with view size

	*new { arg grid;
		^super.newCopyArgs(grid.asGrid).init
	}

	init {
		range = [grid.spec.minval, grid.spec.maxval];
		labelOffset = "20000".bounds.size.asPoint;
	}
	grid_ { arg g;
		grid = g.asGrid;
		range = [grid.spec.minval, grid.spec.maxval];
		this.clearCache;
	}
	setZoom { arg min,max;
		range = [min, max];
	}
	tickSpacing_{ |px|
		px !? {
			tickSpacing = px;
			this.clearCache;
		};
	}
	numTicks_{ |num|
		numTicks = num;
		this.clearCache;
	}
	font_{ |afont|
		font = afont;
		this.clearCache;
	}
	fontColor_{ |color|
		fontColor = color;
		this.clearCache;
	}
	gridColor_{ |color|
		gridColor = color;
		this.clearCache;
	}
	// labelOffset is effectively a point describing the size of a grid label
	labelOffset_{ |pt|
		labelOffset = pt;
		this.clearCache;
	}
	commands {
		var p;
		var valNorm, lineColor;
		if(cacheKey != [range,bounds],{ commands = nil });
		^commands ?? {
			cacheKey = [range,bounds];
			commands = [];
			p = grid.getParams(range[0], range[1], bounds.left, bounds.right, numTicks, tickSpacing);

			p['lines'].do { arg val, i;
				var x;
				val = val.asArray; // value, [color]
				valNorm = grid.spec.unmap(val[0]);
				x = valNorm.linlin(0, 1, bounds.left, bounds.right);
				lineColor = val[1];

				commands = this.prAddLineCmds(commands, x, lineColor);

				// always draw line on left and right edges
				case
				{i == 0 and: { valNorm != 0 }} {
					commands = this.prAddLineCmds(commands, bounds.left, lineColor);
				}
				{i == (p['lines'].size-1) and: { valNorm != 1 }} {
					commands = this.prAddLineCmds(commands, bounds.right, lineColor);
				}
			};
			// Handle case where there is only one line:
			// left and middle line has been added, now need a right line
			if (p['lines'].size == 1 and: { valNorm != 1 }) {
				commands = this.prAddLineCmds(commands, bounds.right, lineColor);
			};

			if(p['labels'].notNil and: { labelOffset.x > 0 }, {
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );
				p['labels'].do { arg val; // [value, label, (color, font, dropIfNeeded)]
					var x;
					if(val[4].asBoolean.not) {
						if(val[2].notNil) {
							commands = commands.add( ['color_',val[2] ] );
						};
						if(val[3].notNil) {
							commands = commands.add( ['font_',val[3] ] );
						};
						x = grid.spec.unmap(val[0]).linlin(0, 1, bounds.left, bounds.right);

						commands = commands.add([
							'stringCenteredIn', val[1].asString,
							Rect.aboutPoint(
								x @ bounds.bottom, labelOffset.x/2, labelOffset.y/2
							).top_(bounds.bottom + txtPad)
						]);
					}
				}
			});
			commands
		}
	}

	prAddLineCmds { |cmds, val, color|
		cmds = cmds.add( ['strokeColor_', color ? gridColor] );
		cmds = if (this.class == DrawGridX) {
			cmds.add( ['line', Point(val, bounds.top), Point(val, bounds.bottom) ] );
		} { // DrawGridY
			cmds.add( ['line', Point(bounds.left, val), Point(bounds.right, val) ] );
		};
		^cmds = cmds.add( ['stroke'] ); // return
	}

	clearCache { cacheKey = nil; }
	copy { ^super.copy.clearCache }
}


DrawGridY : DrawGridX {

	commands {
		var p;
		var valNorm, lineColor;
		if(cacheKey != [range,bounds],{ commands = nil });

		^commands ?? {

			commands = [];

			p = grid.getParams(range[0], range[1], bounds.top, bounds.bottom, numTicks, tickSpacing);

			p['lines'].do { arg val, i; // value, [color]
				var y;
				val = val.asArray;
				valNorm = grid.spec.unmap(val[0]);
				lineColor = val[1];
				y = valNorm.linlin(0, 1, bounds.bottom, bounds.top);

				commands = this.prAddLineCmds(commands, y, lineColor);

				// draw grid line on top and bottom bound even if there is no 'line' there
				case
				{ i == 0 and: { valNorm != 0 } } { // bottom
					commands = this.prAddLineCmds(commands, bounds.bottom, lineColor);
				}
				{ i == (p['lines'].size-1) and: { valNorm != 1 } } { // top
					commands = this.prAddLineCmds(commands, bounds.top, lineColor);
				};
			};
			// Handle case where there is only one line:
			// bottom and middle line has been added, now need a top line
			if (p['lines'].size == 1 and: { valNorm != 1 }) {
				commands = this.prAddLineCmds(commands, bounds.top, lineColor);
			};

			if(p['labels'].notNil and: { labelOffset.y > 0 }, {
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );

				p['labels'].do { arg val;
					var y, lblRect;

					y = grid.spec.unmap(val[0]).linlin(0, 1, bounds.bottom, bounds.top);
					if(val[2].notNil) {
						commands = commands.add( ['color_', val[2]] );
					};
					if(val[3].notNil) {
						commands = commands.add( ['font_', val[3]] );
					};

					lblRect = Rect.aboutPoint(
						Point(0, y), labelOffset.x/2, labelOffset.y/2
					).right_(bounds.left - txtPad);

					switch(y.asInteger,
						bounds.bottom.asInteger, {
							lblRect = lblRect.bottom_(bounds.bottom + txtPad) },
						bounds.top.asInteger, {
							lblRect = lblRect.top_(bounds.top - txtPad) }
					);
					commands = commands.add(['stringRightJustIn', val[1].asString, lblRect]);
				}
			});
			commands
		}
	}
}

// DrawGridRadial : DrawGridX {}

// "factory" class
GridLines {

	*new { arg spec;
		^spec.gridClass.new(spec.asSpec);
	}
}

AbstractGridLines {

	var <>spec;

	*new { arg spec;
		^super.newCopyArgs(spec.asSpec).prCheckWarp;
	}

	prCheckWarp {
		if(this.class.name != this.spec.gridClass.name) {
			"% expects a spec with %, but was passed a spec with % instead.".format(this.class.name, this.spec.warp.class.name, spec.asSpec.warp.class.name).warn;
		};
	}

	asGrid { ^this }
	niceNum { arg val,round;
		// http://books.google.de/books?id=fvA7zLEFWZgC&pg=PA61&lpg=PA61
		var exp,f,nf,rf;
		exp = floor(log10(val));
		f = val / 10.pow(exp);
		rf = 10.pow(exp);
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
		if (numDecimalPlaces == 0) {
			^val.asInteger.asString
		} {
			^val.round( (10**numDecimalPlaces).reciprocal).asString
		}
	}
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
				var drawLabel = true, maxNumTicks;
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

DrawGridTest : DrawGrid {
	var <testView;
	var insetH = 45, insetV = 35; // left, bottom margins for labels
	var gridPad = 15;             // right, top margin
	var txtPad = 2;               // label offset from window's edge
	var win, winBounds, font, fcolor;

	*new { arg drawGrid;
		^super.new(
			drawGrid.bounds, drawGrid.x.grid, drawGrid.y.grid
		)
		.opacity_(drawGrid.opacity)
		.smoothing_(drawGrid.smoothing)
		.linePattern_(drawGrid.linePattern)
		.makeWindow;
	}

	makeWindow {
		font = Font( Font.defaultSansFace, 9 ); // match this.font
		fcolor = Color.grey(0.3); // match this.fontColor
		insetH = insetH + gridPad;
		insetV = insetV + gridPad;

		// bounds of the grid lines
		this.bounds = this.bounds ?? { Rect(0, 0, 500, 400) };
		// move grid bounds to make room for tick & axis labels
		this.bounds = this.bounds.moveTo(insetH-gridPad, gridPad);

		winBounds = this.bounds + Size(insetH, insetV);
		win = Window("Grid test",
			winBounds.center_(Window.screenBounds.center)
		).front;

		// the view holding the gridlines
		testView = UserView(win, winBounds.size.asRect)
		.drawFunc_({ |uv|
			var unitsStr;

			// draw the drid
			this.draw;

			// draw x-axis label
			unitsStr = this.x.grid.spec.units;
			if(unitsStr.size > 0) {
				Pen.push;
				Pen.translate(this.bounds.center.x, uv.bounds.bottom);
				Pen.stringCenteredIn(unitsStr,
					unitsStr.bounds.center_(0@0).bottom_(txtPad.neg),
					font, fcolor);
				Pen.pop;
			};

			// draw y-axis label
			unitsStr = this.y.grid.spec.units;
			if(unitsStr.size > 0) {
				Pen.push;
				Pen.translate(0, this.bounds.center.y);
				Pen.rotateDeg(-90);
				Pen.stringCenteredIn(unitsStr,
					unitsStr.bounds.center_(0@0).top_(txtPad),
					font, fcolor);
				Pen.pop;
			};
		})
		.onResize_({ |uv|
			this.bounds = uv.bounds
			.insetBy(insetH.half, insetV.half)
			.moveTo(insetH - gridPad, gridPad);
		})
		.resize_(5)
		.background_(Color.white);
	}

	refresh { testView.refresh }
}


+ Nil {
	asGrid { ^BlankGridLines.new }
	gridClass { ^BlankGridLines }
}
