LevelComp {
	*new { |levelComp, rate, n|
		if (levelComp == true) {
			^if(rate == \audio) {
				// ar default is equal power for backwards compatibility
				n.reciprocal.sqrt
			} {
				// kr default is equal amplitude
				n.reciprocal
			}
		};
		// if false, level is untouched
		if (levelComp == false) { ^1 };

		// now levelComp is a number or UGen,
		// so we scale by exponent: 0 is no change,
		// 0.5 is square root = equal power
		// 1.0 is level / numchans = equal amplitude
		^n.reciprocal ** levelComp.clip(0.0, 1.0)
	}
}

Splay : UGen {

	*new1 { arg rate, spread = 1, level = 1, center = 0.0, levelComp = true ... inArray;

		var n = max(2, inArray.size);
		var n1 = n - 1;
		var positions = ((0 .. n1) * (2 / n1) - 1) * spread + center;
		level = level * LevelComp(levelComp, rate, n);

		^Mix(Pan2.perform(this.methodSelectorForRate(rate), inArray, positions)) * level;
	}

	*kr { arg inArray, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.multiNewList([\control, spread, level, center, levelComp] ++ inArray)

	}

	*ar { arg inArray, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.multiNewList([\audio, spread, level, center, levelComp] ++ inArray)
	}

	*arFill { arg n, function, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.ar((function ! n), spread, level, center, levelComp)
	}

}



SplayAz : UGen {

	*kr { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0, orientation = 0.5, levelComp = true;

		var n = max(1, inArray.size);
		var normSpread = (n - 1 / n) * spread;
		var pos = if(n == 1) { center } { [ center - normSpread, center + normSpread ].resamp1(n) };
		level = level * LevelComp(levelComp, \control, n);

		^PanAz.kr(numChans, inArray.asArray, pos, level, width, orientation).flop.collect(Mix(_))
	}

	*ar { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0, orientation = 0.5, levelComp = true;

		var n = max(1, inArray.size);
		var normSpread = (n - 1 / n) * spread;
		var pos = if(n == 1) { center } { [ center - normSpread, center + normSpread ].resamp1(n) };

		level = level * LevelComp(levelComp, \audio, n);

		^PanAz.ar(numChans, inArray.asArray, pos, level, width, orientation).flop.collect(Mix(_))
	}

	*arFill { arg numChans = 4, n, function, spread = 1, level = 1, width = 2, center = 0.0, orientation = 0.5, levelComp = true;
		^this.ar(numChans, function ! n, spread, level, width, center, orientation, levelComp)
	}

}
