
Plot {

	var <>plotter, <value, <>spec, <>domainSpec;
	var <bounds, <plotBounds;
	var <>font, <>gridColorX, <>gridColorY, <>plotColor, <>backgroundColor;
	var	<>gridLinePattern, <>gridLineSmoothing;
	var <>gridOnX = true, <>gridOnY = true;
	
	var valueCache;
	
	*initClass {
		GUI.skin.put(\plot, (
			gridColorX: Color.grey(0.6),
			gridColorY: Color.grey(0.6),
			plotColor: Color.black,
			background: Color.new255(240, 240, 240),
			gridLinePattern: FloatArray[1, 5],
			gridLineSmoothing: false
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
		};
	}
	
	bounds_ { |rect|
		plotBounds = if(rect.height > 40) {
			 rect.insetBy(font.size * 1.5, font.size * 1.5) 
		} { 
			rect 
		};
		bounds = rect;
		valueCache = nil;
	}
	
	value_ { |array|
		value = array;
		if(plotter.findSpecs or: domainSpec.isNil) { 
			domainSpec = [0, value.size, \lin, 1].asSpec
		};
		valueCache = nil;
	}
	
		
	draw {
		this.drawBackground;
		if(gridOnX) { this.drawGridX };
		if(gridOnY) { this.drawGridY };
		this.drawData;
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
			var string = val.round(0.001).asStringPrec(5).asString ++ spec.units;
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
		var gridValues, n;
		n = (plotBounds.width / 64).round(2);
		if(domainSpec.hasZeroCrossing) { n = n + 1 };
		
		gridValues = domainSpec.gridValues(n);
		if(gridOnY) { gridValues = gridValues.drop(1) };
		gridValues = gridValues.drop(-1);

		Pen.beginPath;
		
		gridValues.do { |x, i|
			var hpos = left + (domainSpec.unmap(x) * width);
			var string = x.round(0.001).asStringPrec(5) ++ domainSpec.units;
			Pen.moveTo(hpos @ base);
			Pen.lineTo(hpos @ top);
			string.drawAtPoint(hpos @ base, font, gridColorX);
		};
		
		gridColorX.set;
		this.prStrokeGrid;
	}
	
	
	drawData {
	
		var mode = plotter.plotMode;
		
		var left = plotBounds.left;
		var base = plotBounds.bottom;
		var top = plotBounds.top;
		var array = this.prResampValues;
		var step = plotBounds.width / array.size;
		var ycoord = base - (spec.unmap(array) * plotBounds.height); // measures from top left
		
		
		Pen.width = 1.0;
		plotColor = plotColor.as(Array);
		
		if(ycoord.at(0).isSequenceableCollection) { // multi channel expansion
			ycoord.flop.do { |y, i| 
				Pen.beginPath;
				this.perform(mode, y, step);
				plotColor.wrapAt(i).set;
				Pen.stroke;
			} 
		} {
			Pen.beginPath;
			plotColor.at(0).set;
			this.perform(mode, ycoord, step);
			Pen.stroke;
		};
	
	}
	
	// modes
	
	lines { |y, step|
		var x = plotBounds.left;
		Pen.moveTo(x @ y.first);
		y.do { |y, i|
			Pen.lineTo(x @ y);
			x = x + step
		}
	}
	
	points { |y, step|
		var x = plotBounds.left;
		Pen.moveTo(x @ y.first);
		y.do { |y, i|
			Pen.addArc(x @ y, 0.5, 0, 2pi);
			x = x + step
		}
	}
	
	levels { |y, step|
		var x = plotBounds.left;
		Pen.moveTo(x @ y.first);
		Pen.setSmoothing(false);
		y.do { |y, i|
			Pen.moveTo(x @ y);
			x = x + step;
			Pen.lineTo(x @ y);
		}
	}

	
	// editing
	
	editData { |x, y|
		var index = ((x - plotBounds.left) / plotBounds.width * value.size).round.asInteger;
		var curval = value.clipAt(index);
		var val = (plotBounds.bottom - y - (font.size * 1)) / plotBounds.height;
		val = spec.map(val);
		value.clipPut(index, val);
		valueCache = nil;
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
	
	*new { |name, bounds, parent|
		^super.newCopyArgs(name, bounds ? Rect(100, 200, 500, 300)).makeWindow(parent)
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
		
		modes = [\points, \levels, \lines].iter.loop;
		
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
				} {
					if(modifiers.isAlt) { 
						this.postCurrentValue(x, y) 
					};
				}
			})
			.mouseMoveAction_({ |v, x, y, modifiers|
				cursorPos = x @ y;
				if(editMode && superpose.not) {
					this.editData(x, y);
					if(this.numFrames < 100) { this.refresh };
				} {
					if(modifiers.isAlt) { this.postCurrentValue(x, y) };
				}
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
					// x zoom out (doesn't work yet)
					$*, {
						this.setDomainSpecs(*plotDomainSpecs.collect(_.zoom(3/2)))
					},
					// x zoom in (doesn't work yet)
					$_, {
						this.setDomainSpecs(*plotDomainSpecs.collect(_.zoom(2/3)))
					},
					// neutral zoom
					$=, {
						this.calcSpecs;
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
		if(findSpecs) { this.calcSpecs };
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
			}.insetBy(11, 10)
	}
	
	// subviews
	
	updatePlotBounds {
		var deltaY = if(this.numChannels > 1 ) { 3.0 } { 0.0 };
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
	
	calcCommonSpec { |spec = \unipolar|
		plotSpecs = spec.asSpec.calcRange(value.flat).roundRange.dup(this.numChannels);
	}
	
		
	// interaction
	
	setPlotMode { |mode|
		plotMode = mode;
		this.refresh;
	}
	
	pointIsInWhichPlot { |point|
		^plots.detectIndex { |plot|
			plot.bounds.containsPoint(point)
		}
	}
	
	getDataPoint { |x, y|
		var plotIndex = this.pointIsInWhichPlot(x @ y);
		var index, array;
		^plotIndex !? {
				array = value.at(plotIndex);
				index = (x - bounds.left) / bounds.width * array.size;
				index = index.round.asInteger;
				[index, array.at(index)]
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
			{ plotter.value = array.unlace(buf.numChannels) }.defer 
		});
		^plotter
	}
}

+ Env {

	plot2 { arg size = 400, bounds, minval, maxval;
		var plotter = this.asSignal(size)
			.plot2("envelope plot", bounds, minval: minval, maxval: maxval);
		plotter.setDomainSpecs(ControlSpec(0, this.times.sum, units: "s"));
		^plotter
	}
	
}


