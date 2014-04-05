
+ Object {

	gui { arg parent,bounds ... args;
		^this.guiClass.new(this).performList(\gui,[parent,bounds] ++ args);
	}
	guiClass { ^ObjectGui }
}


// just a label with the name
+ String {
	guiClass { ^StringGui }
}
+ Symbol {
	guiClass { ^StringGui }
}
+ SimpleNumber {
	guiClass { ^StringGui }
}
+ Boolean {
	guiClass { ^StringGui }
}


+ Nil {

	guiClass { ^StringGui }

	// create a window/FlowView if you don't supply a parent to: thing.gui
	asPageLayout { arg name,bounds;
		^PageLayout(name.asString,bounds ).front
	}
	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
}

