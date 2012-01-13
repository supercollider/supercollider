
+ FlowView {

	// place a FlowView on this FlowView
	flow { arg func,bounds;
		var f,consumed,b;
		if(bounds.notNil,{
			f = FlowView(this,bounds);
			func.value(f);
			^f
		});
		f = FlowView(this,this.allocateRemaining);
		func.value(f);
		consumed = f.resizeToFit;
		// did we exceed ?
		if(this.decorator.bounds.containsRect(consumed).not,{
			// yes
			// pretend I just consumed nothing
			this.didUseAllocated(consumed.resizeTo(0,0));

			// goto the next line
			this.decorator.nextLine; // don't put a StartRow in there, the decorator should auto-flow on resize
			// take everything
			b = this.allocateRemaining;
			// and if its too big for that then it will just have to jutt or scroll over
			// that's what you asked for.
			// move the last object there
			f.bounds = b;
			// reflow the sub view
			f.reflowAll.resizeToFit;
			this.didUseAllocated(f.bounds);
		},{
			this.didUseAllocated(consumed);
		});
		^f
	}

	horz { arg func,bounds,spacing;
		var comp;
		comp = GUI.hLayoutView.new(this,bounds ?? { this.indentedRemaining });
		try { // not on Qt
			comp.spacing = spacing ? GUI.skin.gap.x;
		};
		func.value(comp);
		^comp
	}
	vert { arg func,bounds,spacing;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.indentedRemaining });
		try { // not on Qt
			comp.spacing = spacing ? GUI.skin.gap.y;
		};
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.indentedRemaining });
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,
				autohidesScrollers=true,autoScrolls=true,
				hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = GUI.scrollView.new(this,bounds ?? { this.bounds });
		comp.autohidesScrollers = autohidesScrollers;
		try { // not on Qt
			comp.autoScrolls = autoScrolls;
		};
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}
}


