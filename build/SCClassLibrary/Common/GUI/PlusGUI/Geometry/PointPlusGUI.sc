+ Lines {
	draw {
		_Lines_Draw
		^this.primitiveFailed
	}
	click { arg where;
		^false // need to implement this..
	}
}

+ Polygon {
	draw {
		_Polygon_Draw
		^this.primitiveFailed
	}
	click { arg where;
		^false // need to implement this..
	}
}

+ ZigZag {
	draw {
		_ZigZag_Draw
		^this.primitiveFailed
	}
	click { arg where;
		^false // need to implement this..
	}
}
