Polar : Number {
	var <>rho, <>theta;

	*new { |rho, theta|
		^super.newCopyArgs(rho, theta)
	}

	magnitude { ^rho }

	angle { ^theta }
	phase { ^theta }

	real { ^rho * cos(theta) }
	imag { ^rho * sin(theta) }

	asPolar { ^this }
	asComplex { ^Complex.new(this.real, this.imag) }
	asPoint { ^Point.new(this.real, this.imag) }

	scale { |scale|
		^Polar.new(rho * scale, theta)
	}
	rotate { |angle| // in radians
		^Polar.new(rho, theta + angle)
	}

	// do math as Complex
	+ { |aNumber| ^this.asComplex + aNumber }
	- { |aNumber| ^this.asComplex - aNumber }
	* { |aNumber| ^this.asComplex * aNumber }
	/ { |aNumber| ^this.asComplex / aNumber }

	== { |aPolar|
		^aPolar respondsTo: #[\rho, \theta] and: {
			rho == aPolar.rho and: { theta == aPolar.theta }
		}
	}

	hash {
		^rho.hash bitXor: theta.hash
	}

	neg { ^Polar.new(rho, theta + pi) }

	performBinaryOpOnSomething { |aSelector, thing, adverb|
		^thing.asComplex.perform(aSelector, this, adverb)
	}

	performBinaryOpOnUGen { |aSelector, aUGen|
		^Complex.new(
			BinaryOpUGen.new(aSelector, aUGen, this.real),
			BinaryOpUGen.new(aSelector, aUGen, this.imag)
		)
	}

	printOn { |stream|
		stream << "Polar(" << rho << ", " << theta << ")"
	}
	storeArgs { ^[rho, theta] }
}
