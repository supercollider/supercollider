

+ Window {

	asFlowView { arg bounds;
		^FlowView(this,bounds)
	}
	flow { arg func,bounds;
		var f;
		f = FlowView(this,bounds ?? { this.bounds.moveTo(0,0) });
		func.value(f);
		if(bounds.isNil,{ f.resizeToFit });
		^f
	}
	comp { arg func,bounds;
		var f;
		f = View(this,bounds ?? { this.bounds.moveTo(0,0) });
		func.value(f);
		^f
	}
}


+ View {

	asFlowView { arg bounds;
		^FlowView(this,bounds ?? {this.bounds})
	}
	deepDo { arg function;
		// includes self
		function.value(this);
		this.children.do({arg child;
			child.deepDo(function);
		});
	}
	allChildren {
		// includes self
		var all;
		all = Array.new;
		this.deepDo({ arg child; all = all.add(child) });
		^all
	}
	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds); // flow view intellegently calc defaults bounds
		func.value(f);
		if(bounds.isNil,{ f.resizeToFit });
		^f
	}
	horz { arg func,bounds;
		var comp;
		comp = HLayoutView(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = VLayoutView(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = View(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,autohidesScrollers=true,autoScrolls=true,
					hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = ScrollView(this,bounds ?? { this.bounds });
		comp.autohidesScrollers = autohidesScrollers;
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}
}
