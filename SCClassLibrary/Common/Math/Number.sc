Number : Magnitude {
	isNumber { ^true }

	+ { |aNumber| ^this.subclassResponsibility(thisMethod) }
	- { |aNumber| ^this.subclassResponsibility(thisMethod) }
	* { |aNumber| ^this.subclassResponsibility(thisMethod) }
	/ { |aNumber| ^this.subclassResponsibility(thisMethod) }
	mod { |aNumber| ^this.subclassResponsibility(thisMethod) }
	modSeaside { |aNumber| ^this.subclassResponsibility(thisMethod) }
	div { |aNumber| ^this.subclassResponsibility(thisMethod) }
	pow { |aNumber| ^this.subclassResponsibility(thisMethod) }

	performBinaryOpOnSeqColl { |aSelector, aSeqColl, adverb|
		^aSeqColl.collect { |item|
			item.perform(aSelector, this, adverb)
		}
	}
	performBinaryOpOnPoint { |op, aPoint, adverb|
		^Point.new(this.perform(op, aPoint.x, adverb), this.perform(op, aPoint.y, adverb))
	}

	// polar support
	rho { ^this }
	theta { ^0.0 }

	// complex support
	real { ^this }
	imag { ^0.0 }

	// conversion
	@ { |aNumber| ^Point.new(this, aNumber) }
	complex { |imaginaryPart| ^Complex.new(this, imaginaryPart) }
	polar { |angle| ^Polar.new(this, angle) }

	// iteration
	for { |endValue, function|
		var i, j = 0;
		i = this;
		while { i <= endValue } { function.value(i, j); i = i + 1; j = j + 1 }
	}
	forBy { |endValue, stepValue, function|
		var i, j = 0;
		i = this;
		if(stepValue > 0) {
			while { i <= endValue } { function.value(i, j); i = i + stepValue; j = j + 1 }
		} {
			while { i >= endValue } { function.value(i, j); i = i + stepValue; j = j + 1 }
		}
	}

	forSeries { |second, last, function|
		// called by generator expression
		// compiler replaces this with special byte codes.
		var step, j = 0;
		if(second.isNil) {
			last = last ? inf;
			step = if(this < last, 1, -1)
		} {
			last ?? { last = if(second < this, -inf, inf) };
			step = second - this
		};
		^forBy(this, last, step, function)
	}

}
