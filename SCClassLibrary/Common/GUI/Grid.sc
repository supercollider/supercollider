

DrawGrid {

	var <bounds,<>x,<>y;
	var <>opacity=0.7,<>smoothing=false,<>linePattern;
	var pen;

	*new { |bounds,horzGrid,vertGrid|
		^super.new.init(bounds, horzGrid, vertGrid)
	}
	*test { arg horzGrid,vertGrid,bounds;
		var w,grid;
		bounds = bounds ?? {Rect(0,0,500,400)};
		grid = DrawGrid(bounds,horzGrid,vertGrid);
		w = Window("Grid",bounds).front;
		UserView(w,bounds ?? {w.bounds.moveTo(0,0)})
			.resize_(5)
			.drawFunc_({ arg v;
				grid.bounds = v.bounds;
				grid.draw
			})
			.background_(Color.white)
		^grid
	}

	init { arg bounds,h,v;
		var w;
		pen = GUI.pen;
		x = DrawGridX(h);
		y = DrawGridY(v);
		this.bounds = bounds;
		this.font = Font( Font.defaultSansFace, 9 );
		this.fontColor = Color.grey(0.3);
		this.gridColors = [Color.grey(0.7),Color.grey(0.7)];
	}
	bounds_ { arg b;
		bounds = b;
		x.bounds = b;
		y.bounds = b;
	}
	draw {
		pen.push;
			pen.alpha = opacity;
			pen.smoothing = smoothing;
			if(linePattern.notNil) {Pen.lineDash_(linePattern)};
			x.commands.do({ arg cmd; pen.perform(cmd) });
			y.commands.do({ arg cmd; pen.perform(cmd) });
		pen.pop;
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
	var <>font,<>fontColor,<>gridColor,<>labelOffset;
	var commands,cacheKey;

	*new { arg grid;
		^super.newCopyArgs(grid.asGrid).init
	}

	init {
		range = [grid.spec.minval, grid.spec.maxval];
		labelOffset = 4 @ -10;
	}
	grid_ { arg g;
		grid = g.asGrid;
		range = [grid.spec.minval, grid.spec.maxval];
		this.clearCache;
	}
	setZoom { arg min,max;
		range = [min,max];
	}
	commands {
		var p;
		if(cacheKey != [range,bounds],{ commands = nil });
		^commands ?? {
			cacheKey = [range,bounds];
			commands = [];
			p = grid.getParams(range[0],range[1],bounds.left,bounds.right);
			p['lines'].do { arg val;
				// value, [color]
				var x;
				val = val.asArray;
				x = val[0].linlin(range[0],range[1],bounds.left,bounds.right);
				commands = commands.add( ['strokeColor_',val[1] ? gridColor] );
				commands = commands.add( ['line', Point( x, bounds.top), Point(x,bounds.bottom) ] );
				commands = commands.add( ['stroke' ] );
			};
			if(bounds.width >= 12	,{
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );
				p['labels'].do { arg val;
					var x;
					// value, label, [color, font]
					if(val[2].notNil,{
						commands = commands.add( ['color_',val[2] ] );
					});
					if(val[3].notNil,{
						commands = commands.add( ['font_',val[3] ] );
					});
					x = val[0].linlin(range[0],range[1],bounds.left,bounds.right);
					commands = commands.add( ['stringAtPoint', val[1].asString, Point(x, bounds.bottom) + labelOffset ] );
				}
			});
			commands
		}
	}
	clearCache { cacheKey = nil; }
	copy { ^super.copy.clearCache }
}


DrawGridY : DrawGridX {

	init {
		range = [grid.spec.minval, grid.spec.maxval];
		labelOffset = 4 @ 4;
	}
	commands {
		var p;
		if(cacheKey != [range,bounds],{ commands = nil });
		^commands ?? {
			commands = [];

			p = grid.getParams(range[0],range[1],bounds.top,bounds.bottom);
			p['lines'].do { arg val;
				// value, [color]
				var y;
				val = val.asArray;
				y = val[0].linlin(range[0],range[1],bounds.bottom,bounds.top);
				commands = commands.add( ['strokeColor_',val[1] ? gridColor] );
				commands = commands.add( ['line', Point( bounds.left,y), Point(bounds.right,y) ] );
				commands = commands.add( ['stroke' ] );
			};
			if(bounds.height >= 20	,{
				commands = commands.add(['font_',font ] );
				commands = commands.add(['color_',fontColor ] );
				p['labels'].do { arg val,i;
					var y;
					y = val[0].linlin(range[0],range[1],bounds.bottom,bounds.top);
					if(val[2].notNil,{
						commands = commands.add( ['color_',val[2] ] );
					});
					if(val[3].notNil,{
						commands = commands.add( ['font_',val[3] ] );
					});
					commands = commands.add( ['stringAtPoint', val[1].asString, Point(bounds.left, y) + labelOffset ] );
				}
			});
			commands
		}
	}
}


// DrawGridRadial : DrawGridX {}


GridLines {

	var <>spec;

	*new { arg spec;
		^super.newCopyArgs(spec.asSpec)
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
		^this.ideals(min,max).at( [ 0,1] )
	}
	getParams { |valueMin,valueMax,pixelMin,pixelMax,numTicks|
		var lines,p,pixRange;
		var nfrac,d,graphmin,graphmax,range;
		pixRange = pixelMax - pixelMin;
		if(numTicks.isNil,{
			numTicks = (pixRange / 64);
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
			p['labels'] = lines.collect({ arg val; [val, this.formatLabel(val,nfrac) ] });
		};
		^p
	}
	formatLabel { arg val, numDecimalPlaces;
		^val.round( (10**numDecimalPlaces).reciprocal).asString + (spec.units?"")
	}
}


BlankGridLines : GridLines {

	getParams {
		^()
	}
}


+ Nil {
	asGrid { ^BlankGridLines.new }
}
