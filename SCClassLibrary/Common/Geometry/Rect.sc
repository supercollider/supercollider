Rect {
	var <>left=0, <>top=0, <>width=0, <>height=0;

	*new { arg left=0, top=0, width=0, height=0;
		^super.newCopyArgs(left, top, width, height)
	}
	*newSides { arg left=0, top=0, right=0, bottom=0;
		^super.newCopyArgs(left, top, right-left, bottom-top)
	}
	*fromPoints { arg pt1, pt2;
		^super.newCopyArgs(
			pt1.x min: pt2.x,
			pt1.y min: pt2.y,
			absdif(pt1.x, pt2.x),
			absdif(pt1.y, pt2.y)
		)
	}
	*fromRect { arg rect;
		^this.new(rect.left, rect.top, rect.width, rect.height)
	}
	*fromArray {|array|
		^this.new(*array)
	}
	*aboutPoint { arg point, dx, dy;
		^this.new(point.x-dx, point.y-dy, 2*dx, 2*dy)
	}

	set { arg argLeft=0, argTop=0, argWidth=0, argHeight=0;
		left = argLeft;
		top = argTop;
		width = argWidth;
		height = argHeight;
	}
	setExtent { arg argWidth=0, argHeight=0;
		width = argWidth;
		height = argHeight;
	}

	origin { ^Point.new(left, top) }
	origin_ { | pt | left = pt.x; top = pt.y }
	extent { ^Point.new(width, height) }
	extent_ { | pt | width = pt.x; height = pt.y }
	center { ^Point.new(left + (width * 0.5), top + (height * 0.5)) }
	center_ { arg center; ^this.class.aboutPoint(center, width * 0.5, height * 0.5) }

	bottom { ^top + height }
	bottom_ { |b| top = top - (this.bottom - b) }
	right { ^left + width }
	right_ { |r| left = left - (this.right - r) }

	leftTop { ^Point.new(this.left, this.top) }
	rightTop { ^Point.new(this.right, this.top) }
	leftBottom { ^Point.new(this.left, this.bottom) }
	rightBottom { ^Point.new(this.right, this.bottom) }

	size { ^Size(width,height) }
	size_ { |sz| width = sz.width; height = sz.height }

	moveBy { arg h, v;
		^this.class.new(left + h, top + v, width, height)
	}
	moveTo { arg h, v;
		^this.class.new(h, v, width, height)
	}
	moveToPoint { arg aPoint;
		^this.class.new(aPoint.x, aPoint.y, width, height)
	}
	anchorTo { arg pt, position;
		^switch(position,
			// center_ returns new Rect, side setters modify this
			\center,      { this.center_(pt) },
			\top,         { this.center_(pt).top_(pt.y) },
			\bottom,      { this.center_(pt).bottom_(pt.y) },
			\left,        { this.center_(pt).left_(pt.x) },
			\right,       { this.center_(pt).right_(pt.x) },
			\topLeft,     { this.copy.top_(pt.y).left_(pt.x) },
			\topRight,    { this.copy.top_(pt.y).right_(pt.x) },
			\bottomLeft,  { this.copy.bottom_(pt.y).left_(pt.x) },
			\bottomRight, { this.copy.bottom_(pt.y).right_(pt.x) },
			// support synonymous keys that match preexisting Rect getters
			\leftTop,     { this.copy.top_(pt.y).left_(pt.x) },
			\rightTop,    { this.copy.top_(pt.y).right_(pt.x) },
			\leftBottom,  { this.copy.bottom_(pt.y).left_(pt.x) },
			\rightBottom, { this.copy.bottom_(pt.y).right_(pt.x) }
		)
	}
	resizeBy { arg h, v;
		^this.class.new(left, top, width + h, height + (v ? h))
	}
	resizeTo { arg h, v;
		^this.class.new(left, top, h, v)
	}
	insetBy { arg h, v;
		if(v.isNil){ v = h };
		^this.class.new(left + h, top + v, width - h - h, height - v - v)
	}
	insetAll { arg a, b, c, d;
		^this.class.new(left + a, top + b, width - a - c, height - b - d)
	}
	insetByRect { arg r;
		^this.copy.insetAll(r.left, r.top, r.right, r.bottom)
	}
	centerSquare {
		var pos, center;
		if (width > height, {
			pos = (width - height) * 0.5 + left;
			^Rect(pos, top, height, height)
		},{
			pos = (height - width) * 0.5 + top;
			^Rect(left, pos, width, width)
		});
	}
	centerIn { arg inRect;
		var pos, spacing;
		spacing  = (inRect.extent - this.extent) * 0.5;
		^inRect.origin - this.origin + spacing;
	}

	contains{ arg anObject;
		if ( anObject.isKindOf( Point ),
			{ ^this.containsPoint( anObject ) });
		if ( anObject.isKindOf( Rect ),
			{ ^this.containsRect( anObject ) });
		^false;
	}

	containsPoint { arg aPoint;
		^(aPoint.x.inclusivelyBetween(left, left + width)
			and: { aPoint.y.inclusivelyBetween(top, top + height) })
	}
	containsRect { arg aRect;
		^(this.containsPoint(aRect.leftTop) and: {this.containsPoint(aRect.rightBottom) })
	}
	intersects { arg aRect;
		if (aRect.right <= this.left, { ^false });
		if (aRect.bottom <= this.top, { ^false });
		if (aRect.left >= this.right, { ^false });
		if (aRect.top >= this.bottom, { ^false });
		^true
	}

	& { arg aRect; ^this sect: aRect }
	| { arg aRect; ^this union: aRect }

	union { arg aRect;
		^this.class.newSides( left min: aRect.left, top min: aRect.top,
			this.right max: aRect.right, this.bottom max: aRect.bottom)
	}
	sect { arg aRect;
		^this.class.newSides( left max: aRect.left, top max: aRect.top,
			this.right min: aRect.right, this.bottom min: aRect.bottom)
	}
	storeArgs { ^[left,top,width,height] }
	printOn { arg stream;
		stream << this.class.name << "(" <<* [left, top, width, height] << ")";
	}

	// the drawing routine here use Quickdraw.
	// If you want CoreGraphics drawing, use methods in Pen.
	draw { arg color, operation=2;
		_Rect_Draw
		^this.primitiveFailed
	}

	asRect { ^this }
	asSize { ^Size(width, height) }
	bounds { ^Rect.new(left, top, width, height) }
	== { arg that;
		^this.compareObject(that, #[\left, \top, \width, \height])
	}
	hash {
		^this.instVarHash(#[\left, \top, \width, \height])
	}

	// deprec
	layout { arg argBounds;
		this.set(argBounds.left, argBounds.top, argBounds.width, argBounds.height);
	}

	asArray { ^[this.left, this.top, this.width, this.height] }

	performBinaryOpOnSomething { |aSelector, thing, adverb|
		^thing.asRect.perform(aSelector, this, adverb)
	}
	+ {|that|
		var thatRect;
		thatRect = that.asRect;

		^Rect(
			this.left + thatRect.left,
			this.top + thatRect.top,
			this.width + thatRect.width,
			this.height + thatRect.height
		)
	}
	- {|that|
		var thatRect;
		thatRect = that.asRect;

		^Rect(
			this.left - thatRect.left,
			this.top - thatRect.top,
			this.width - thatRect.width,
			this.height - thatRect.height
		)
	}
	minSize { |minSize|
		var width, height;
		minSize = minSize.asSize;
		width = if (this.width < minSize.width) {
			minSize.width } {
			this.width
		};
		height = if (this.height < minSize.height) {
			minSize.height } {
			this.height
		};
		^Rect(this.left, this.top, width, height)
	}
}
