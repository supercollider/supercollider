
+ Group {	
	asGroup {}
}

+ Nil {
	asGroup {
		^RootNode.new
	}
}
+ Server {
	asGroup {
		^RootNode(this)
	}
}
+ Synth {
	asGroup {
		^this.group
	}
}