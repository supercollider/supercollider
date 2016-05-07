Gradient {
	var color1, color2, direction, steps;
	*new { arg color1, color2, direction=\h, steps=64;
		^super.newCopyArgs(color1, color2, direction, steps)
	}
	at { |pos|
		^blend(color1, color2, pos.round(steps.reciprocal))
	}
}

HiliteGradient {
	var color1, color2, direction, steps, frac;
	*new { arg color1, color2, direction=\v, steps=64, frac = 0.33;
		^super.newCopyArgs(color1, color2, direction, steps, frac)
	}
	at { |pos|
		^blend(color1, color2, pos.round(steps.reciprocal))
	}
}
