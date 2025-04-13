Splay : UGen {

	*new1 { arg rate, spread = 1, level = 1, center = 0.0, levelComp = true ... inArray;

		var n = max(2, inArray.size);
		var n1 = n - 1;
		var positions = ((0 .. n1) * (2 / n1) - 1) * spread + center;

		if (levelComp == true) {
			// ar default is equal power for backwards compatibility
			if(rate == \audio) {
				level = level * n.reciprocal.sqrt
			} {
				// kr default is equal amplitude
				level = level / n
			}
		} {
			// if false, level is untouched
			if (levelComp != false) {
				// if number or control signal,
				// scale by exponent: 0 is none,
				// 0.5 is square root = equal power
				// 1.0 is level / numchans = equal amplitude
				level = level / (n ** levelComp.clip(0.0, 1.0));
			};
		};
		level.poll;

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
		var pos = if(n == 1) { center } { [ center - spread, center + spread ].resamp1(n) };
		if (levelComp) { level = level * n.reciprocal.sqrt };
		^PanAz.kr(numChans, inArray.asArray, pos, level, width, orientation).flop.collect(Mix(_))
	}

	*ar { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0, orientation = 0.5, levelComp = true;

		var n = max(1, inArray.size);
		var normalizedSpread = spread * (n - 1 / n);
		var pos = if(n == 1) { center } { [ center - normalizedSpread, center + normalizedSpread ].resamp1(n) };

		if (levelComp) { level = level * n.reciprocal.sqrt };
		^PanAz.ar(numChans, inArray.asArray, pos, level, width, orientation).flop.collect(Mix(_))
	}

	*arFill { arg numChans = 4, n, function, spread = 1, level = 1, width = 2, center = 0.0, orientation = 0.5, levelComp = true;
		^this.ar(numChans, function ! n, spread, level, width, center, orientation, levelComp)
	}

}
