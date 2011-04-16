+ AbstractGroup {
	asGroup {}
}

+ Nil {
	asGroup {
		^Group.basicNew(this, 1)
	}
}
+ Server {
	asGroup {
		^Group.basicNew(this, 1)
	}
}
+ Synth {
	asGroup {
		^this.group
	}
}
+ Integer {
	asGroup {
		^Group.basicNew(nil, this)
	}
}
