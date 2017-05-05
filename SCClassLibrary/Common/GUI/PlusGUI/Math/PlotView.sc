Plot {

	var <>plotter, <value;
	var <bounds, <plotBounds,<>drawGrid;

	var <spec, <domainSpec;
	var <font, <fontColor, <gridColorX, <gridColorY, <>plotColor, <>backgroundColor;
	var <gridOnX = true, <gridOnY = true, <>labelX, <>labelY;

	var valueCache;

	*initClass {
		StartUp.add {
			GUI.skin.put(\plot, (
				gridColorX: QtGUI.palette.highlight,
				gridColorY: QtGUI.palette.highlight,
				fontColor: QtGUI.palette.windowText,
				plotColor: [QtGUI.palette.windowText, Color.blue.valueBlend(QtGUI.palette.windowText), Color.red.valueBlend(QtGUI.palette.windowText), Color.green(0.7).valueBlend(QtGUI.palette.windowText)],
				background: QtGUI.palette.base,
				gridLinePattern: nil,
				gridLineSmoothing: false,
				labelX: "",
				labelY: "",
				expertMode: false,
				gridFont: Font( Font.defaultSansFace, 9 )
			));
		}
	}

	*new { |plotter|
		^super.newCopyArgs(plotter).init
	}

	init {
		var fontName;
		var gui = plotter.gui;
		var skin = GUI.skin.at(\plot);

		drawGrid = DrawGrid(bounds ? Rect(0,0,1,1),nil,nil);
		drawGrid.x.labelOffset = Point(0,4);
		drawGrid.y.labelOffset = Point(-10,0);
		skin.use {
			font = ~gridFont ?? { Font.default };
			this.gridColorX = ~gridColorX;
			this.gridColorY = ~gridColorY;
			plotColor = ~plotColor;
			this.fontColor = ~fontColor;
			backgroundColor = ~background;
			this.gridLineSmoothing = ~gridLineSmoothing;
			this.gridLinePattern = ~gridLinePattern !? {~gridLinePattern.as(FloatArray)};
			labelX = ~labelX;
			labelY = ~labelY;
		};
	}

	bounds_ { |rect|
		var size = (try { "foo".bounds(font).height } ?? { font.size } * 1.5);
		plotBounds = if(rect.height > 40) { rect.insetBy(size, size) } { rect };
		bounds = rect;
		valueCache = nil;
		drawGrid.bounds = plotBounds;
	}

	value_ { |array|
		value = array;
		valueCache = nil;
	}
	spec_ { |sp|
		spec = sp;
		if(gridOnY and: spec.notNil,{
			drawGrid.vertGrid = spec.grid;
		},{
			drawGrid.vertGrid = nil
		})
	}
	domainSpec_ { |sp|
		domainSpec = sp;
		if(gridOnX and: domainSpec.notNil,{
			drawGrid.horzGrid = domainSpec.grid;
		},{
			drawGrid.horzGrid = nil
		})
	}
	gridColorX_ { |c|
		drawGrid.x.gridColor = c;
		gridColorX = c;
	}
	gridColorY_ { |c|
		drawGrid.y.gridColor = c;
		gridColorY = c;
	}
	font_ { |f|
		font = f;
		drawGrid.font = f;
	}
	fontColor_ { |c|
		fontColor = c;
		drawGrid.fontColor = c;
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
		var sbounds;
		if(gridOnX and: { labelX.notNil }) {
			sbounds = try { labelX.bounds(font) } ? 0;
			Pen.font = font;
			Pen.strokeColor = fontColor;
			Pen.stringAtPoint(labelX,
				plotBounds.right - sbounds.width @ plotBounds.bottom
			)
		};
		if(gridOnY and: { labelY.notNil }) {
			sbounds = try { labelY.bounds(font) } ? 0;
			Pen.font = font;
			Pen.strokeColor = fontColor;
			Pen.stringAtPoint(labelY,
				plotBounds.left - sbounds.width - 3 @ plotBounds.top
			)
		};
	}

	domainCoordinates { |size|
		var val = this.resampledDomainSpec.unmap(plotter.domain ?? { (0..size-1) });
		^plotBounds.left + (val * plotBounds.width);
	}

	dataCoordinates {
		var val = spec.warp.unmap(this.prResampValues);
		^plotBounds.bottom - (val * plotBounds.height); // measures from top left (may be arrays)
	}

	resampledSize {
		^min(value.size, plotBounds.width / plotter.resolution)
	}

	resampledDomainSpec {
		var offset = if(this.hasSteplikeDisplay) { 0 } { 1 };
		^domainSpec.copy.maxval_(this.resampledSize - offset)
	}

	drawData {
		var mode = plotter.plotMode;
		var ycoord = this.dataCoordinates;
		var xcoord = this.domainCoordinates(ycoord.size);

		Pen.use {
			Pen.width = 1.0;
			Pen.joinStyle = 1;
			plotColor = plotColor.as(Array);

			Pen.addRect(plotBounds);
			Pen.clip; // clip curve to bounds.

			if(ycoord.at(0).isSequenceableCollection) { // multi channel expansion
				ycoord.flop.do { |y, i|
					Pen.beginPath;
					this.perform(mode, xcoord, y);
					Pen.strokeColor = plotColor.wrapAt(i);
					Pen.stroke;
				}
			} {
				Pen.beginPath;
				Pen.strokeColor = plotColor.at(0);
				this.perform(mode, xcoord, ycoord);
				Pen.stroke;
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
		^this.resampledDomainSpec.map((x - plotBounds.left) / plotBounds.width)
	}

	getRelativePositionY { |y|
		^spec.map((plotBounds.bottom - y) / plotBounds.height)
	}

	hasSteplikeDisplay {
		^#[\levels, \steps].includes(plotter.plotMode)
	}

	getIndex { |x|
		var offset = if(this.hasSteplikeDisplay) { 0.5 } { 0.0 }; // needs to be fixed.
		^(this.getRelativePositionX(x) - offset).round.asInteger
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
		drawGrid.clearCache;
	}
	copy {
		^super.copy.drawGrid_(drawGrid.copy)
	}
	prResampValues {
		^if(value.size <= (plotBounds.width / plotter.resolution)) {
			value
		} {
			valueCache ?? { valueCache = value.resamp1(plotBounds.width / plotter.resolution) }
		}
	}
}



Plotter {

	var <>name, <>bounds, <>parent;
	var <value, <data, <>domain;
	var <plots, <specs, <domainSpecs;
	var <cursorPos, <>plotMode = \linear, <>editMode = false, <>normalized = false;
	var <>resolution = 1, <>findSpecs = true, <superpose = false;
	var modes, <interactionView;
	var <editPlotIndex, <editPos;

	var <>drawFunc, <>editFunc;
	var <gui;

	*new { |name, bounds, parent|
		^super.newCopyArgs(name).makeWindow(parent, bounds)
	}

	makeWindow { |argParent, argBounds|
		var btnBounds;
		parent = argParent ? parent;
		bounds = argBounds ? bounds;
		if(parent.isNil) {
			parent = Window.new(name ? "Plot", bounds ? Rect(100, 200, 400, 300));
			bounds = parent.view.bounds.insetBy(8);
			interactionView = UserView.new(parent, bounds);

			interactionView.drawFunc = { this.draw };
			parent.front;
			parent.onClose = { parent = nil };

		} {
			bounds = bounds ?? { parent.bounds.moveTo(0, 0) };
			interactionView = UserView.new(parent, bounds);
			interactionView.drawFunc = { this.draw };
		};
		modes = [\points, \levels, \linear, \plines, \steps].iter.loop;

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
		});
	}

	value_ { |arrays|
		this.setValue(arrays, findSpecs, true)
	}

	setValue { |arrays, findSpecs = true, refresh = true, separately = true, minval, maxval, defaultRange|
		value = arrays;
		data = this.prReshape(arrays);
		if(findSpecs) {
			this.calcSpecs(separately, minval, maxval, defaultRange);
			this.calcDomainSpecs;
		};
		this.updatePlots;
		if(refresh) { this.refresh };
	}

	superpose_ { |flag|
		superpose = flag;
		if ( value.notNil ){
			this.setValue(value, false, true);
		};
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
			)
		}
	}

	makePlots {
		var template = if(plots.isNil) { Plot(this) } { plots.last };
		plots !? { plots = plots.keep(data.size.neg) };
		plots = plots ++ template.dup(data.size - plots.size);
		plots.do { |plot, i| plot.value = data.at(i) };

		this.updatePlotSpecs;
		this.updatePlotBounds;
	}

	updatePlots {
		if(plots.size != data.size) {
			this.makePlots;
		} {
			plots.do { |plot, i|
				plot.value = data.at(i)
			}
		}
	}

	updatePlotSpecs {
		specs !? {
			plots.do { |plot, i|
				plot.spec = specs.clipAt(i)
			}
		};
		domainSpecs !? {
			plots.do { |plot, i|
				plot.domainSpec = domainSpecs.clipAt(i)
			}
		}
	}

	setProperties { |... pairs|
		pairs.pairsDo { |selector, value|
			selector = selector.asSetter;
			plots.do { |x| x.perform(selector, value) }
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
			[0, val.size - 1, \lin, 1].asSpec
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
			^array.collect { |x| x.asArray.clipExtend(size) }.flop.bubble		};
		^array
	}
}


