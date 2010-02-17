
Plot {

	var <>plotter, <value, <>spec, <>domainSpec;
	var <bounds, <plotBounds;
	var <>font, <>gridColorX, <>gridColorY, <>plotColor, <>backgroundColor;
	var	<>gridLinePattern, <>gridLineSmoothing;
	var <>gridOnX = true, <>gridOnY = true, <>labelX, <>labelY;
	
	var valueCache;
	
	*initClass {
		GUI.skin.put(\plot, (
			gridColorX: Color.grey(0.7),
			gridColorY: Color.grey(0.7),
			plotColor: Color.black,
			background: Color.new255(235, 235, 235),
			//gridLinePattern: FloatArray[1, 5],
			gridLinePattern: FloatArray[1, 0],
			gridLineSmoothing: false,
			labelX: "",
			labelY: ""
		));
	}
	
	*new { |plotter|
		^super.newCopyArgs(plotter).init
	}
	
	init {
		var skin = GUI.skin.at(\plot);
		font = Font("Courier", 9);
		skin.use {
			gridColorX = ~gridColorX;
			gridColorY = ~gridColorY;
			plotColor = ~plotColor;
			backgroundColor = ~background;
			gridLineSmoothing = ~gridLineSmoothing;
			gridLinePattern = ~gridLinePattern.as(FloatArray);
			labelX = ~labelX;
			labelY = ~labelY;
		};
	}
	
	bounds_ { |rect|
		var size = "\foo".bounds(font).height;
		plotBounds = if(rect.height > 40) {
			 rect.insetBy(size * 1.5, size * 1.5) 
		} { 
			rect 
		};
		bounds = rect;
		valueCache = nil;
	}
	
	value_ { |array|
		value = array;
		valueCache = nil;
	}
	
		
	draw {
		this.drawBackground;
		if(gridOnX) { this.drawGridX };
		if(gridOnY) { this.drawGridY };
		this.drawLabels;
		this.drawData;
		plotter.drawFunc.value(this); // additional elements
	}
	
	drawBackground {
		Pen.addRect(bounds);
		backgroundColor.set;
		Pen.fill;
	}

	drawGridY {
	
		var left = plotBounds.left;
		var right = plotBounds.right;
		var base = plotBounds.bottom;
		var height = plotBounds.height;
		var n, gridValues;
		n = (plotBounds.height / 32).round(2);
		if(spec.hasZeroCrossing) { n = n + 1 };
		gridValues = spec.gridValues(n);
		
		
		Pen.beginPath;
		
		gridValues.do { |val, i|
			var vpos = base - (spec.unmap(val) * height); // measures from top left
			var string = val.asStringPrec(5).asString ++ spec.units;
			Pen.moveTo(left @ vpos);
			Pen.lineTo(right @ vpos);
			if(gridOnX.not or: { i > 0 }) {
				string.drawAtPoint(left @ vpos, font, gridColorY);
			}
		};
		gridColorY.set;
		this.prStrokeGrid;

	}
	
	drawGridX {
	
		var top = plotBounds.top;
		var base = plotBounds.bottom;
		var width = plotBounds.width;
		var left = plotBounds.left;
		var gridValues, n, xspec = this.resampledDomainSpec;
		xspec = domainSpec; //-
		n = (plotBounds.width / 64).round(2);
		if(xspec.hasZeroCrossing) { n = n + 1 };
		
		gridValues = xspec.gridValues(n);
		if(gridOnY) { gridValues = gridValues.drop(1) };
		gridValues = gridValues.drop(-1);

		Pen.beginPath;
		
		gridValues.do { |x, i|
			var hpos = left + (xspec.unmap(x) * width);
			var string = x.asStringPrec(5) ++ xspec.units;
			Pen.moveTo(hpos @ base);
			Pen.lineTo(hpos @ top);
			string.drawAtPoint(hpos @ base, font, gridColorX);
		};
		
		gridColorX.set;
		this.prStrokeGrid;
		

	}
	
	drawLabels {
		var sbounds;
		if(gridOnX and: { labelX.notNil }) { 
			sbounds = labelX.bounds(font);
			labelX.drawAtPoint(
				plotBounds.right - sbounds.width @ plotBounds.bottom, font, gridColorY
			)
		};
		if(gridOnY and: { labelY.notNil }) {
			sbounds = labelY.bounds(font);
			labelY.drawAtPoint(
				plotBounds.left - sbounds.width @ plotBounds.top, font, gridColorY
			) 
		};
	}
	
	resampledDomainSpec {
		var offset = if(plotter.plotMode == \levels) { 0 } { 1 };
		^domainSpec.copy.maxval_(this.resampledSize - offset)
	}
	
	resampledSize {
		^min(value.size, plotBounds.width / plotter.resolution)
	}
	
	indexScale { // to be used later in domain scaling
		var offset = if(plotter.plotMode == \levels) { 0 } { 1 };
		^this.resampledSize - offset / (value.size - 1)
	}
	
	domainCoordinates { |size|
		var val = this.resampledDomainSpec.unmap((0..size-1));
		^plotBounds.left + (val * plotBounds.width);
	}
	
	dataCoordinates {
		 var val = spec.unmap(this.prResampValues);
		 ^plotBounds.bottom - (val * plotBounds.height); // measures from top left (may be arrays)
	}
	
	compressionRatio {
		^if(valueCache.isNil) { 1.0 } {
			valueCache.size / value.size	
		}
	}
	
	drawData {
		
		var mode = plotter.plotMode;
		var ycoord = this.dataCoordinates;
		var xcoord = this.domainCoordinates(ycoord.size);
		
		Pen.width = 1.0;
		plotColor = plotColor.as(Array);
		
		if(ycoord.at(0).isSequenceableCollection) { // multi channel expansion
			ycoord.flop.do { |y, i| 
				Pen.beginPath;
				this.perform(mode, xcoord, y);
				plotColor.wrapAt(i).set;
				Pen.stroke;
			}
		} {
			Pen.beginPath;
			plotColor.at(0).set;
			this.perform(mode, xcoord, ycoord);
			Pen.stroke;
		};
	
	}
	
	// modes
	
	lines { |x, y|
		Pen.moveTo(x.first @ y.first);
		y.size.do { |i|
			Pen.lineTo(x[i] @ y[i]);
		}
	}
	
	points { |x, y|
		y.size.do { |i|
			Pen.addArc(x[i] @ y[i], 0.5, 0, 2pi);
		}
	}
	
	plines { |x, y|
		Pen.moveTo(x.first @ y.first);
		y.size.do { |i|
			var p = x[i] @ y[i];
			Pen.lineTo(p);
			Pen.addArc(p, 3, 0, 2pi);
			Pen.moveTo(p);
		}
	}
	
	levels { |x, y|
		Pen.setSmoothing(false);
		y.size.do { |i|
			Pen.moveTo(x[i] @ y[i]);
			Pen.lineTo(x[i + 1] ?? { plotBounds.right } @ y[i]);
		}
	}
	
	// editing
	
		
	editData { |x, y|
		var index = this.getIndex(x);
		var val = this.getRelativePositionY(y);
		value.clipPut(index, val);
		valueCache = nil;
	}
	
	getRelativePositionX { |x|
		^this.resampledDomainSpec.map((x - plotBounds.left) / plotBounds.width)
	}
	
	getRelativePositionY { |y|
		^spec.map((plotBounds.bottom - y) / plotBounds.height)
	}
	
	getIndex { |x|
		var offset = if(plotter.plotMode == \levels) { 0.5 } { 0.0 }; // needs to be fixed.
		^(this.getRelativePositionX(x) - offset).round.asInteger;
	}

	getDataPoint { |x|
		var index = this.getIndex(x);
		^[index, value.clipAt(index)]
	}

	
	// private implementation	
	
	prResampValues {
		^if(value.size <= (plotBounds.width / plotter.resolution)) { 
			value 
		} { 
			valueCache ?? { valueCache = value.resamp1(plotBounds.width / plotter.resolution) }
		};
	}
	
	prStrokeGrid {
		Pen.width = 1;
		
		try {
			Pen.setSmoothing(gridLineSmoothing);
			Pen.lineDash_(gridLinePattern);
		};
		
		Pen.stroke;
		
		try { 
			Pen.setSmoothing(true);
			Pen.lineDash_(FloatArray[1, 0]) 
		};
	}

}



