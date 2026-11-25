Magnitude : Object {
	// Magnitudes are objects which represent a linear measure

	== { |aMagnitude| ^this.subclassResponsibility(thisMethod) }
	!= { |aMagnitude| ^(this == aMagnitude).not }
	hash { ^this.subclassResponsibility(thisMethod) }

	// all of the other compare operations are built upon <
	< { |aMagnitude| ^this.subclassResponsibility(thisMethod) }

	> { |aMagnitude| ^aMagnitude < this }
	<= { |aMagnitude| ^(aMagnitude < this).not }
	>= { |aMagnitude| ^(this < aMagnitude).not }
	exclusivelyBetween { |lo, hi| ^(lo < this) and: { this < hi } }
	inclusivelyBetween { |lo, hi| ^(lo <= this) and: { this <= hi } }
	min { |aMagnitude| if(this < aMagnitude) { ^this } { ^aMagnitude } }
	max { |aMagnitude| if(this < aMagnitude) { ^aMagnitude } { ^this } }
	clip { |lo, hi|
		// clip the receiver to the range lo, hi
		^if(this < lo) { ^lo } { if(hi < this) { ^hi } { ^this } }
	}

}
