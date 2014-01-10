/*
TODO:
- filtering
- lock/unlockFocus (QPen stack)
- accelerated (control over QImage/QPixmap conversion; necessary?)
*/

Image {
	classvar <compositingOperations, <allPlotWindows, <resizeModes;
	var dataptr, finalizer;
	var <>name, <url, <>scalesWhenResized=false;

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

		resizeModes = [
			'doNotScale',
			'ignoreAspectRatio',
			'keepAspectRatio',
			'keepAspectRatioByExpanding'
		];
	}

	*new { arg multiple, height = nil;
		var ret;

		case
		{ multiple.isKindOf(Point) } {
			ret = this.newEmpty(multiple.x, multiple.y);
		}

		{ multiple.isKindOf(Number) } {
			ret = this.newEmpty(multiple, height ? multiple);
		}

		{ multiple.isKindOf(String) } {
			if (multiple.beginsWith("http://").not
				and:{ multiple.beginsWith("file://").not }
				and:{ multiple.beginsWith("ftp://").not  }, {
					ret = this.open(multiple);
				}, {
					ret = this.openURL(multiple);
			});
		}

		{
			Error("Image: wrong arguments to constructor").throw
		};

		^ret;
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
		^super.new.prNewPath(path);
	}

	*openURL { arg url, timeout = 60;
		^super.new.prNewURL(url, timeout).url_(url);
	}

	*fromImage { arg image;
		if(image.isKindOf(this), {
			^image.copy;
		});
		"Image: invalid instance to copy from.".error;
		^nil
	}

	*fromWindow { arg window, rect;
		^super.new.prNewFromWindow(
			window.asView,
			rect ?? { window.asView.bounds }
		);
	}

	// Class variables attributes
	*formats { arg rw = "r";
		case(
			{ rw.asSymbol == \r }, { rw = 0 },
			{ rw.asSymbol == \w }, { rw = 1 },
			{ true },
			{ Error("Image.formats(rw) must be either 'r' or 'w'").throw }
		);
		^this.prFormats(rw);
	}

	*interpolations {
		^[\fast, \smooth]; // TODO
	}

	*colorToPixel { arg color;
		_QImage_ColorToPixel;
		^this.primitiveFailed;
	}

	*pixelToColor { arg pixel;
		_QImage_PixelToColor;
		^this.primitiveFailed;
	}

	// Instance methods

	isValid {
		^dataptr.notNil;
	}

	interpolation {
		^if(this.smooth, \smooth, \fast);
	}
	interpolation_ { arg mode;
		switch (mode,
			\smooth, { this.smooth = true },
			\fast, { this.smooth = false },
			{ Error("Image: Interpolation mode invalid").throw }
		);
	}

	smooth { _QImage_HasSmoothTransformation; ^this.primitiveFailed }
	smooth_ { arg smooth; _QImage_SetSmoothTransformation; ^this.primitiveFailed }

	url_ { arg newURL;
		if(newURL.isKindOf(String), {
			url = newURL.standardizePath.replace(" ", "%20");
		}, {
			url = "";
		});
	}

	bounds {
		^Rect(0, 0, this.width, this.height);
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

	setSize { arg width, height, resizeMode;
		if (resizeMode.isNil) {
			resizeMode = if (scalesWhenResized, \ignoreAspectRatio, \doNotScale);
		};
		resizeMode = resizeModes.indexOf(resizeMode);
		if (resizeMode.isNil) {
			Error("Image: invalid resize mode.").throw;
		};
		this.prSetSize(width, height, resizeMode);
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

	setColor { arg color, x, y;
		_QImage_SetColor
		^this.primitiveFailed
	}

	getColor { arg x, y;
		_QImage_GetColor
		^this.primitiveFailed;
	}

	pixels {
		var pixelArray;
		if(this.width == 0 or: { this.height == 0 }, { ^nil });
		pixelArray = Int32Array.newClear(this.width * this.height);
		this.loadPixels(pixelArray);
		^pixelArray;
	}

	loadPixels {arg array, region = nil, start = 0;
		this.prTransferPixels(array, region, start, false);
		^this;
	}

	pixels_ {arg array;
		this.setPixels(array);
	}

	setPixels { arg array, region = nil, start = 0;
		this.prTransferPixels(array, region, start, true);
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

	draw { arg aFunction;
		this.prSetPainter;
		protect({
			aFunction.value(this);
		}, {
			this.prUnsetPainter;
		});
	}

	drawAtPoint { arg point, fromRect, operation = 'sourceOver', fraction = 1.0;
		Pen.drawImage( point, this, fromRect, operation, fraction );
	}

	drawInRect { arg rect, fromRect, operation = 'sourceOver', fraction = 1.0;
		// TODO: check stretch behavior
		Pen.drawImage(rect, this, fromRect, operation, fraction);
	}

	tileInRect { arg rect, fromRect, operation = 'sourceOver', opacity = 1.0;
		Pen.tileImage(rect, this, fromRect, operation, opacity);
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

	prSetSize { arg width, height, resizeMode;
		_QImage_SetSize
		^this.primitiveFailed
	}

	prTransferPixels { arg array, region, start, store = false;
		_QImage_TransferPixels
		^this.primitiveFailed
	}
}
