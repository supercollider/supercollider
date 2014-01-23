Size {
	var <>width = 0, <>height = 0;

	*new { arg width=0, height=0;
		^super.newCopyArgs(width, height);
	}

	asSize { ^this }

	asRect { ^Rect(0, 0, width, height) }

	asPoint { ^Point(width, height) }

	== { arg size;
		^this.compareObject(size, #[\width, \height])
	}

	hash {
		^this.instVarHash(#[\width, \height])
	}

	printOn { |stream|
		this.storeOn(stream)
	}

	storeArgs { ^[width, height] }

}

+ SimpleNumber {
	asSize { ^Size(this, this) }
}
