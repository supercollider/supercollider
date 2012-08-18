Splay : UGen {

	*new1 { arg rate, spread = 1, level = 1, center = 0.0, levelComp = true ... inArray;

		var n = max(2, inArray.size);
		var n1 = n - 1;
		var positions = ((0 .. n1) * (2 / n1) - 1) * spread + center;

		if (levelComp) {
			if(rate == \audio) {
				level = level * n.reciprocal.sqrt
			} {
				level = level / n
			}
		 };

		^Pan2.perform(this.methodSelectorForRate(rate), inArray, positions).sum * level;
	}

	*kr { arg inArray, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.multiNewList([\control, spread, level, center, levelComp] ++ inArray)

	}

	*ar { arg inArray, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.multiNewList([\audio, spread, level, center, levelComp] ++ inArray)
	}

	/*

	*ar { arg inArray, spread = 1, level = 1, center = 0.0, levelComp = true;

		var n = inArray.size.max(2);
		var n1 = n-1;

		if (levelComp) { level = level * n.reciprocal.sqrt };

		^Pan2.ar(
			inArray,
			((0 .. n1) * (2 / n1) - 1) * spread + center
		).sum * level;
	}

	*/

	*arFill { arg n, function, spread = 1, level = 1, center = 0.0, levelComp = true;
		^this.ar((function ! n), spread, level, center, levelComp)
	}
}



SplayAz : UGen {
	*kr { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0,
			orientation = 0.5, levelComp = true;

		var n = max(1, inArray.size);

		var pos = [ center - spread, center + spread ].resamp1(n);

		if (levelComp) {
			level = level / n
		};

		^(PanAz.kr(numChans, inArray, pos, level, width, orientation).flop.collect(_.sum))
	}

	*ar { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0,
			orientation = 0.5, levelComp = true;

		var n = max(1, inArray.size);

		var pos = [ center - spread, center + spread ].resamp1(n);

		if (levelComp) {
			level = level * n.reciprocal.sqrt
		};

		^(PanAz.ar(numChans, inArray, pos, level, width, orientation).flop.collect(_.sum))
	}

	*arFill { arg numChans = 4, n, function, spread = 1, level = 1, width = 2, center = 0.0,
		orientation = 0.5, levelComp = true;
		^this.ar(numChans, function ! n, spread, level, width, center,
			orientation, levelComp)
	}
}



SplayZ {
	*ar { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0,
			orientation = 0.5, levelComp = true;

		var n = inArray.size.max(2);
		var n1 = n - 1;

		if (levelComp) { level = level * n.reciprocal.sqrt };

		"SplayZ is deprecated, because its geometry is wrong.
		Please convert to SplayAz.".inform;

		^PanAz.ar(
			numChans,
			inArray,
			((0 .. n1) * (2 / n1) - 1) * spread + center,
			1,
			width,
			orientation
		).sum * level;
	}

	*arFill { arg numChans = 4, n, function, spread = 1, level = 1, width = 2, center = 0.0,
		orientation = 0.5, levelComp = true;
		^this.ar(numChans, function ! n, spread, level, width, center,
		orientation, levelComp)
	}
}
