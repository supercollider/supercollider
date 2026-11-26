Point {
	var <>x = 0, <>y = 0;

	*new { |x = 0, y = 0|
		^super.newCopyArgs(x, y)
	}
	@ { |aPoint|
		^Rect.fromPoints(this, aPoint)
	}

	set { |argX = 0, argY = 0| x = argX; y = argY }

	asPoint { ^this }
	asComplex { ^Complex.new(x, y) }
	asPolar { ^Polar.new(this.rho, this.theta) }
	asSize { ^Size(x, y) }
	asRect { ^Rect.new(0, 0, x, y) }
	asArray { ^[this.x, this.y] }

	== { |aPoint|
		^this.compareObject(aPoint, #[\x, \y])
	}
	hash {
		^this.instVarHash(#[\x, \y])
	}
	performBinaryOpOnSomething { |aSelector, thing, adverb|
		^thing.asPoint.perform(aSelector, this, adverb)
	}
	+ { |delta|
		var deltaPoint;
		deltaPoint = delta.asPoint;
		^Point(this.x + deltaPoint.x, this.y + deltaPoint.y)
	}
	- { |delta|
		var deltaPoint;
		deltaPoint = delta.asPoint;
		^Point(this.x - deltaPoint.x, this.y - deltaPoint.y)
	}

	* { |scale|
		var scalePoint;
		scalePoint = scale.asPoint;
		^Point(this.x * scalePoint.x, this.y * scalePoint.y)
	}
	/ { |scale|
		var scalePoint;
		scalePoint = scale.asPoint;
		^Point(this.x / scalePoint.x, this.y / scalePoint.y)
	}
	div { |scale|
		var scalePoint;
		scalePoint = scale.asPoint;
		^Point(this.x div: scalePoint.x, this.y div: scalePoint.y)
	}
	translate { |delta|
		^Point(this.x + delta.x, this.y + delta.y)
	}
	scale { |scale|
		^Point(this.x * scale.x, this.y * scale.y)
	}
	rotate { |angle| // in radians
		var sinr, cosr;
		sinr = angle.sin;
		cosr = angle.cos;
		^Point((x * cosr) - (y * sinr), (y * cosr) + (x * sinr))
	}

	abs { ^Point(x.abs, y.abs) }

	rho { ^hypot(x, y) }
	theta { ^atan2(y, x) }

	dist { |aPoint|
		aPoint = aPoint.asPoint;
		^hypot(x - aPoint.x, y - aPoint.y)
	}
	transpose { ^Point(y, x) }

	round { |quant|
		quant = quant.asPoint;
		^Point(x.round(quant.x), y.round(quant.y))
	}
	trunc { |quant|
		quant = quant.asPoint;
		^Point(x.trunc(quant.x), y.trunc(quant.y))
	}

	mod { |that|
		var thatPoint;
		thatPoint = that.asPoint;
		^Point(this.x mod: thatPoint.x, this.y mod: thatPoint.y)
	}

	modSeaside { |that|
		var thatPoint;
		thatPoint = that.asPoint;
		^Point(this.x modSeaside: thatPoint.x, this.y modSeaside: thatPoint.y)
	}

	printOn { |stream|
		stream << this.class.name << "(" << x << ", " << y << ")"
	}
	storeArgs { ^[x, y] }
}


PointArray : Point {
	*new { |n|
		^super.new(Signal.new(n), Signal.new(n))
	}
	add { |point|
		x = x.add(point.x);
		y = y.add(point.y)
	}
}

//Lines : PointArray
//{
//	*new { arg n;
//		^super.new(2*n)
//	}
//}
//
//Polygon : PointArray
//{
//}
//
//ZigZag : PointArray
//{
//}