Plotter {
	
	var <>name, <>bounds, <>parent;
	var <value, <data;
	var <plots, <plotSpecs, <plotDomainSpecs;
	var <cursorPos, <>plotMode = \lines, <>editMode = false;
	var <>resolution = 1, <>findSpecs = true, <superpose = false;
	var modes, <interactionView;
	
	var <>drawFunc;

	*new { |name, bounds, parent|
		^super.newCopyArgs(name, bounds ? Rect(100, 200, 400, 300)).makeWindow(parent)
	}
	
	makeWindow { |argParent|
		parent = argParent ? parent;
		if(parent.isNil) {
			parent = Window.new(name ? "Plot", bounds);
			interactionView = UserView(parent, parent.view.bounds);
			parent.drawHook = { this.draw };
			parent.front;
			parent.onClose = { parent = nil };
		} {
			interactionView = UserView(parent, bounds);
			interactionView.drawFunc = { this.draw }
		};
		
		modes = [\points, \levels, \lines, \plines].iter.loop;
		
		interactionView
			.background_(Color.clear)
			.focusColor_(Color.clear)
			.resize_(5)
			.focus(true)
			.mouseDownAction_({ |v, x, y, modifiers|
				cursorPos = x @ y;
				if(editMode && superpose.not) {
					this.editData(x, y);
					if(this.numFrames < 100) { this.refresh };
				};
				if(modifiers.isAlt) { this.postCurrentValue(x, y) };
			})
			.mouseMoveAction_({ |v, x, y, modifiers|
				cursorPos = x @ y;
				if(editMode && superpose.not) {
					this.editData(x, y);
					if(this.numFrames < 100) { this.refresh };
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
					// y zoom out
					$-, {
						this.setSpecs(*plotSpecs.collect(_.zoom(3/2)))
					},
					// y zoom in
					$+, {
						this.setSpecs(*plotSpecs.collect(_.zoom(2/3)))
					},
					/*// x zoom out (doesn't work yet)
					$*, {
						this.setDomainSpecs(*plotDomainSpecs.collect(_.zoom(3/2)))
					},
					// x zoom in (doesn't work yet)
					$_, {
						this.setDomainSpecs(*plotDomainSpecs.collect(_.zoom(2/3)))
					},*/
					// neutral zoom
					$=, {
						this.calcSpecs;
						this.updatePlotSpecs;
						this.refresh;
					},
					$n, {
						plotSpecs = plotSpecs.collect(_.normalize);
						this.updatePlotSpecs;
						this.refresh;
					},
					
					// toggle grid
					$g, {
						plots.do { |x| x.gridOnY = x.gridOnY.not }
					},
					// toggle domain grid
					$G, {
						plots.do { |x| x.gridOnX = x.gridOnX.not }
					},
					// toggle plot mode
					$m, {
						this.setPlotMode(modes.next)
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
		
	value_ { |arrays|
		this.setValue(arrays, findSpecs, true)
	}
	
	setValue { |arrays, findSpecs = true, refresh = true|
		data = arrays;
		value = this.prReshape(arrays);
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
		this.setValue(data, false, true);
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
		};
	}
	
	drawBounds { // needs to be unified in UserView and Window
		^if(parent.respondsTo(\view)) {
				parent.view.bounds
			} {
				parent.bounds
			}.insetBy(9, 8)
	}
	
	// subviews
	
	updatePlotBounds {
		var deltaY = if(this.numChannels > 1 ) { 4.0 } { 0.0 };
		var distY = bounds.height / this.numChannels;
		var height = max(20, distY - deltaY);
		
		plots.do { |plot, i|
			plot.bounds_(
				Rect(bounds.left, distY * i + bounds.top, bounds.width, height)
			)
		};
	}
	
	makePlots {
		var template = if(plots.isNil) { Plot(this) } { plots.last };
		plots !? { plots = plots.keep(value.size.neg) };
		plots = plots ++ template.dup(value.size - plots.size);
		plots.do { |plot, i| plot.value = value.at(i) };
		
		this.updatePlotSpecs;
		this.updatePlotBounds;
		
	}
	
	updatePlots {
		if(plots.size != this.numChannels) {
			this.makePlots;
		} {
			plots.do { |plot, i|
				plot.value = value.at(i)
			}
		};
	}
	
	updatePlotSpecs {
		plotSpecs !? {
			plots.do { |plot, i|
				plot.spec = plotSpecs.clipAt(i)
			}
		};
		plotDomainSpecs !? {
			plots.do { |plot, i|
				plot.domainSpec = plotDomainSpecs.clipAt(i)
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
	
	setSpecs { |... specs|
		plotSpecs = specs.collect(_.asSpec);
		this.updatePlotSpecs;
		this.refresh;
	}
	
	setDomainSpecs { |... specs|
		plotDomainSpecs = specs.collect(_.asSpec);
		this.updatePlotSpecs;
		this.refresh;
	}
	
	minval_ { |val|
		val = val.asArray;
		plotSpecs.do { |x, i| x.minval = val.wrapAt(i) };
		this.updatePlotSpecs; 
		this.refresh;
	}
	
	maxval_ { |val|
		val = val.asArray;
		plotSpecs.do { |x, i| x.minval = val.wrapAt(i) };
		this.updatePlotSpecs; 
		this.refresh;
	}
	
	
	calcSpecs { |...specs|
		plotSpecs = (specs ? plotSpecs ? \unipolar).asArray.wrapExtend(this.numChannels);
		plotSpecs = plotSpecs.collect { |spec, i|
			spec.asSpec.calcRange(value.at(i).flat).roundRange;
		};
	}
	
	calcDomainSpecs { 
		// for now, a simple version
		plotDomainSpecs = value.collect { |val|
				[0, val.size - 1, \lin, 1].asSpec
		}
	}
	
	calcCommonSpec { |spec = \unipolar|
		plotSpecs = spec.asSpec.calcRange(value.flat).roundRange.dup(this.numChannels);
	}
	
		
	// interaction
	
	setPlotMode { |mode|
		plotMode = mode;
		this.refresh;
	}
	
	pointIsInWhichPlot { |point|
		var res = plots.detectIndex { |plot|
			plot.bounds.containsPoint(point)
		};
		^res ?? { 
				if(point.x < bounds.center.x) { 0 } { plots.size - 1 } 
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
				plots.at(plotIndex).editData(x, y)
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

	plot2 { arg name, bounds, discrete=false, numChannels, minval, maxval;
		var array = this.as(Array), plotter = Plotter(name, bounds);
		if(discrete) { plotter.plotMode = \points };
		
		numChannels !? { array = array.unlace(numChannels) };
		plotter.value = array;
		
		minval !? { plotter.minval = minval; };
		maxval !? { plotter.maxval = maxval };
		
		^plotter
	}
		
}


+ Function {

	plot2 { arg duration = 0.01, server, bounds, minval, maxval;
		var name = this.asCompileString, plotter;
		if(name.size > 50) { name = "function plot" };
		plotter = [0].plot2(name, bounds);
		server = server ? Server.default;
		server.waitForBoot {
			this.loadToFloatArray(duration, server, { |array, buf|
				var numChan = buf.numChannels;
				{
					plotter.value = array.unlace(buf.numChannels);
					plotter.setDomainSpecs(ControlSpec(0, duration, units: "s"));
					minval !? { plotter.minval = minval; };
					maxval !? { plotter.maxval = maxval };
					
				}.defer;
			})
		};
		^plotter
	}

}

+ Wavetable {

	plot2 { arg name, bounds, minval, maxval;
		^this.asSignal.plot2(name, bounds, minval: minval, maxval: maxval);
	}
}

+ Buffer {

	plot2 { arg name, bounds, minval = -1, maxval = 1;
		var plotter = [0].plot2(
			name ? "Buffer plot (bufnum: %)".format(this.bufnum), 
			bounds, minval: minval, maxval: maxval
		);
		this.loadToFloatArray(action: { |array, buf| 
			{ 
				plotter.value = array.unlace(buf.numChannels);
			}.defer 
		});
		^plotter
	}
}

+ Env {

	plot2 { arg size = 400, bounds, minval, maxval;
		var plotter = this.asSignal(size)
			.plot2("envelope plot", bounds, minval: minval, maxval: maxval);
		plotter.setDomainSpecs(ControlSpec(0, this.times.sum, units: "s"));
		plotter.setProperties(\labelX, "time");
		^plotter
	}
	
}
