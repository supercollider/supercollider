
// the drawing routines in these classes use Quickdraw.
// If you want CoreGraphics drawing, use methods in Pen.

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
	extent { ^Point.new(width, height) }
	center { ^Point.new(left + (width * 0.5), top + (height * 0.5)) }
	
	bottom { ^top + height }
	right { ^left + width }
	
	leftTop { ^Point.new(this.left, this.top) }
	rightTop { ^Point.new(this.right, this.top) }
	leftBottom { ^Point.new(this.left, this.bottom) }
	rightBottom { ^Point.new(this.right, this.bottom) }
	
	moveBy { arg h, v;
		^this.class.new(left + h, top + v, width, height)
	}
	moveTo { arg h, v;
		^this.class.new(h, v, width, height)
	}
	moveToPoint { arg aPoint;
		^this.class.new(aPoint.x, aPoint.y, width, height)
	}
	resizeBy { arg h, v;
		^this.class.new(left, top, width + h, height + v)
	}
	resizeTo { arg h, v;
		^this.class.new(left, top, h, v)
	}
	insetBy { arg h, v;
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

	containsPoint { arg aPoint;
		^ aPoint.x.inclusivelyBetween(left, left + width) 
			and: { aPoint.y.inclusivelyBetween(top, top + height) }
	}
	containsRect { arg aRect;
		^(this.containsPoint(aRect.leftTop) and: {this.containsPoint(aRect.rightBottom) })
	}
	intersects { arg aRect;
		if (aRect.right < this.left, { ^false });
		if (aRect.bottom < this.top, { ^false });
		if (aRect.left > this.right, { ^false });
		if (aRect.top > this.bottom, { ^false });
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

	printOn { arg stream;
		stream << this.class.name << "(" 
			<<* [left, top, width, height] << ")";
	}
	storeOn { arg stream;
		stream << this.class.name << "(" 
			<<<* [left, top, width, height] << ")";
	}
	
	draw { arg color, operation=2;
		_Rect_Draw
		^this.primitiveFailed
	}
	
	asRect { ^this }
	bounds { ^Rect.new(left, top, width, height) }
	== { arg that; 
		^(left == that.left 
			and: {(top == that.top)} 
			and: {(width == that.width)} 
			and: {(height == that.height)})
	}
	layout { arg argBounds; 
		this.set(argBounds.left, argBounds.top, argBounds.width, argBounds.height);
	}

}

Oval : Rect {
	draw { arg color, operation=2;
		_Oval_Draw
		^this.primitiveFailed
	}
}

RoundRect : Rect {
	var <>radius=2;
	*newSides { arg left=0, top=0, right=0, bottom=0, radius=2;
		^super.newSides(left, top, right, bottom).radius_(radius)
	}
	*new { arg left=0, top=0, width=0, height=0, radius=2;
		^super.new(left, top, width, height).radius_(radius)
	}
	*fromRect { arg rect, radius=2;
		^super.fromRect(rect).radius_(radius)
	}
	draw { arg color, operation=2;
		_RoundRect_Draw
		^this.primitiveFailed
	}
	// ==
}

Arc : Rect {
	var <>startAngle=0, <>arcAngle=90;
	*new { arg left=0, top=0, width=0, height=0, startAngle=0, arcAngle=90;
		^super.new(left, top, width, height).startAngle_(startAngle).arcAngle_(arcAngle)
	}
	*newSides { arg left=0, top=0, right=0, bottom=0, startAngle=0, arcAngle=90;
		^super.newSides(left, top, right, bottom).startAngle_(startAngle).arcAngle_(arcAngle)
	}
	*fromRect { arg rect, startAngle=0, arcAngle=90;
		^super.fromRect(rect).startAngle_(startAngle).arcAngle_(arcAngle)
	}
	draw { arg color, operation=2;
		_Arc_Draw
		^this.primitiveFailed
	}
	// ==
}


BevelRect : Rect {
	var <>bevelWidth=2, <>inout=false;
	*new { arg left=0, top=0, width=0, height=0, bevelWidth=2, inout=false;
		^super.new(left, top, width, height).bevelWidth_(bevelWidth).inout_(inout)
	}
	*newSides { arg left=0, top=0, right=0, bottom=0, bevelWidth=2, inout=false;
		^super.newSides(left, top, right, bottom).bevelWidth_(bevelWidth).inout_(inout)
	}
	*fromRect { arg rect, bevelWidth=2, inout=false;
		^super.fromRect(rect).bevelWidth_(bevelWidth).inout_(inout)
	}
	draw { arg color, operation=2;
		_BevelRect_Draw
		^this.primitiveFailed
	}
	// ==
}

