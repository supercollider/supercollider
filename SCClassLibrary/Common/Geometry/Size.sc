Size {
	var <>width = 0, <>height = 0;

	*new { arg width=0, height=0;
		^super.newCopyArgs(width, height);
	}

	asSize { ^this }

	asRect { ^Rect(0,0,width,height); }

	asPoint { ^Point(width,height); }

	asString {
		^("Size(" ++ width ++ ", " ++ height ++ ")");
	}

	== { arg other;
		^ other respondsTo: #[\width, \height] and: {
			(other.width == width) && (other.height == height)
		}
	}
}

+ SimpleNumber {
	asSize { ^Size(this, this) }
}
