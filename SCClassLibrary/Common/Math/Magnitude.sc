Magnitude : Object {
	// Magnitudes are objects which represent a linear measure

	== { arg aMagnitude; ^this.subclassResponsibility(thisMethod) }
	!= { arg aMagnitude; ^(this == aMagnitude).not }
	hash { ^this.subclassResponsibility(thisMethod) }

	// all of the other compare operations are built upon <
	< { arg aMagnitude; ^this.subclassResponsibility(thisMethod) }

	> { arg aMagnitude; ^aMagnitude < this }
	<= { arg aMagnitude; ^(aMagnitude < this).not }
	>= { arg aMagnitude; ^(this < aMagnitude).not }
	exclusivelyBetween { arg lo, hi; ^(lo < this) and: { this < hi } }
	inclusivelyBetween { arg lo, hi; ^(lo <= this) and: { this <= hi } }
	min { arg aMagnitude; if (this < aMagnitude, {^this}, {^aMagnitude}) }
	max { arg aMagnitude; if (this < aMagnitude, {^aMagnitude}, {^this}) }
	clip { arg lo, hi;
		// clip the receiver to the range lo, hi
		^if (this < lo, {^lo}, { if (hi < this, {^hi}, {^this}) })
	}

}
