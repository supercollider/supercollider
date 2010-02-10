
Plot {

	var <>plotter, <value, <>spec, <>domainSpec;
	var <bounds, <plotBounds;
	var <>font, <>gridColorX, <>gridColorY, <>plotColor, <>backgroundColor;
	var	<>gridLinePattern, <>gridLineSmoothing;
	var <>gridOnX = true, <>gridOnY = true;
	
	var valueCache;
	
	*initClass {
		GUI.skin.put(\plot, (
			gridColorX: Color.grey(0.5),
			gridColorY: Color.grey(0.5),
			plotColor: Color.black,
			background: Color.new255(240, 240, 240),
			gridLinePattern: FloatArray[1, 4],
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
		// reshapeLike
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
	
	var <parent, <bounds, <value;
	var <plots, <interactionView, <plotSpecs, <plotDomainSpecs;
	var <>gapY = 2;
	var <>cursorPos, <>plotMode = \lines, editMode = false;
	var <>resolution = 1, <>findSpecs = true;
	var modes;
	
	*new { |parent, bounds|
		^super.newCopyArgs(parent, bounds).init
	}
	
	init {
		bounds = bounds ? Rect(100, 200, 500, 300);
		modes = [\points, \levels, \lines].iter.loop;
		interactionView = UserView(parent.tryPerform(\parent) ? parent, bounds);

		if(parent.respondsTo('drawFunc_')) {
			parent.drawFunc = { this.draw };
		} {
			if(parent.respondsTo('drawHook_')) {
				parent.drawHook = { this.draw }; // this should be fixed in the classes 
										// (but needs to be done also in swingosc)
			} {
				interactionView.drawFunc = { this.draw };
			}
		};
		
		interactionView
			.background_(Color.clear)
			.focusColor_(Color.clear)
			.resize_(5)
			.focus(true)
			.mouseDownAction_({ |v, x, y, modifiers|
				if(editMode) {
					this.editData(x, y);
					if(this.numFrames < 100) { parent.refresh };
				} {
					if(modifiers.isAlt) { 
						this.postCurrentValue(x, y) 
					};
				}
			})
			.mouseMoveAction_({ |v, x, y, modifiers|
				if(editMode) {
					this.editData(x, y);
					if(this.numFrames < 100) { parent.refresh };
				} {
					if(modifiers.isAlt) { this.postCurrentValue(x, y) };
				}
			})
			.mouseUpAction_({
				if(editMode) { parent.refresh };
			})
			.keyDownAction_({ |view, char, modifiers, unicode, keycode|
				if(modifiers.isCmd.not) {
				switch(char,
					// zoom out
					$-, {
						this.setSpecs(*plotSpecs.collect(_.zoom(3/2)));
					},
					// zoom in
					$+, {
						this.setSpecs(*plotSpecs.collect(_.zoom(2/3)));
					},
					// toggle grid
					$g, {
						plots.do { |x| x.gridOnY = x.gridOnY.not };
					},
					// toggle domain grid
					$G, {
						plots.do { |x| x.gridOnX = x.gridOnX.not };
					},
					// toggle plot mode
					$m, {
						plotMode = modes.next;
					},
					// toggle editing
					$e, {
						this.toggleEditMode;
					}
				);
				parent.refresh;
				};
			});
	}
	
	
	value_ { |arrays|
		value = this.reshape(arrays);
		if(findSpecs) { this.calcSpecs };
		this.updatePlotSpecs;
		this.updatePlots;
		this.draw;
	}
	
	reshape { |item|
		var array = item.asArray;
		if(item.first.isSequenceableCollection.not) {
			array = array.bubble;
		};
		^array
	}
	
	numChannels {
		^value.size
	}
	
	numFrames {
		if(value.isNil) { ^0 };
		^value.first.size
	}

	
	bounds_ { |rect|
		bounds = rect;
		this.updatePlotBounds;
	}
	
	
	draw {
		try { this.bounds = parent.view.bounds.insetBy(11, 10) };
		Pen.use {
			plots.do { |plot| plot.draw };
		}
	}
	
	// subviews
	
	updatePlotBounds {
		var deltaY = if(this.numChannels > 1 ) { gapY } { 0.0 };
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
				plot.spec = plotSpecs.at(i)
			}
		}
	}
	
	// specs
	
	setSpecs { |... specs|
		plotSpecs = specs.collect(_.asSpec).clipExtend(this.numChannels);
		this.refresh;
	}
	
	setDomainSpecs { |... specs|
		plotDomainSpecs = specs.collect(_.asSpec).clipExtend(this.numChannels).postln;
		plots.do { |plot, i| plot.domainSpec = plotDomainSpecs.at(i) };
		this.updatePlotSpecs; this.draw;
	}
	
	minval_ { |val|
		val = val.asArray;
		plotSpecs.do { |x, i| x.minval = val.wrapAt(i) };
		this.updatePlotSpecs; this.draw;
	}
	
	maxval_ { |val|
		val = val.asArray;
		plotSpecs.do { |x, i| x.minval = val.wrapAt(i) };
		this.updatePlotSpecs; this.draw;
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
	
	toggleEditMode {
		editMode = editMode.not;
		"editMode: %\n".postf(editMode);
	}
	
	refresh {
		this.updatePlotSpecs;
		this.draw;
	}
	
}


PlotWindow : Window {
	var <plotter;
	
	*new { arg name, bounds;
		bounds = bounds ? Rect(20, 30, 500, 300);
		^super.new(name ? "plot", bounds).init
	}
	
	init {
		this.view.background = Color.grey(0.7);
		plotter = Plotter.new(this, this.view.bounds.insetBy(10, 10));
		this.refresh;
	}
		
	value_ { |arrays|
		plotter.value_(arrays);
		this.refresh;
	}
	
	value {
		^plotter.value
	}
	
	plotMode_ { |mode|
		plotter.plotMode = mode;
		this.refresh;
	}
	
	setSpecs { |... specs|
		plotter.setSpecs(*specs);
		this.refresh;
	}
	
	setDomainSpecs { |... specs|
		plotter.setDomainSpecs(*specs);
		this.refresh;
	}

	minval_ { |val|
		plotter.minval_(val);
		this.refresh;
	}
	
	maxval_ { |val|
		plotter.maxval_(val);
		this.refresh;
	}
	
	
}


// for now, use plot2.


+ ArrayedCollection {

	plot2 { arg name, bounds, discrete=false, numChannels, minval, maxval;
		var array = this.as(Array), plotWindow = PlotWindow(name, bounds);
		if(discrete) { plotWindow.plotMode = \points };
		
		numChannels !? { array = array.unlace(numChannels) };
		plotWindow.value = array;
		
		minval !? { plotWindow.minval = minval; };
		maxval !? { plotWindow.maxval = maxval };
		
		^plotWindow.front
	}
		
}


+ Function {

	plot2 { arg duration = 0.01, server, bounds, minval, maxval;
		var name = this.asCompileString, plotWindow;
		if(name.size > 50) { name = "function plot" };
		plotWindow = [0].plot2(name, bounds);
		server = server ? Server.default;
		server.waitForBoot {
			this.loadToFloatArray(duration, server, { |array, buf|
				var numChan = buf.numChannels;
				{
					plotWindow.value = array.unlace(buf.numChannels);
					plotWindow.plotter.setDomainSpecs(ControlSpec(0, duration, units: "s"));
					minval !? { plotWindow.minval = minval; };
					maxval !? { plotWindow.maxval = maxval };
					
				}.defer;
			})
		};
		^plotWindow
	}

}

+ Wavetable {

	plot2 { arg name, bounds, minval, maxval;
		^this.asSignal.plot2(name, bounds, minval: minval, maxval: maxval);
	}
}

+ Buffer {

	plot2 { arg name, bounds, minval = -1, maxval = 1;
		var plotWindow = [0].plot2(
			name ? "Buffer plot (num: %)".format(this.bufnum), 
			bounds, minval: minval, maxval: maxval
		);
		this.loadToFloatArray(action: { |array, buf| 
			{ plotWindow.value = array.unlace(buf.numChannels) }.defer 
		});
		^plotWindow
	}
}

+ Env {

	plot2 { arg size = 400, bounds, minval, maxval;
		var plotWindow = this.asSignal(size)
			.plot2("envelope plot", bounds, minval: minval, maxval: maxval);
		plotWindow.setDomainSpecs(ControlSpec(0, this.times.sum, units: "s"));
		^plotWindow
	}
	
}


