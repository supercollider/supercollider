/*
	Port to Qt of the SCImage class.
	Some methods are verbatim, no methods
	were harmed during porting (I hope).

	Currently there are no equivalent for
	SCImageFilter or SCImageKernel.
*/

/*
	TODO:
		- check representations
		- raw data
		- tileInRect
		- on screen interpolations (?)
		- lock/unlockFocus (?)
*/

QImage {
	classvar <compositingOperations, <allPlotWindows, <aspectRatioModes, <transformModes;
	var dataptr, finalizer;
	var <>name, <url;
	var arm, trm;

	*initClass {
		compositingOperations = [
			'sourceOver',       // 0
			'destinationOver',  // 1
			'clear',            // 2
			'copy',             // 3

			'destination',      // 4 qt name

			'sourceIn',         // 5
			'destinationIn',    // 6
			'sourceOut',        // 7
			'destinationOut',   // 8
			'sourceATop',       // 9
			'destinationATop',  // 10
			'xor',              // 11

			'plus',             // 12 qt name
			'multiply',         // 13 qt name
			'screen',           // 14 qt name
			'overlay',          // 15 qt name

			'plusDarker',       // 16 CompositionMode_Darken + plus?
			'plusLighter',      // 17 CompositionMode_Lighten + plus?
			'highlight',        // 18 CompositionMode_ColorDodge?

			'colorBurn',        // 19 qt name
			'hardLight',        // 20 qt name
			'softLight',        // 21 qt name
			'difference',       // 22 qt name
			'exclusion',        // 23 qt name
			// 24-32 RasterOp
		];

		aspectRatioModes = [
			'ignoreAspectRatio',
			'keepAspectRatio',
			'keepAspectRatioByExpanding'
		];

		transformModes = [
			'fastTransformation',
			'smoothTransformation'
		];
	}

	*new { arg multiple, height = nil;
		var ret;

		case(
		{ multiple.isKindOf(Point) }, {
			ret = this.newEmpty(multiple.x, multiple.y);
		},

		{ multiple.isKindOf(Number) }, {
			ret = this.newEmpty(multiple, height ? multiple);
		},

		{ multiple.isKindOf(String) }, {
			if (multiple.beginsWith("http://").not
				and:{ multiple.beginsWith("file://").not }
				and:{ multiple.beginsWith("ftp://").not  }, {
				ret = this.open(multiple);
			}, {
				ret = this.openURL(multiple);
			});
		});

		if(ret.isNil, {
			^nil;
		}, {
			ret.arMode = 'keepAspectRatio';
			ret.trMode = 'fastTransformation';
			^ret;
		});
	}

	*newEmpty { arg width, height;
		^super.new.prNewEmpty(width, height);
	}

	*color { arg ...args;
		var ret, color;
		ret = this.new(*args);
		color = args.last;
		if(color.isKindOf(Color).not, {
			color = Color.black;
		});
		ret.fill(color);
		^ret;
	}

	*open { arg path;
		path = path.standardizePath;
		^try({
			super.new.prNewPath(path);
		});
	}

	*openURL { arg path, timeout = 60;
		^try({
			super.new.prNewURL(path, timeout);
		});
	}

	*fromImage { arg image;
		if(image.isKindOf(this), {
			^image.copy;
		});
		"QImage: invalid instance to copy from.".error;
		^nil
	}

	*fromWindow { arg window, rect;
		^super.new.prNewFromWindow(
			window,
			rect ?? { window.bounds.moveTo(0, 0) }
		);
	}

	// Class variables attributes
	*formats { arg rw = "r";
		case(
			{ rw.asSymbol == \r }, { rw = 0 },
			{ rw.asSymbol == \w }, { rw = 1 },
			{ true },
			{ Error("QImage.formats(rw) must be either 'r' or 'w'").throw }
		);
		^this.prFormats(rw);
	}

	*interpolations {
		^[]; // TODO
	}

	*colorToPixel { arg col;
		^Integer.fromRGBA(
			(col.red * 255 ).asInteger,
			(col.green * 255).asInteger,
			(col.blue * 255 ).asInteger,
			(col.alpha * 255 ).asInteger);
	}

	// Instance methods
	arMode {
		^aspectRatioModes.at(arm);
	}
	arMode_ { arg m;
		var res;
		if((res = aspectRatioModes.indexOf(m)).notNil, {
			arm = res;
		});
	}

	trMode {
		^transformModes.at(trm);
	}
	trMode_ { arg m;
		var res;
		if((res = transformModes.indexOf(m)).notNil, {
			trm = res;
		});
	}

	scalesWhenResized { // FIX: compatibility behavior
		^(this.arMode == 'ignoreAspectRatio');
	}
	scalesWhenResized_ { arg flag, mode; // FIX: compatibility behavior
		if(flag, {
			this.arMode = 'ignoreAspectRatio';
		}, {
			if(mode.notNil, {
				this.arMode = mode;
			}, {
				this.arMode = 'keepAspectRatio';
			});
		});
	}

	width {
		_QImage_Width
		^this.primitiveFailed
	}
	width_ { arg w;
		this.setSize(w, this.height);
	}

	height {
		_QImage_Height
		^this.primitiveFailed
	}
	height_ { arg h;
		this.setSize(this.width, h);
	}

	setSize { arg width, height;
		this.prSetSize(width, height, arm, trm);
	}

	// pixel manipulation
	setPixel { arg rgbaInteger, x, y;
		_QImage_SetPixel
		^this.primitiveFailed
	}

	getPixel { arg x, y;
		_QImage_GetPixel
		^this.primitiveFailed
	}

	pixels {
		var pixelArray;
		if(this.width == 0 or: { this.height == 0 }, { ^nil });
		pixelArray = Int32Array.newClear(this.width * this.height);
		this.prLoadPixels(pixelArray, nil, 0, true);
		^pixelArray;
	}

	loadPixels {arg array, region = nil, start = 0;
		if(array.isKindOf(Int32Array).not, {
			"QImage: array should be an Int32Array".warn;
			^nil;
		});
		this.prLoadPixels(array, region, start, true);
		^this;
	}

	pixels_ {arg array;
		this.setPixels(array);
	}

	setPixels { arg array, region = nil, start = 0;
		this.prLoadPixels(array, region, start, false);
	}

	fill { arg color;
		_QImage_Fill
		^this.primitiveFailed
	}

	free {
		_QImage_Free
		^this.primitiveFailed
	}

	// TODO: make a primitive
	copy {
		var new, rect;
		rect = Rect(0, 0, this.width, this.height);
		new = this.class.newEmpty(rect.width, rect.height);
		new.name_(this.name);
		new.url_(this.url);
		new.draw({
			Pen.drawImage(rect, this, rect);
		});
		^new
	}

	isValid {
		^dataptr.notNil;
	}

	bounds {
		^Rect(0, 0, this.width, this.height);
	}

	draw { arg aFunction;
		this.prSetPainter;
		protect({
			aFunction.value(this);
		}, {
			this.prUnsetPainter;
		});
	}

	// scimage interface
	url_ { arg newURL;
		if(newURL.isKindOf(String), {
			url = newURL.standardizePath.replace(" ", "%20");
		}, {
			url = "";
		});
	}

	accelerated {}
	accelerated_ {}

	drawAtPoint { arg point, fromRect, operation = 'sourceOver', fraction = 1.0;
		var size = Point(this.width, this.height);
		Pen.drawImage(Rect(point, size), this, Rect(0@0, size), operation, fraction);
	}

	drawInRect { arg rect, fromRect, operation = 'sourceOver', fraction = 1.0;
		// TODO: check stretch behavior
		Pen.drawImage(rect, this, fromRect, operation, fraction);
	}

	tileInRect { arg rect, fromRect, operation = 'sourceOver', fraction = 1.0;
		// TODO: check stretch behavior
		//Pen.drawImage(rect, this, fromRect, operation, fraction);
	}

	drawStringAtPoint { arg string, point, font, color;
		this.draw({
			Pen.stringAtPoint(string, point, font, color);
		});
	}

	plot { arg name, bounds, freeOnClose = false, background = nil, showInfo = true;
		var uview, window, nw, nh, ratio = this.width / this.height, info = "";

		nw = this.width.min(600).max(200);
		nh = nw / ratio;
		window = Window.new(name ? "plot", bounds ? Rect(400, 400, nw, nh));
		allPlotWindows = allPlotWindows.add(window);

		if(background.notNil, {
			window.view.background_(background);
		});
		window.acceptsMouseOver = true;

		uview = UserView(window, window.view.bounds)
			.resize_(5)
			.focusColor_(Color.clear);

		window.onClose_({
			allPlotWindows.remove(window);
			if(freeOnClose, {
				this.free
			});
		});

		uview.drawFunc_({
			Pen.drawImage(window.view.bounds, this, this.bounds);

			if(showInfo, {
				Pen.use {
					Pen.width_(0.5);
					Color.black.alpha_(0.4).setFill;
					Color.white.setStroke;
					Pen.fillRect(Rect(5.5, 5.5, 100, 20));
					Pen.strokeRect(Rect(5.5, 5.5, 100, 20));
					info.drawAtPoint(10@10, Font.default, Color.white);
				}
			});
		});

		uview.mouseOverAction_({ arg v, x, y;
			if(showInfo, {
				if(this.isValid, {
					info = format("X: %, Y: %",
					((x / window.view.bounds.width) * this.width).floor.min(this.width-1),
					((y / window.view.bounds.height) * this.height).floor.min(this.height-1) );
				}, {
					info = "invalid image";
				});
				//window.view.refreshInRect(Rect(5.5,5.5,100,20));
				window.view.refresh;
			});
		});
		^window.front;
	}

	*closeAllPlotWindows {
		allPlotWindows.do(_.close);
	}

	storeOn { arg stream;
		stream << this.class.name << ".openURL(" << url.asCompileString << ")";
	}

	archiveAsCompileString { ^true }

	// saving and archiving
	write { arg path, format, quality = -1;
		_QImage_Write
		^this.primitiveFailed
	}

	// private primitives
	prNewPath { arg path;
		_QImage_NewPath
		^this.primitiveFailed
	}

	prNewURL { arg path, timeout;
		_QImage_NewURL
		^this.primitiveFailed
	}

	prNewFromWindow { arg window, rect;
		_QImage_NewFromWindow
		^this.primitiveFailed
	}

	prNewEmpty { arg width, height;
		_QImage_NewEmpty
		^this.primitiveFailed
	}

	prSetPainter {
		_QImage_SetPainter
		^this.primitiveFailed
	}

	prUnsetPainter {
		_QImage_UnsetPainter
		^this.primitiveFailed
	}

	*prFormats { arg rw;
		_QImage_Formats
		^this.primitiveFailed
	}

	prSetSize { arg width, height, arMode, trMode;
		_QImage_SetSize
		^this.primitiveFailed
	}

	prLoadPixels {arg array, region, start, geset;
		_QImage_LoadPixels
		^this.primitiveFailed
	}
}

