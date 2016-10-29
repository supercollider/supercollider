// asLayoutElement interface to allow non-Views to go into Layouts
// (such as SCViewHolders)

+ Object {
	asLayoutElement { ^this.asView }
}

+ View {
	asLayoutElement { ^this }
}

+ Number {
	asLayoutElement { ^this }
}

+ Symbol {
	asLayoutElement { ^this }
}

+ SequenceableCollection {
	asLayoutElement { ^this.collect { |item| item.asLayoutElement } }
}