+ ArrayedCollection {
	plot { |name, bounds, discrete=false, numChannels, minval, maxval, separately = true|
		var array, plotter;
		array = this.as(Array);

		if(array.maxDepth > 3) {
			"Cannot currently plot an array with more than 3 dimensions".warn;
			^nil
		};
		plotter = Plotter(name, bounds);
		if(discrete) { plotter.plotMode = \points };

		numChannels !? { array = array.unlace(numChannels) };
		array = array.collect {|elem|
			if (elem.isKindOf(Env)) {
				elem.asMultichannelSignal.flop
			} {
				elem
			};
			if (elem.isKindOf(Rational)) { elem.asFloat } { elem };
		};
		plotter.setValue(
			array,
			findSpecs: true,
			separately: separately,
			refresh: true,
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
		plotter.domainSpecs = [[min ?? { this.minItem }, max ?? { this.maxItem }].asSpec];
		plotter.specs = [[0, histo.maxItem, \linear, 1].asSpec];
		plotter.plotMode = \steps;
		^plotter
	}
}


+ Function {
	plot { |duration = 0.01, server, bounds, minval, maxval, separately = false|
		var name = this.asCompileString, plotter;
		if(name.size > 50 or: { name.includes(Char.nl) }) { name = "function plot" };
		plotter = Plotter(name, bounds);
		plotter.value = [0.0];
		server = server ? Server.default;
		server.waitForBoot {
			this.loadToFloatArray(duration, server, { |array, buf|
				var numChan = buf.numChannels;
				{
					plotter.setValue(
						array.unlace(numChan).collect(_.drop(-1)),
						findSpecs: true,
						separately: separately,
						refresh: false,
						minval: minval,
						maxval: maxval
					);
					plotter.domainSpecs = ControlSpec(0, duration, units: "s");
					plotter.refresh;
				}.defer
			})
		};
		^plotter
	}

	plotAudio { |duration = 0.01, minval = -1, maxval = 1, server, bounds|
		^this.plot(duration, server, bounds, minval, maxval)
	}
}

+ Bus {
	plot { |duration = 0.01, bounds, minval, maxval, separately = false|
		^{ InFeedback.ar(this.index, this.numChannels) }.plot(duration, this.server, bounds, minval, maxval, separately)
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
		var plotter;
		if(server.serverRunning.not) { "Server % not running".format(server).warn; ^nil };
		if(numFrames.isNil) { "Buffer not allocated, can't plot data".warn; ^nil };
		plotter = [0].plot(
			name ? "Buffer plot (bufnum: %)".format(this.bufnum),
			bounds, minval: minval, maxval: maxval
		);
		this.loadToFloatArray(action: { |array, buf|
			{
				plotter.setValue(
					array.unlace(buf.numChannels),
					findSpecs: true,
					separately: separately,
					refresh: false,
					minval: minval,
					maxval: maxval
				);
				plotter.domainSpecs = ControlSpec(0.0, buf.numFrames, units:"frames");
				plotter.refresh;
			}.defer
		});
		^plotter
	}
}


+ Env {
	plot { |size = 400, bounds, minval, maxval, name|
		var plotLabel = if (name.isNil) { "envelope plot" } { name };
		var plotter = [this.asMultichannelSignal(size).flop]
		.plot(name, bounds, minval: minval, maxval: maxval);

		var duration     = this.duration.asArray;
		var channelCount = duration.size;

		var totalDuration = if (channelCount == 1) { duration } { duration.maxItem ! channelCount };

		plotter.domainSpecs = totalDuration.collect(ControlSpec(0, _, units: "s"));
		plotter.setProperties(\labelX, "time");
		plotter.refresh;
		^plotter
	}
}

+ AbstractFunction {
	plotGraph { arg n=500, from = 0.0, to = 1.0, name, bounds, discrete = false,
		numChannels, minval, maxval, separately = true;
		var array = Array.interpolation(n, from, to);
		var res = array.collect { |x| this.value(x) };
		res.plot(name, bounds, discrete, numChannels, minval, maxval, separately)
	}
}