// integer additions to retrieve 8-bit pixel component from RGBA packed data
// values were rotated to Qt's ARGB order

+Integer {
	*fromRGBA { arg r, g = 0, b = 0, a = 255;
		^(
			((a.asInteger & 16r000000FF) << 24) |
			((r.asInteger & 16r000000FF) << 16) |
			((g.asInteger & 16r000000FF) << 8) |
			(b.asInteger & 16r000000FF)
		);
	}

	*fromColor {|color|
		^this.fromRGBA(
			(color.red * 255).asInteger,
			(color.green * 255).asInteger,
			(color.blue * 255).asInteger,
			(color.alpha * 255).asInteger
		)
	}

	asColor {
		^Color.new255(this.red, this.green, this.blue, this.alpha);
	}

	rgbaArray {
		^[this.red, this.green, this.blue, this.alpha];
	}

	alpha {
		^((this >> 24) & 16r000000FF);
	}
	red {
		^((this >> 16) & 16r000000FF);
	}
	green {
		^((this >> 8) & 16r000000FF);
	}
	blue {
		^(this & 16r000000FF);
	}
}

+Array {
	asRGBA {
		^Integer.fromRGBA(this.at(0)?0,this.at(1)?0, this.at(2)?0, this.at(3)?0);
	}
}

