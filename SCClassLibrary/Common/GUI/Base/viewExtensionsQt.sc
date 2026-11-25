

+ Window {

	asFlowView { |bounds|
		^FlowView(this, bounds)
	}
	flow { |func, bounds|
		var f;
		f = FlowView(this, bounds ?? { this.bounds.moveTo(0, 0) });
		func.value(f);
		if(bounds.isNil) { f.resizeToFit };
		^f
	}
	comp { |func, bounds|
		var f;
		f = View(this, bounds ?? { this.bounds.moveTo(0, 0) });
		func.value(f);
		^f
	}
}


+ View {

	asFlowView { |bounds|
		^FlowView(this, bounds ?? { this.bounds })
	}
	deepDo { |function|
		// includes self
		function.value(this);
		this.children.do { |child|
			child.deepDo(function)
		}
	}
	allChildren {
		// includes self
		var all;
		all = Array.new;
		this.deepDo({ |child| all = all.add(child) });
		^all
	}
	flow { |func, bounds|
		var f, comp;
		f = FlowView(this, bounds); // flow view intellegently calc defaults bounds
		func.value(f);
		if(bounds.isNil) { f.resizeToFit };
		^f
	}
	horz { |func, bounds|
		var comp;
		comp = HLayoutView(this, bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	vert { |func, bounds|
		var comp;
		comp = VLayoutView(this, bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	comp { |func, bounds|
		var comp;
		comp = View(this, bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	scroll { |func, bounds, autohidesScrollers = true, autoScrolls = true,
			hasHorizontalScroller = true, hasVerticalScroller = true|
		var comp;
		comp = ScrollView(this, bounds ?? { this.bounds });
		comp.autohidesScrollers = autohidesScrollers;
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}
}
