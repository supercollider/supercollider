
Plot {

	var <>plotter, <value, <>spec, <>domainSpec;
	var <bounds, <plotBounds;
	var <>font, <>fontColor, <>gridColorX, <>gridColorY, <>plotColor, <>backgroundColor;
	var	<>gridLinePattern, <>gridLineSmoothing;
	var <>gridOnX = true, <>gridOnY = true, <>labelX, <>labelY;

	var valueCache;

	*initClass {
		StartUp.add {
				GUI.skin.put(\plot, (
					gridColorX: Color.grey(0.7),
					gridColorY: Color.grey(0.7),
					fontColor: Color.grey(0.3),
					plotColor: [Color.black, Color.blue, Color.red, Color.green(0.7)],
					background: Color.new255(235, 235, 235),
					//gridLinePattern: FloatArray[1, 5],
					gridLinePattern: FloatArray[1, 0],
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
		var skin = GUI.skin.at(\plot);

		skin.use {
			font = ~gridFont ?? { Font.default };
			if(font.class != GUI.font) { font = Font(font.name, font.size) };
			gridColorX = ~gridColorX;
			gridColorY = ~gridColorY;
			plotColor = ~plotColor;
			fontColor = ~fontColor;
			backgroundColor = ~background;
			gridLineSmoothing = ~gridLineSmoothing;
			gridLinePattern = ~gridLinePattern.as(FloatArray);
			labelX = ~labelX;
			labelY = ~labelY;
		};
	}

	bounds_ { |rect|
		var size = (try { "foo".bounds(font).height } ?? { font.size } * 1.5);
		plotBounds = if(rect.height > 40) { rect.insetBy(size, size) } { rect };
		bounds = rect;
		valueCache = nil;
	}

	value_ { |array|
		value = array;
		valueCache = nil;
	}


	draw {
		this.drawBackground;
		if(gridOnX) { this.drawGridX; this.drawNumbersX; };
		if(gridOnY) { this.drawGridY; this.drawNumbersY; };
		this.drawLabels;
		this.drawData;
		plotter.drawFunc.value(this); // additional elements
	}

	drawBackground {
		Pen.addRect(bounds);
		Pen.fillColor = backgroundColor;
		Pen.fill;
	}

	drawGridX {

		var top = plotBounds.top;
		var base = plotBounds.bottom;

		this.drawOnGridX { |hpos|
			Pen.moveTo(hpos @ base);
			Pen.lineTo(hpos @ top);
		};

		Pen.strokeColor = gridColorX;
		this.prStrokeGrid;

	}

	drawGridY {

		var left = plotBounds.left;
		var right = plotBounds.right;

		this.drawOnGridY { |vpos|
			Pen.moveTo(left @ vpos);
			Pen.lineTo(right @ vpos);
		};

		Pen.strokeColor = gridColorY;
		this.prStrokeGrid;

	}

	drawNumbersX {
		var top = plotBounds.top;
		var base = plotBounds.bottom;
		Pen.fillColor = fontColor;
		this.drawOnGridX { |hpos, val, i|
			var string = val.asStringPrec(5) ++ domainSpec.units;
			Pen.font = font;
			Pen.stringAtPoint(string, hpos @ base);
			Pen.stroke;
		};
		Pen.stroke;

	}

	drawNumbersY {

		var left = plotBounds.left;
		var right = plotBounds.right;
		Pen.fillColor = fontColor;

		this.drawOnGridY { |vpos, val, i|
			var string = val.asStringPrec(5).asString ++ spec.units;
			if(gridOnX.not or: { i > 0 }) {
				Pen.font = font;
				Pen.stringAtPoint(string, left @ vpos);
			}
		};

		Pen.stroke;

	}


	drawOnGridX { |func|

		var width = plotBounds.width;
		var left = plotBounds.left;
		var n, gridValues;
		var xspec = domainSpec;
		if(this.hasSteplikeDisplay) {
			// special treatment of special case: lines need more space
			xspec = xspec.copy.maxval_(xspec.maxval * value.size / (value.size - 1))
		};
		n = (plotBounds.width / 64).round(2);
		if(xspec.hasZeroCrossing) { n = n + 1 };

		gridValues = xspec.gridValues(n);
		if(gridOnY) { gridValues = gridValues.drop(1) };
		gridValues = gridValues.drop(-1);

		gridValues.do { |val, i|
			var hpos = left + (xspec.unmap(val) * width);
			func.value(hpos, val, i);
		};
	}

	drawOnGridY { |func|

		var base = plotBounds.bottom;
		var height = plotBounds.height.neg; // measures from top left
		var n, gridValues;

		n = (plotBounds.height / 32).round(2);
		if(spec.hasZeroCrossing) { n = n + 1 };
		gridValues = spec.gridValues(n);

		gridValues.do { |val, i|
			var vpos = base + (spec.unmap(val) * height);
			func.value(vpos, val, i);
		};

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
		 var val = spec.unmap(this.prResampValues);
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

		Pen.width = 1.0;
		Pen.joinStyle = 1;
		plotColor = plotColor.as(Array);

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


	editData { |x, y, plotIndex|
		var index = this.getIndex(x);
		var val = this.getRelativePositionY(y);
		plotter.editFunc.value(plotter, plotIndex, index, val, x, y);
		value.clipPut(index, val);
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
	}


	// private implementation

	prResampValues {
		^if(value.size <= (plotBounds.width / plotter.resolution)) {
			value
		} {
			valueCache ?? { valueCache = value.resamp1(plotBounds.width / plotter.resolution) }
		}
	}

	prStrokeGrid {
		Pen.width = 1;

		try {
			Pen.smoothing_(gridLineSmoothing);
			Pen.lineDash_(gridLinePattern);
		};

		Pen.stroke;

		try {
			Pen.smoothing_(true);
			Pen.lineDash_(FloatArray[1, 0])
		};
	}

}



Plotter {

	var <>name, <>bounds, <>parent;
	var <value, <data, <>domain;
	var <plots, <specs, <domainSpecs;
	var <cursorPos, <>plotMode = \linear, <>editMode = false, <>normalized = false;
	var <>resolution = 1, <>findSpecs = true, <superpose = false;
	var modes, <interactionView;

	var <>drawFunc, <>editFunc;

	*new { |name, bounds, parent|
		^super.newCopyArgs(name).makeWindow(parent, bounds)
	}

	makeWindow { |argParent, argBounds|
		parent = argParent ? parent;
		bounds = argBounds ? bounds;
		if(parent.isNil) {
			parent = Window.new(name ? "Plot", bounds ? Rect(100, 200, 400, 300));
			bounds = parent.view.bounds.insetBy(5, 0).moveBy(-5, 0);
			interactionView = UserView(parent, bounds);
			if(GUI.skin.at(\plot).at(\expertMode).not) { this.makeButtons };
			parent.drawHook = { this.draw };
			parent.front;
			parent.onClose = { parent = nil };

		} {
			bounds = bounds ?? { parent.bounds.moveTo(0, 0) };
			interactionView = UserView(parent, bounds);
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
				if(editMode && superpose.not) {
					this.editData(x, y);
					if(this.numFrames < 200) { this.refresh };
				};
				if(modifiers.isAlt) { this.postCurrentValue(x, y) };
			})
			.mouseMoveAction_({ |v, x, y, modifiers|
				cursorPos = x @ y;
				if(editMode && superpose.not) {
					this.editData(x, y);
					if(this.numFrames < 200) { this.refresh };
				};
				if(modifiers.isAlt) { this.postCurrentValue(x, y) };
			})
			.mouseUpAction_({
				cursorPos = nil;
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
								this.specs = specs.collect(_.zoom(3/2));
								normalized = false;
							}
						},
						// y zoom in / font zoom
						$+, {
							if(modifiers.isCtrl) {
								plots.do(_.zoomFont(2));
							} {
								this.specs = specs.collect(_.zoom(2/3));
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
						}
					);
					parent.refresh;
				};
			});
	}

	makeButtons {
		Button(parent, Rect(parent.view.bounds.right - 16, 8, 14, 14))
				.states_([["?", Color.black, Color.clear]])
				.focusColor_(Color.clear)
				.resize_(3)
				.font_(Font( Font.defaultSansFace, 9 ))
				.action_ { this.class.openHelpFile };
	}


	value_ { |arrays|
		this.setValue(arrays, findSpecs, true)
	}

	setValue { |arrays, findSpecs = true, refresh = true|
		value = arrays;
		data = this.prReshape(arrays);
		if(findSpecs) {
				this.calcSpecs;
				this.calcDomainSpecs;
		};
		this.updatePlotSpecs;
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
		bounds = this.drawBounds;
		this.updatePlotBounds;
		Pen.use {
			plots.do { |plot| plot.draw };
		}
	}

	drawBounds {
		^interactionView.bounds.insetBy(9, 8)
	}

	// subviews

	updatePlotBounds {
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


	calcSpecs { |separately = true|
		specs = (specs ? [\unipolar.asSpec]).clipExtend(data.size);
		if(separately) {
			this.specs = specs.collect { |spec, i|
				var list = data.at(i);
				list !? { spec = spec.calcRange(list.flat).roundRange };
			}
		} {
			this.specs = specs.first.calcRange(data.flat).roundRange;
		}
	}


	calcDomainSpecs {
		// for now, a simple version
		domainSpecs = data.collect { |val|
				[0, val.size - 1, \lin, 1].asSpec
		}
	}


	// interaction


	pointIsInWhichPlot { |point|
		var res = plots.detectIndex { |plot|
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

	// private implementation

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



// for now, use plot2.


+ ArrayedCollection {

	plot2 { |name, bounds, discrete=false, numChannels, minval, maxval|
		var array = this.as(Array), plotter = Plotter(name, bounds);
		if(discrete) { plotter.plotMode = \points };

		numChannels !? { array = array.unlace(numChannels) };
		plotter.setValue(array, true, false);

		minval !? { plotter.minval = minval; };
		maxval !? { plotter.maxval = maxval };
		plotter.refresh;

		^plotter
	}

}

+ Collection {

	plotHisto { arg steps = 100, min, max;
			var histo = this.histo(steps, min, max);
			var plotter = histo.plot2;
			plotter.domainSpecs = [[min ?? { this.minItem }, max ?? { this.maxItem }].asSpec];
			plotter.specs = [[0, histo.maxItem, \linear, 1].asSpec];
			plotter.plotMode = \steps;
			^plotter
	}

}


+ Function {

	plot2 { |duration = 0.01, server, bounds, minval, maxval|
		var name = this.asCompileString, plotter;
		if(name.size > 50) { name = "function plot" };
		plotter = [0].plot2(name, bounds);
		server = server ? Server.default;
		server.waitForBoot {
			this.loadToFloatArray(duration, server, { |array, buf|
				var numChan = buf.numChannels;
				{
					plotter.value = array.unlace(buf.numChannels).collect(_.drop(-1));
					plotter.domainSpecs = (ControlSpec(0, duration, units: "s"));
					minval !? { plotter.minval = minval; };
					maxval !? { plotter.maxval = maxval };
					plotter.refresh;

				}.defer
			})
		};
		^plotter
	}

}

+ Wavetable {

	plot2 { |name, bounds, minval, maxval|
		^this.asSignal.plot2(name, bounds, minval: minval, maxval: maxval)
	}
}

+ Buffer {

	plot2 { |name, bounds, minval, maxval|
		var plotter;
		if(server.serverRunning.not) { "Server % not running".format(server).warn; ^nil };
		plotter = [0].plot2(
			name ? "Buffer plot (bufnum: %)".format(this.bufnum),
			bounds, minval: minval, maxval: maxval
		);
		this.loadToFloatArray(action: { |array, buf|
			{
				plotter.value = array.unlace(buf.numChannels);
				plotter.setProperties(\labelX, "frames");
			}.defer
		});
		^plotter
	}
}

+ Env {

	plot2 { |size = 400, bounds, minval, maxval|
		var plotter = this.asSignal(size)
			.plot2("envelope plot", bounds, minval: minval, maxval: maxval);
		plotter.domainSpecs = ControlSpec(0, this.times.sum, units: "s");
		plotter.setProperties(\labelX, "time");
		plotter.refresh;
		^plotter
	}

}
