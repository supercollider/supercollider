// blackrain at realizedsound dot net and charles picasso - thelych at gmail dot com
// 09-2008

SCImage {
	classvar <formats, <compositingOperations, <interpolations, <allPlotWindows;
	var dataptr, <width, <height, <background, <>name, <url, <>autoMode=true, <filters, prCache, prFinalizer;

	*initClass {

		formats = [
			'tiff',	// 0
			'bmp',	// 1
			'gif',	// 2
			'jpeg',	// 3
			'png'	// 4
		];

		interpolations = [
			'default', 	// 0
			'none', 		// 1
			'low', 		// 2
			'high'		// 3
		];

		compositingOperations = [
			'clear',			// 0
			'copy',			// 1
			'sourceOver',		// 2
			'sourceIn',		// 3
			'sourceOut',		// 4
			'sourceATop',		// 5
			'destinationOver',	// 6
			'destinationIn',	// 7
			'destinationOut',	// 8
			'destinationATop',	// 9
			'xor',			// 10
			'plusDarker',		// 11
			'highlight',		// 12
			'plusLighter'		// 13
		];

	}
	*new { arg multiple, height=nil;

		if(multiple.isKindOf(Point), {
			^super.new.init(multiple.x, multiple.y);
		});

		if(multiple.isKindOf(Number), {
			^super.new.init(multiple, height ? multiple);
		});

		if(multiple.isKindOf(String), {

			if ( multiple.beginsWith("http://").not
				and:{ multiple.beginsWith("file://").not }
				and:{ multiple.beginsWith("ftp://").not  }) {

				^this.open(multiple);

			};

			^this.openURL( multiple );
		});
		^nil;
	}
	*color { arg ... args;
		var newone, color, filter;
		newone = SCImage.new(*args);
		color = args.last;
		if(color.isKindOf(Color).not, {color = Color.black});
		filter = SCImageFilter(\CIConstantColorGenerator);
		filter.color_(color);
		newone.applyFilters(filter);
		^newone;
	}
	*open { arg path;
		path = path.standardizePath;
		if ( File.exists(path) ) {
			^super.new.initFromURL("file://" +/+ path.replace(" ", "%20"));
		}{
			format("SCImage: % not found.", path).error;
			^nil
		}
	}
	*openURL { arg url;
		^super.new.initFromURL(url.replace(" ", "%20"));
	}
	*fromName { arg imageNamed;
		// return a system Image or a previously allocated instance of an Image named ...
		// tbi
	}
	*fromImage { arg scimage;
		if(scimage.isKindOf(this), {
			^scimage.copy;
		});
		"SCImage: invalid instance to copy from.".error;
		^nil
	}
	*fromWindow {arg window, rect;
		if(window.isKindOf(SCWindow).not, {
			"SCImage: fromWindowRect window argument is not instance of SCWindow.".error;
		});
		rect = rect ? window.view.bounds.moveTo(0,0);
		if(rect.isKindOf(Rect).not, {
			"SCImage: fromWindowRect rect argument is not instance of Rect.".error;
		});
		^this.prFromWindowRect(window, rect);
	}

	/**
	 *	Converts a Color instance into
	 *	a pixel datatype suitable for SCImage.
	 *	This is a 32bit packed Integer in
	 *	the RGBA format.
	 */
	*colorToPixel { arg col;
		^Integer.fromRGBA(
			(col.red * 255 ).asInteger,
			(col.green * 255).asInteger,
			(col.blue * 255 ).asInteger,
			(col.alpha * 255 ).asInteger);
	}

	*prFromWindowRect {arg window, rect;
		_SCImage_fromWindowRect
		^this.primitiveFailed;
	}

	init { arg width, height;
		this.prInit(width, height);
		filters = [];
	}
	/* not used any more
	initFromFile { arg path;
		url = "file://" ++ path.standardizePath;
		this.prInitFromFile(path);
		all = all.add(this);
		filters = [];
	}
	*/
	initFromURL { arg newURL;
		url = newURL;
		this.prInitFromURL(url);
		filters = [];
	}
	free {
		this.clearCache;
		this.prFree;
	}
	copy {
		var new;
		new = this.class.new(width, height);
		new.name_(this.name);
		new.url_(this.url);
		new.autoMode_(this.autoMode);
		new.lockFocus;

			this.drawAtPoint(0@0,nil,1,1.0);

		new.unlockFocus;
		^new
	}
	isValid {
		^dataptr.notNil
	}
	bounds { // helper
		^Rect(0,0,width?0,height?0)
	}
	write { arg path, format; // ok
		if (this.isValid) {
			path = path.standardizePath;
			format = format ?? { path.basename.splitext.at(1).asSymbol };
			format = formats.indexOf(format) ? 0;
			^this.prWriteToFile(path, format);
		};
		format("SCImage:write invalid instance.").error;
	}
	url_ { arg new_url;
		if(new_url.isKindOf(String), {
			url = new_url.standardizePath.replace(" ", "%20");
		}, {url=""});
	}
	crop {|aRect|
		var filter, transform;
		if(aRect.isKindOf(Rect).not, {
			"SCImage: bad argument for cropping image !".warn;
			^this;
		});
		aRect.top = this.height - aRect.top - aRect.height;
		if(autoMode, {this.accelerated_(true)});
		filter = SCImageFilter(\CICrop);
		filter.rectangle_(aRect);
		transform = SCImageFilter(\CIAffineTransform);
		transform.transform_([aRect.left.neg, aRect.top.neg]);
		this.applyFilters([filter, transform]);
		^this;
	}
	invert {
		if(autoMode, {this.accelerated_(true)});
		this.applyFilters(SCImageFilter(\CIColorInvert));
		^this;
	}
	interpolation {
		var index;
		index = this.prGetInterpolation;
		^this.class.interpolations[index];
	}
	interpolation_ {|mode|
		var index;
		if(mode.isKindOf(Integer), {
			this.prSetInterpolation(mode.clip(0, this.class.interpolations.size - 1));
		}, {
			index = this.class.interpolations.indexOf(mode.asSymbol);
			if(index.notNil, {
				this.prSetInterpolation(index);
			}, {
				"SCImage: bad interpolation value as argument !".error;
			});
		});
	}

	// pixel manipulation
	setPixel {|rgbaInteger, x, y|
		^this.prSetPixel(rgbaInteger, x, y);
	}
	getPixel {|x,y|
		^this.prGetPixel(x,y);
	}
	setColor {|color, x, y|
		^this.prSetColor(color, x, y);
	}
	getColor {|x, y|
		^this.prGetColor(x, y);
	}
	pixels {
		var pixelArray;
		if(autoMode, {this.accelerated_(false)});
		if(width <= 0 or:{height <= 0}, {^nil});
		pixelArray = Int32Array.newClear(width*height);
		this.prLoadPixels(pixelArray);
		^pixelArray;
	}
	loadPixels {arg array, region=nil, start=0;
		if(autoMode, {this.accelerated_(false)});
		if(array.isKindOf(Int32Array).not, {
			"SCImage: array should be an Int32Array".warn;
			^nil;
		});
		this.prLoadPixels(array, region, start);
		^this;
	}
	pixels_ {|array|
		this.setPixels(array);
	}
	setPixels {|array, region=nil, start=0|
		if(autoMode, {this.accelerated_(false)});
		if(region.isNil, {
			this.prUpdatePixels(array, start);
		}, {
			this.prUpdatePixelsInRect(array, region, start);
		});
	}
	// this method should not be called directly
	// unless you know exactly what you want to do
	// use autoMode_(true) to let the class choose the best rep internally
	// or autoMode_(false) with accelerated_ method to manually manage your representations
	// a bad management can lead to : worse performance when manipulating images
	// bad syncing issue between representations !
	accelerated_ {|aBool| // if yes ensure to use CoreImages
		_SCImage_setAccelerated
		^this.primitiveFailed;
	}

	accelerated {
		_SCImage_isAccelerated
		^this.primitiveFailed;
	}

	// filters
	addFilter {|filter|
		if(filter.isKindOf(SCImageFilter), {
			filters = filters.add(filter);
		});
	}
	removeFilter {|filter|
		// remove last occurence of a filter in the array
		filters.reverseDo {|object, i|
			if(object == filter, {
				filters.removeAt(filter.size - i);
				^this;
			});
		};
	}
	removeAllFilters {
		filters = [];
	}
	createCache { // only for Filters
		prCache = this.filteredWith(filters);
	}
	/*
		flatten the receiver with all filters added to it
	*/
	flatten {
		this.clearCache;
		this.accelerated_(true); // force acceleration just in case
		this.applyFilters(filters);
		filters = []; // clear all filters
		if(autoMode, {this.accelerated_(false)}); // ensure bitmap representation
	}

	applyFilters {|filters, crop=0, region|
		if(filters.isNil, {^this});
		// passing nil to crop says use the result extent of the filter
		// may return huge extent so check and crop image if needed !
		if(crop == 0, {crop = this.bounds});
		if(crop.isKindOf(Rect).not and:{crop.isNil.not}, {
			"SCImage: crop should be a Rect, 0, or Nil".warn;
			^this;
		});
		if(filters.isKindOf(SCImageFilter), {
			filters = [filters];
		});
		if(autoMode, {this.accelerated_(true)});
		^this.prApplyFilters(filters, true, region, crop);
	}

	// returns a copy of the receiver filtered with filter
	filteredWith {|filters, crop=0|
		if(filters.isNil, {^this});
		if(filters.isKindOf(SCImageFilter), {
			filters = [filters];
		});
		if(crop == 0, {crop = this.bounds});
		if(crop.isKindOf(Rect).not and:{crop.isNil.not}, {
			"SCImage: crop should be a Rect, 0, or Nil".warn;
			^this;
		});
		if(autoMode, {this.accelerated_(true)});
		^this.prApplyFilters(filters, false, nil, crop);
	}

	clearCache {
		if(prCache.notNil, {prCache.free; prCache=nil});
	}

	// still experimental
	applyKernel {|kernel, crop|
		if(kernel.isKindOf(SCImageKernel).not, {
			"SCImage: aKernel should be a SCImageKernel !".warn;
			^this;
		});
		if(kernel.isValid.not, {
			"SCImage: kernel does not seem to be valid !".warn;
			^this;
		});
		^this.prApplyKernel(kernel, crop, true);
	}

	// drawing the image
	lockFocus {
		if(autoMode, {this.accelerated_(false)});
		this.prLockFocus;
	}
	unlockFocus {
		if(autoMode, {this.accelerated_(false)});
		this.prUnlockFocus;
	}
	drawAtPoint { arg point, fromRect, operation='sourceOver', fraction=1.0;
		if(filters.size == 0, {
			operation = compositingOperations.indexOf(operation) ? 2;
			this.prDrawAtPoint(point, fromRect, operation, fraction);
		}, {
			this.clearCache;
			this.accelerated_(true); // we have to force acceleration
			this.createCache;
			prCache.drawAtPoint(point, fromRect, operation, fraction);
		});
	}
	drawInRect { arg rect, fromRect, operation='sourceOver', fraction=1.0;
		if(filters.size == 0, {
			operation = compositingOperations.indexOf(operation) ? 2;
			this.prDrawInRect(rect, fromRect, operation, fraction);
		}, {
			this.clearCache;
			this.accelerated_(true); // we have to force acceleration
			this.createCache;
			prCache.drawInRect(rect, fromRect, operation, fraction);
		});
	}
	tileInRect { arg rect, fromRect, operation='sourceOver', fraction=1.0;		if(filters.size == 0, {
			this.prTileInRect(rect, fromRect ? this.bounds,
				compositingOperations.indexOf(operation) ? 2, fraction);
		}, {
			this.clearCache;
			this.accelerated_(true); // we have to force acceleration
			this.createCache;
			prCache.tileInRect(rect, fromRect, operation, fraction);
		})
	}
	draw {|aFunction|
		this.lockFocus;
		aFunction.value(this);
		this.unlockFocus;
	}

	// string drawing support
	drawStringAtPoint { arg string, point, font, color;
		var strbounds;
		if (string.notNil) {
			strbounds = string.bounds(font);
			SCPen.use { // for now
				SCPen.translate(0, this.height);
				SCPen.scale(1,-1);
				point.y = this.height - point.y - strbounds.height;
				string.drawAtPoint(point, font, color);
			}
		}
	}

	// simple convenient function
	plot { arg name, bounds, freeOnClose=false, background=nil, showInfo=true;
		var uview, window, nw, nh, ratio = width / height, info="";
		nw = width.min(600).max(200);
		nh = nw / ratio;
		window = SCWindow.new(name ? "plot", bounds ? Rect(400,400,nw,nh)/*, textured: false*/);
		allPlotWindows = allPlotWindows.add(window);

		if(background.notNil, {
			window.view.background_(background);
		});
		window.acceptsMouseOver = true;

		uview = SCUserView(window, window.view.bounds)
			.resize_(5)
			.focusColor_(Color.clear);

		window.onClose_({
			allPlotWindows.remove(window);
			if(freeOnClose, {
				this.free
			});
		});
		uview.drawFunc_({

			SCPen.use {
				this.drawInRect(window.view.bounds, this.bounds, 2, 1.0);
			};

			if(showInfo, {
				SCPen.use {
					SCPen.width_(0.5);
					Color.black.alpha_(0.4).setFill;
					Color.white.setStroke;
					SCPen.fillRect(Rect(5.5,5.5,100,20));
					SCPen.strokeRect(Rect(5.5,5.5,100,20));
					info.drawAtPoint(10@10, Font.default, Color.white);
				}
			});
		});
		uview.mouseOverAction_({|v, x, y|
			if(showInfo, {
				if (this.isValid) {
					info = format("X: %, Y: %",
					((x / window.view.bounds.width) * this.width).floor.min(width-1),
					((y / window.view.bounds.height) * this.height).floor.min(height-1) );
				}{
					info = "invalid image";
				};
				window.view.refreshInRect(Rect(5.5,5.5,100,20));
			});
		});
		^window.front;
	}
	*closeAllPlotWindows {
		allPlotWindows.do(_.close);
	}

	storeOn { arg stream;
		stream << this.class.name << ".openURL(" << url.asCompileString <<")"
	}

	archiveAsCompileString { ^true }

	// cocoa bridge additions
	asNSObject {
		^dataptr.asNSReturn
	}

//
	scalesWhenResized {
		_SCImage_scalesWhenResized
		^this.primitiveFailed
	}
	scalesWhenResized_ { arg flag; // to test
		_SCImage_setScalesWhenResized
		^this.primitiveFailed
	}

	width_ { arg w;
		this.setSize(w, height);
	}
	height_ { arg h;
		this.setSize(width, h);
	}
	setSize { arg width, height; // to test
		_SCImage_setSize
		^this.primitiveFailed
	}

// primtives
	// pixel manipulation private
	prSetPixel {|rgbaInteger, x, y|
		_SCImage_setPixelAt
		^this.primitiveFailed
	}
	prGetPixel {|x,y|
		_SCImage_getPixelAt
		^this.primitiveFailed
	}
	prSetColor {|color, x, y|
		_SCImage_setColorAt
		^this.primitiveFailed
	}
	prGetColor {|x, y|
		_SCImage_getColorAt
		^this.primitiveFailed
	}
	prApplyFilters {|filterArray, inPlaceBolean, region, maxSize|
		_SCImageFilter_ApplyMultiple
		^this.primitiveFailed;
	}
	prApplyKernel {|kernel, crop, inPlace |
		_SCImageFilter_ApplyKernel
		^this.primitiveFailed;
	}
	prGetInterpolation {
		_SCImage_interpolation
		^this.primitiveFailed;
	}

	prSetInterpolation {|index|
		_SCImage_setInterpolation
		^this.primitiveFailed;
	}

	prDrawAtPoint { arg point, fromRect, operation, fraction;
		_SCImage_DrawAtPoint
		^this.primitiveFailed
	}

	prDrawInRect { arg rect, fromRect, operation, fraction;
		_SCImage_DrawInRect
		^this.primitiveFailed
	}

	prSetName { arg newName; // currently does nothing
		_SCImage_setName
		^this.primitiveFailed
	}

	prSetBackground { arg color; // currently does nothing
		_SCImage_setBackgroundColor
		^this.primitiveFailed
	}

	prSync { 	// should never be used -- be provided in case
		_SCImage_sync
		^this.primitiveFailed
	}

	//
	prInit { arg width, height;
		_SCImage_New
		^this.primitiveFailed
	}
	/* not used any more
	prInitFromFile { arg path;
		_SCImage_NewFromFile
		^this.primitiveFailed
	}
	*/
	prInitFromURL { arg url;
		_SCImage_NewFromURL
		^this.primitiveFailed
	}
	prLockFocus {
		_SCImage_lockFocus
		^this.primitiveFailed
	}
	prUnlockFocus {
		_SCImage_unlockFocus
		^this.primitiveFailed
	}
	prFree {
		_SCImage_Free
		^this.primitiveFailed
	}
	*prFreeAll {
		_SCImage_FreeAll
		^this.primitiveFailed
	}
	prLoadPixels {arg array, region, startIndex;
		_SCImage_loadPixels
		^this.primitiveFailed
	}
	prUpdatePixels {arg array, startIndex;
		_SCImage_updatePixels
		^this.primitiveFailed
	}
	prUpdatePixelsInRect {arg array, rect, startIndex;
		_SCImage_updatePixelsInRect
		^this.primitiveFailed
	}
	prTileInRect { arg rect, fromRect, operation, fraction;
		_SCImage_tileInRect
		^this.primitiveFailed
	}
	prWriteToFile { arg path, format;
		_SCImage_WriteToFile
		^this.primitiveFailed
	}
}

