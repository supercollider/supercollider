
+ Object {

	// create a gui using the guiClass passing in args
	// the first arg is the parent: a window or view
	// and the second arg is the bounds: a Rect, Point (width x height), or nil
	gui { arg  ... args;
		^this.guiClass.new(this).performList(\gui,args);
	}

	guiClass { ^ObjectGui }

}


// these all get simple guis : just a label with the name
+ String {
	guiClass { ^StringGui }
}
+ Symbol {
	guiClass { ^StringGui }
}
// though it would be nice to have it indicated what class it is by color etc
+ SimpleNumber {
	guiClass { ^StringGui }
}
+ Boolean {
	guiClass { ^StringGui }
}
+ Nil {
	guiClass { ^StringGui }
	
	asPageLayout { arg name,bounds;
		^PageLayout(name.asString,bounds ).front
	}

	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
}

+ Point {
	asPageLayout {
		^PageLayout("",this.asRect ).front
	}
}
+ Rect {
	asPageLayout {
		^PageLayout("",this ).front
	}
}

+ PageLayout {

	flow { arg func,bounds;
		^this.view.flow(func,bounds)
	}
	vert { arg func,bounds,spacing;
		^this.view.vert(func,bounds,spacing)
	}
	horz { arg func,bounds,spacing;
		^this.view.horz(func,bounds,spacing)
	}
	comp { arg func,bounds;
		^this.view.comp(func,bounds)
	}
	scroll { arg ... args;
		^this.view.performList(\scroll,args)
	}
}


+ FlowView {
	asFlowView {}
	asPageLayout {} // should be compatible
}

