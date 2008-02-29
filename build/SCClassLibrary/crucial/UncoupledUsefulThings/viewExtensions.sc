
+ SCWindow {
	flow { arg func,bounds;
		var f,comp;
		f = FlowView(this,bounds ?? { this.bounds });
		func.value(f);
		f.resizeToFit;
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

+ StartRow {
	children { ^nil }
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
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.bounds });
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.bounds });
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,autohidesScrollers=true,autoScrolls=true,
					hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = GUI.scrollView.new(this,bounds ?? { this.bounds });
		comp.tryPerform('relativeOrigin_',true);
		comp.autohidesScrollers = autohidesScrollers;
		comp.autoScrolls = autoScrolls;
		comp.hasHorizontalScroller = hasHorizontalScroller;
		comp.hasVerticalScroller = hasVerticalScroller;
		func.value(comp);
		^comp
	}

	// not yet working. do not use.
	// this depends on an SCView primitive that I'm not sure about yet.
	// you must run replace on the decorator now
	replaceChild { arg oldView,addFunction;
		var index,newView;
		if(oldView.isKindOf(ObjectGui),{
			oldView = oldView.view; // the flow view that is actually in the children array
		});
		index = children.indexOf(oldView);//.debug("index of old view in children:"+children);
		if(index.isNil,{
			//oldView.insp("removeing");
			//children.insp("children");
			//index.insp("index found");
			//"replaceChild: oldView not found in my children".die;
			^this
		});
		
		oldView.remove;
		newView = addFunction.value(this);
		if(children.size-1 != index,{
			this.swapChildren(index,children.size-1);
		},{
			"was the last view, no swap".debug;
		});
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

	horz { arg func,bounds;
		var comp;
		comp = GUI.hLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	vert { arg func,bounds;
		var comp;
		comp = GUI.vLayoutView.new(this,bounds ?? { this.decorator.indentedRemaining });
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	comp { arg func,bounds;
		var comp;
		comp = GUI.compositeView.new(this,bounds ?? { this.indentedRemaining });
		comp.tryPerform('relativeOrigin_',true);
		func.value(comp);
		^comp
	}
	scroll { arg func,bounds,
				autohidesScrollers=true,autoScrolls=true,
				hasHorizontalScroller=true,hasVerticalScroller=true;
		var comp;
		comp = GUI.scrollView.new(this,bounds ?? { this.bounds });
		comp.tryPerform('relativeOrigin_',true);
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
	scroll { arg ... args;
		^this.view.performList(\scroll,args)
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