SCImageFilter {
	classvar categories;
	var <name, <attributes, <values, <>enable=true;

	*filterCategories {
		^categories;
	}

	*initClass {
		var categoryNames = [
			\CICategoryDistortionEffect,
			\CICategoryGeometryAdjustment,
			\CICategoryCompositeOperation,
			\CICategoryHalftoneEffect,
			\CICategoryColorAdjustment,
			\CICategoryColorEffect,
			\CICategoryTransition,
			\CICategoryTileEffect,
			\CICategoryGenerator,
			\CICategoryGradient,
			\CICategoryStylize,
			\CICategorySharpen,
			\CICategoryBlur,
			\CICategoryVideo,
			\CICategoryStillImage,
			\CICategoryInterlaced,
			\CICategoryNonSquarePixels,
			\CICategoryHighDynamicRange,
			\CICategoryDistortionEffect,
			\CICategoryBuiltIn
		];

		categories = IdentityDictionary.new;

		Platform.when(#[\_SCImageFilter_NamesInCategory], {

			categoryNames.do {|key|
				categories.add(key -> this.getFilterNames(key));
			};

			//"SCImage filter categories done !".postln;
		});

	}

	*new {|filterName, args|
		^super.newCopyArgs(filterName.asSymbol).initSCImageFilter(args);
	}

	defaults {
		values = Array.new;
	}

	initSCImageFilter {|arguments|
		attributes = this.class.getFilterAttributes(name);
		values = Array.new;
		this.attributes_(arguments);
	}

	*translateObject {|object|
		if(object.isKindOf(Boolean), {
			^ if(object == true, {1}, {0});
		});

		if(object.isKindOf(Rect), {
			^ [object.left, object.top, object.width, object.height];
		});

		^ object; // no translation
	}

	doesNotUnderstand { arg selector ... args;
		var key, index;
		if(selector.isSetter && attributes.includesKey(selector.asGetter), {
			key = selector.asGetter;
			args[0] = this.class.translateObject(args[0]);
			if(args[0].isKindOf(attributes.at(key)),
			{
				index = values.indexOf(key);
				if(index.notNil, {
					values[index+1] = args[0];
				}, {
					values = values.add(key);
					values = values.add(args[0]);
				});
			}, {
				("SCImageFilter: invalid value for filter attribute: "+key+"(a"+args[0].class.asString++") -> argument should be of class:"+attributes.at(key)).error;
			});
		}, {
			^attributes.at(selector);
		});
	}

	*getFilterNames {|category|
		^this.prGetFilterNames(category, Array.newClear(64));
	}

	*getFilterAttributes {|filterName|
		var key, class;
		var array, result;
		array = this.prGetFilterAttributes(filterName);
		result = IdentityDictionary.new;

		(array.size >> 1).do {|i|
			i = i << 1;
			key = array[i];
			class = array[i+1];

			switch(class,
				\NSNumber,	{class = \Number},
				\CIVector, 	{class = \Array},
				\CIImage, 	{class = \SCImage},
				\CIColor, 	{class = \Color},
				\NSAffineTransform, {class = \Array},
				\NSPoint,		{class = \Point}
			);

			result.put(key.asSymbol, class.asClass);
		};

		^result;
	}

	attributeRange { |attributeName|
		var result = [nil, nil, nil];
		this.prAttributeRange(attributeName.asSymbol, result);
		^result;
	}

	// new
	attributes_ {|array|
		var method, value, max;
		if(array.isNil, {^this});
		max = array.size.asInteger >> 1;
		max.do {|i|
			method = array[i << 1];
			value = array[(i << 1) + 1];
			this.perform(method.asSymbol.asSetter, value);
		};
	}

	set {arg ... values;
		this.attributes_(values);
	}

	prAttributeRange { |attr|
		_SCImageFilter_GetAttributeMinMax
		^this.primitiveFailed
	}

	*prGetFilterNames {|cat, array|
		_SCImageFilter_NamesInCategory
		^this.primitiveFailed
	}

	// direct primitive call - should not be used !!!
	*prFilterSet{ |filterName, filterArguments|
		_SCImageFilter_Set
		^this.primitiveFailed
	}

	*prGetFilterAttributes {|filterName|
		_SCImageFilter_Attributes
		^this.primitiveFailed
	}
}

SCImageKernel {
	var <>shader, <>values, <>bounds, <>enabled, dataptr, finalizer;
	*new {|shader, values, bounds|
		if(values.isKindOf(Array).not and:{values.isNil.not}, {
			"SCImageKernel values should be an Array !".warn;
			^nil;
		});
		if(shader.isKindOf(String).not and:{shader.isNil.not}, {
			"SCImageKernel shader should be a String !".warn;
			^nil;
		});
		if(bounds.isKindOf(Rect).not and:{bounds.isNil.not}, {
			"SCImageKernel shader should be a Rect !".warn;
			^nil;
		});

		^super.newCopyArgs(shader, values, bounds, true);
	}

	isValid {
		^(shader.notNil.or(shader.size > 0).and(values.notNil.or(values.size <= 0)));
	}

	compile {
		_SCImageKernel_Compile
		^this.primitiveFailed
	}
}


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
*/

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
