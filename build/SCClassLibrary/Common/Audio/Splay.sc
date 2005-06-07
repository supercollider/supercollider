Splay { 
	*ar { arg inArray, spread=1, level=1, center=0.0, levelComp=true;
		var n, n1; n = inArray.size.max(2); n1 = n-1;
		
		if (levelComp, { level = level * n.reciprocal.sqrt });

		^Pan2.ar(
			inArray, 
			((0 .. n1) * (2 / n1) - 1) * spread + center
		).sum * level;
	}
	
	*arFill { arg n, function, spread=1, level=1, center=0.0, levelComp=true;
		^this.ar((function ! n), spread, level, center, levelComp)
	}
}

SplayZ { 
	*ar { arg numChans=4, inArray, spread=1, level = 1, width = 2, center = 0.0,
			orientation = 0.5, levelComp=true;
		
		var n, n1; n = inArray.size.max(2); n1 = n-1;	
		if (levelComp, { level = level * n.reciprocal.sqrt });
		
		^PanAz.ar(
			numChans, 
			inArray, 
			((0 .. n1) * (2 / n1) - 1) * spread + center, 
			1,
			width, 
			orientation
		).sum * level;
	}

	*arFill { arg nChans=4, n, function, spread=1, level=1, width = 2, center=0.0, 
		orientation = 0.5, levelComp=true;
		^this.ar(nChans, function ! n, spread, level, width, center, 
		orientation, levelComp)
	}
}