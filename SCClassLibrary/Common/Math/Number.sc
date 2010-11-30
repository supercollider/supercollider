Number : Magnitude {
	isNumber { ^true }

	+ { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	- { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	* { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	/ { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	mod { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	div { arg aNumber; ^this.subclassResponsibility(thisMethod) }
	pow { arg aNumber; ^this.subclassResponsibility(thisMethod) }

	performBinaryOpOnSeqColl { arg aSelector, aSeqColl, adverb;
		^aSeqColl.collect({ arg item;
			item.perform(aSelector, this, adverb)
		})
	}
	performBinaryOpOnPoint { arg op, aPoint, adverb;
		^Point.new(this.perform(op, aPoint.x, adverb), this.perform(op, aPoint.y, adverb));
	}

	// polar support
	rho { ^this }
	theta { ^0.0 }

	// complex support
	real { ^this }
	imag { ^0.0 }

	// conversion
	@ { arg aNumber; ^Point.new(this, aNumber) }
	complex { arg imaginaryPart; ^Complex.new(this, imaginaryPart) }
	polar { arg angle; ^Polar.new(this, angle) }

	// iteration
	for { arg endValue, function;
		var i, j = 0;
		i = this;
		while ({ i <= endValue }, { function.value(i, j); i = i + 1; j = j + 1 });
	}
	forBy { arg endValue, stepValue, function;
		var i, j=0;
		i = this;
		(stepValue > 0).if({
			while ({ i <= endValue }, { function.value(i,j); i = i + stepValue; j=j+1; });
		}, {
			while ({ i >= endValue }, { function.value(i,j); i = i + stepValue; j=j+1; });
		});
	}

	forSeries { arg second, last, function;
		// called by generator expression
		// compiler replaces this with special byte codes.
		var step, j=0;
		if (second.isNil) {
			last = last ? inf;
			step = if (this < last, 1, -1);
		}{
			last ?? { last = if (second < this, -inf, inf) };
			step = second - this;
		};
		^this.forBy(last, step, function)
	}

}
