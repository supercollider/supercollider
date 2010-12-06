// InterplEnvs are a fixed duration

InterplEnv {
	// envelope specification for an IEnvGen, InterplEnv is not a UGen itself
	var <levels;
	var <times;
	var <curves = 'lin';		// can also be 'exp', 'sin', 'cos', a float curve value,
							// or an array of curve values
	var <offset;
	var <array;

	classvar shapeNames;

	*new { arg levels=#[0,1,0], times=#[1,1], curve='lin', offset = 0.0;
		^super.newCopyArgs(levels, times, curve, offset)
	}
	*newClear { arg numSegments=8;
		// make an envelope for filling in later.
		^this.new(Array.fill(numSegments+1,0), Array.fill(numSegments,1))
	}

	*initClass {
		shapeNames = IdentityDictionary[
			\step -> 0,
			\lin -> 1,
			\linear -> 1,
			\exp -> 2,
			\exponential -> 2,
			\sin -> 3,
			\sine -> 3,
			\wel -> 4,
			\welch -> 4,
			\sqr -> 6,
			\squared -> 6,
			\cub -> 7,
			\cubed -> 7
		];
	}

	levels_ { arg z;
		levels = z;
		array = nil;
	}
	times_ { arg z;
		times = z;
		array = nil;
	}
	curves_ { arg z;
		curves = z;
		array = nil;
	}

	asArray {
		if (array.isNil) { array = this.prAsArray }
		^array
	}

	at { arg time;
		var env;
		env = this.asEnv;
		^env.asArray.envAt((time - offset).max(0))
	}

	asEnv {
		^Env(this.levels, this.times, this.curves);
		}

	shapeNumber { arg shapeName;
		var shape;
		if (shapeName.isValidUGenInput) { ^5 };
		shape = shapeNames.at(shapeName);
		if (shape.notNil) { ^shape };
		Error("Env shape not defined.").throw;
	}
	curveValue { arg curve;
		if (curve.isValidUGenInput, { ^curve },{ ^0 });
	}

	storeArgs { ^[levels, times, curves] }

	plot {arg size = 400;
		this.asEnv.plot(size);
		}

	prAsArray {
		var contents, curvesArray;
		contents = [offset, levels.at(0), times.size, times.sum];
		curvesArray = curves.asArray;
		times.size.do({ arg i;
			contents = contents ++ [
				times[i],
				this.shapeNumber(curvesArray.wrapAt(i)),
				this.curveValue(curvesArray.wrapAt(i)),
				levels[i+1]
			];
		});
		^contents
	}

}

// InterplXYC([0, 0, \lin], [1, 2, \sin], [2, 0])
// at time 0, value 0, lin to time 1, value 2, sin to time 2, value 0

InterplXYC : InterplEnv {
	*new {arg ... xyc;
		var x, y, c, times, levels, curves, offset;
		#x, y, c = xyc.flat.clump(3).flop;
		offset = x[0];
		levels = y;
		times = Array.fill(x.size - 1, {arg i; x[i + 1] - x[i]});
		c.removeAt(c.size - 1);
		^InterplEnv.new(levels, times, c, offset);
		}
	}


InterplPairs : InterplEnv {
	*new {arg pairs, curve;
		var x, y, times, levels, offset;
		curve = curve ? \lin;
		#x, y = pairs.flat.clump(2).flop;
		offset = x[0];
		levels = y;
		times = Array.fill(x.size - 1, {arg i; x[i + 1] - x[i]});
		^InterplEnv.new(levels, times, curve, offset);
		}
	}

// pairs is an array of [time, [val, val, val], time, [val, val, val]]
// format. All chords MUST have the same number of members

InterplChord {
	var <envs, <times;
	*new {arg pairs;
		^super.new.init(pairs);
	}

	init {arg pairs;
		var points;
		times = Array.newClear(pairs.size * 0.5 - 1);
		points = Array.fill(pairs.size * 0.5, {arg i; pairs[i * 2]});
		points.doAdjacentPairs({arg val1, val2, i; times[i] = val2 - val1});
		envs = Array.fill(pairs[1].size, {arg i;
			var levels;
			levels = Array.fill(pairs.size * 0.5, {arg j;
				pairs[j*2+1][i]});
			Env(levels, times)
			})
		}

	at {arg time;
		^Array.fill(envs.size, {arg i; envs[i][time]});
		}

	choose {arg time;
		^envs.choose[time]
		}
	}

