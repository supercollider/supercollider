
+ SCWindow {
	flow { arg func,bounds;
		var f;
		f = FlowView(this,bounds ?? { this.bounds });
		func.value(f);
		f.resizeToFit;
		^f
	}
	comp { arg func,bounds;
		var f;
		f = GUI.compositeView.new(this,bounds ?? { this.bounds });
		func.value(f);
		^f
	}
}

+ SCView {
	children { ^nil }
	deepDo { arg function;// includes self
		function.value(this);
		this.children.do({arg child;
			child.deepDo(function);
		});
	}
	allChildren {
		var all;
		all = Array.new;
		this.deepDo({ arg child; all = all.add(child) });
		^all
		// includes self
	}
}
+ SCViewHolder {
	children { ^[view] }
	deepDo { arg function;// includes self
		function.value(this);
		this.children.do({arg child;
			child.deepDo(function);
		});
	}
	allChildren {
		var all;
		all = Array.new;
		this.deepDo({ arg child; all = all.add(child) });
		^all
		// includes self
	}
}

+ SCContainerView {

	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds); // flow view intellegently calc defaults bounds
		func.value(f);
		if(bounds.isNil,{ f.resizeToFit });
		^f
	}
	horz { arg func,bounds;
		var comp;
		comp = GUI.hLayoutView.new(this,bounds ?? { this.bounds });
		comp.spacing = GUI.skin.gap.x;
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.bounds });
		comp.spacing = GUI.skin.gap.y;
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,autohidesScrollers=true,autoScrolls=true,
					hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = GUI.scrollView.new(this,bounds ?? { this.bounds });
		comp.autohidesScrollers = autohidesScrollers;
		comp.autoScrolls = autoScrolls;
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}
}

+ SCCompositeView {
	// shouldnt really do this to an h or v layout
	resizeToFit {
		var r;
		r = Rect(0,0,0,0);
		this.children.do({ |kid|
			r = r.union(kid.bounds)
		});
		this.bounds = r;
	}
}

+ SCLayoutView {
	// because a scroll view inside an h or v layout will crash the lang
	// layer in a composite view to protect it until we fix that bug.
	scroll { arg func,bounds,autohidesScrollers=true,autoScrolls=true,
					hasHorizontalScroller=true,hasVerticalScroller=true;
		var scroll;
		this.comp({ |comp|
				^comp.scroll(func,comp.bounds.setOriginAbsolute(comp),
					autohidesScrollers,autoScrolls,hasHorizontalScroller,hasVerticalScroller);
		})
	}
}

/* but its already an SCContainerView
	the difference in the bounds.moveTo is a problem somewhere else in the system,
	nothing to do with SCScrollView per se.
	the FlowView already detects and fixes any bounds problems.
*/
/*
+ SCScrollView {
	flow { |func, bounds|
		var f,comp;
		f = FlowView(this,(bounds ?? { this.bounds.moveTo(0, 0) }));
		func.value(f);
		f.resizeToFit;
		^f
	}
	scroll { ^this }
}
*/
