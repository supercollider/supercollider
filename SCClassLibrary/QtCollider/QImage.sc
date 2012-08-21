
QImage {
	var dataptr, finalizer;

	*new { arg multiple;
	}

	*newEmpty { arg width, height;
		^super.new.prNewEmpty(width, height);
	}

	//*color { arg multiple, color;
	*newColor { arg width, height, color;
		var ret;
		ret = super.new.prNewEmpty(width, height);
		ret.fill(color);
		^ret;
	}

	*open { arg path;
		path = path.standardizePath;
		if ( File.exists(path) ) {
			^super.new.prNewPath(path);
		}{
			format("QImage: % not found.", path).error;
			^nil
		}
	}

	*openURL { arg path;
		^super.new.prNewURL(path);
	}

	*fromImage { arg image;
	}

	*fromWindow { arg window, rect;
		^super.new.prNewFromWindow(
			window,
			rect ?? { window.bounds.moveTo(0, 0) }
		);
	}

	// Class variables attributes
	*formats {}

	*compositingOperations {}

	*interpolations {}

	*closeAllPlotWindows {}

	// Instance methods
	width {
		_QImage_Width
		^this.primitiveFailed
	}
	width_ { arg w;
	}

	height {
		_QImage_Height
		^this.primitiveFailed
	}
	height_ { arg h;
	}

	setPixel {
		_QImage_SetPixel
		^this.primitiveFailed
	}

	getPixel { arg x, y;
		_QImage_GetPixel
		^this.primitiveFailed
	}

	fill { arg color;
		_QImage_Fill
		^this.primitiveFailed
	}

	setSize { arg width, height;
	}

	bounds {}

	free {
		_QImage_Free
		^this.primitiveFailed
	}

	scalesWhenResized {}
	scalesWhenResized_ {}

	url {}
	url_ { arg anURL; }

	accelerated {}
	accelerated_ {}

	draw { arg aFunction;
		this.prSetPainter;
		protect({
			aFunction.value(this);
		}, {
			this.prUnsetPainter;
		});
	}

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

	prNewURL { arg path;
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

	// in QPen.drawImage, TODO: operation, fraction
	//prDrawInRect { arg rect, fromRect, operation, fraction;
	//}

	prSetPainter {
		_QImage_SetPainter
		^this.primitiveFailed
	}

	prUnsetPainter {
		_QImage_UnsetPainter
		^this.primitiveFailed
	}
}


/* *** additions of the original SCImage *** */
// integer additions to retrieve 8-bit pixel component from RGBA packed data

+Integer {
	*fromRGBA {|r, g=0, b=0, a=255|
		^(
			((r.asInteger & 16r000000FF) << 24) | ((g.asInteger & 16r000000FF) << 16) | ((b.asInteger & 16r000000FF) << 8) | (a.asInteger & 16r000000FF)
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

	red {
		^((this >> 24) & 16r000000FF);
	}
	green {
		^((this >> 16) & 16r000000FF);
	}
	blue {
		^((this >> 8) & 16r000000FF);
	}
	alpha {
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