+Color {
	asInteger {
		^Integer.fromRGBA(
			(this.red * 255).asInteger,
			(this.green * 255).asInteger,
			(this.blue * 255).asInteger,
			(this.alpha * 255).asInteger
		);
	}
}

/*
	SCView:backgroundImage

	modes :
	1 - fixed to left, fixed to top
	2 - horizontally tile, fixed to top
	3 - fixed to right, fixed to top
	4 - fixed to left, vertically tile
	5 - horizontally tile, vertically tile
	6 - fixed to right, vertically tile
	7 - fixed to left, fixed to bottom
	8 - horizontally tile, fixed to bottom
	9 - fixed to right, fixed to bottom
	10 - fit
	11 - center, center (scale)
	12 - center , fixed to top
	13 - center , fixed to bottom
	14 - fixed to left, center
	15 - fixed to right, center
	16 - center, center (no scale)
//*/

+SCView {
	backgroundImage_ { arg image, tileMode=1, alpha=1.0, fromRect;
		this.setProperty(\backgroundImage, [image, tileMode, alpha, fromRect])
	}
	refreshInRect {|b|
		_SCView_RefreshInRect
		^this.primitiveFailed
	}
}

+Rect {
	outsetBy {arg h, v;
		if(v.isNil, {v=h});
		^this.class.new(left - h, top - v, width + h + h, height + v + v);
	}
}
*/

