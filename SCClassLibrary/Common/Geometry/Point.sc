Point {
	var <>x = 0, <>y = 0;

	*new { arg x=0, y=0;
		^super.newCopyArgs(x, y);
	}
	@ { arg aPoint;
		^Rect.fromPoints(this, aPoint)
	}

	set { arg argX=0, argY=0; x = argX; y = argY; }

	asPoint { ^this }
	asComplex { ^Complex.new(x,y) }
	asPolar { ^Polar.new(this.rho, this.theta) }
	asSize { ^Size(x,y) }
	asRect { ^Rect.new(0,0,x,y) }
	asArray { ^[this.x, this.y] }

	== { arg aPoint;
		^this.compareObject(aPoint, #[\x, \y])
	}
	hash {
		^this.instVarHash(#[\x, \y])
	}
	performBinaryOpOnSomething { |aSelector, thing, adverb|
		^thing.asPoint.perform(aSelector, this, adverb)
	}
	+ { arg delta;
		var deltaPoint;
		deltaPoint = delta.asPoint;
		^(this.x + deltaPoint.x) @ (this.y + deltaPoint.y)
	}
	- { arg delta;
		var deltaPoint;
		deltaPoint = delta.asPoint;
		^(this.x - deltaPoint.x) @ (this.y - deltaPoint.y)
	}

	* { arg scale;
		var scalePoint;
		scalePoint = scale.asPoint;
		^(this.x * scalePoint.x) @ (this.y * scalePoint.y)
	}
	/ { arg scale;
		var scalePoint;
		scalePoint = scale.asPoint;
		^(this.x / scalePoint.x) @ (this.y / scalePoint.y)
	}
	div { arg scale;
		var scalePoint;
		scalePoint = scale.asPoint;
		^(this.x div: scalePoint.x) @ (this.y div: scalePoint.y)
	}
	translate { arg delta;
		^(this.x + delta.x) @ (this.y + delta.y)
	}
	scale { arg scale;
		^(this.x * scale.x) @ (this.y * scale.y)
	}
	rotate { arg angle; // in radians
		var sinr, cosr;
		sinr = angle.sin;
		cosr = angle.cos;
		^((x * cosr) - (y * sinr)) @ ((y * cosr) + (x * sinr))
	}

	abs { ^x.abs @ y.abs }

	rho { ^hypot(x, y) }
	theta { ^atan2(y, x) }

	dist { arg aPoint;
		aPoint = aPoint.asPoint;
		^hypot(x - aPoint.x, y - aPoint.y)
	}
	transpose { ^y @ x }

	round { arg quant;
		quant = quant.asPoint;
		^x.round(quant.x) @ y.round(quant.y)
	}
	trunc { arg quant;
		quant = quant.asPoint;
		^x.trunc(quant.x) @ y.trunc(quant.y)
	}

	mod {|that|
		var thatPoint;
		thatPoint = that.asPoint;
		^(this.x mod: thatPoint.x) @ (this.y mod: thatPoint.y)
	}

	printOn { arg stream;
		stream << this.class.name << "( " << x << ", " << y << " )";
	}
	storeArgs { ^[x,y] }
}


PointArray : Point
{
	*new { arg n;
		^super.new(Signal.new(n), Signal.new(n))
	}
	add { arg point;
		x = x.add(point.x);
		y = y.add(point.y);
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
