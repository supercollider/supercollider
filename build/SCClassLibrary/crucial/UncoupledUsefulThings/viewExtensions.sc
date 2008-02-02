
+ SCWindow {
	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds ?? { this.bounds });
		func.value(f);
		f.resizeToFit;
		^f
	}
}

+ SCContainerView {

	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds /*?? { this.bounds }*/);
		func.value(f);
		f.resizeToFit;
		^f
	}
	horz { arg func,bounds;
		var comp;
		comp = GUI.hLayoutView.new(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}		
}

+ SCScrollViewÊ{
	flow { |func, bounds|
		var f,comp;
		f = FlowView(this,(bounds ?? { this.bounds.moveTo(0, 0) }));
		func.value(f);
		f.resizeToFit;
		^f
	}
}

+ FlowView {

	flow { arg func,bounds;
		var f;
		f = FlowView(this,bounds ?? { this.decorator.indentedRemaining });
		func.value(f);
		f.resizeToFit;
		this.reflowAll; // annoying and slow
			// should only have to correct the last setting,
			// set the FlowLayout left top maxHeight maxRight
		^f
	}

	horz { arg func,bounds;
		var comp;
		comp = GUI.hLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.bounds });
		func.value(comp);
		^comp
	}
	indentedRemaining { ^this.decorator.indentedRemaining }
}

+ MultiPageLayout {

	flow { arg func,bounds;
		^this.view.flow(func,bounds)
	}
	vert { arg func,bounds;
		^this.view.vert(func,bounds)
	}
	horz { arg func,bounds;
		^this.view.horz(func,bounds)
	}
	comp { arg func,bounds;
		^this.view.comp(func,bounds)
	}
}

+ FlowLayout {

	used { //round out to nearest rect
		^Rect(bounds.left,bounds.top,
			maxRight + margin.x /*+ margin.x*/ - bounds.left,
			(top + maxHeight + margin.y /*+ margin.y*/ ) - bounds.top)
	}
	indentedRemaining {
		var inb;
		inb = this.innerBounds;
		^Rect(left,top,
			inb.width - (left - inb.left - margin.x),
			inb.height - (top - inb.top - margin.y))
	}
	
	wouldExceedBottom { arg aBounds;
		^(top + aBounds.height + margin.y) > bounds.bottom
	}
}


// support for absolute / relative coordinates in composite views

+ Rect {
		// "this" is a Rect with coordinates relative to "view"'s parent
	setOriginRelativeTo { |view|
		var	b = view.bounds;
		^this.moveBy(b.left.neg, b.top.neg)
	}
	
		// "this" is a Rect with coordinates relative to the view
	setOriginAbsolute { |view|
		var	b;
		if((view.tryPerform(\relativeOrigin) ? false)) {
			b = view.tryPerform(\absoluteBounds) ?? { view.bounds };
			^this.moveBy(b.left, b.top)
		} {
			^this
		}
	}
}
