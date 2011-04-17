+ StartRow {
	children { ^nil }
}


+ FlowView {

	// you might wish to put a smaller flow inside the flow view you are already in.
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
		comp = GUI.hLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
//		comp.tryPerform('relativeOrigin_',true);
		comp.spacing = spacing ? GUI.skin.gap.x;
		func.value(comp);
		^comp
	}
	vert { arg func,bounds,spacing;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
//		comp.tryPerform('relativeOrigin_',true);
		comp.spacing = spacing ? GUI.skin.gap.y;
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.indentedRemaining });
//		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,
				autohidesScrollers=true,autoScrolls=true,
				hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = GUI.scrollView.new(this,bounds ?? { this.bounds });
//		comp.tryPerform('relativeOrigin_',true);
		comp.autohidesScrollers = autohidesScrollers;
		comp.autoScrolls = autoScrolls;
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}
	// not yet finished, do not use yet.
	// you must run replace on the decorator now
	replaceChild { arg oldView,addFunction;
		view.replaceChild(oldView,addFunction)
	}
}

+ FlowLayout {
	// can deprec this since multi page layouts are so old skool
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

	// I'm saying this Rect is abs, fix it for whatever the view wants
	absoluteForView { |view|
		var	b;
		if((view.tryPerform(\relativeOrigin) ? false)) {
			b = view.tryPerform(\absoluteBounds) ?? { view.bounds };
			^this.moveBy(b.left, b.top)
		} {
			^this
		}
	}
	// I'm saying this Rect is rel, fix it for whatever the view wants
	relativeForView { |view|
		var	b;
		if((view.tryPerform(\relativeOrigin) ? false)) {
			^this
		} {
			b = view.bounds;
			^this.moveBy(b.left.neg, b.top.neg)
		}
	}
}
