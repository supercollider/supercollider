
// SCViewHolder makes it possible to add more capabilities by holding an SCView, not subclassing it
SCViewHolder {

	classvar <>consumeKeyDowns = false;// should the view by default consume keydowns

	var <view;

	view_ { arg v;
		// subclasses need to ALWAYS use this method to set the view
		view = v;
		view.onClose = { this.viewDidClose; };
	}
	viewDidClose { view = nil; }
	remove {
		if(view.notNil,{
			view.remove;// will cause view.onClose
			//view = nil;
		});
	}

	action { ^view.action }
	action_ { arg f; view.action_(f) }
	doAction { view.doAction }
	keyDownAction_ { arg f;
		view.keyDownAction_(f);
	}
	keyDownResponder { ^nil }
	enableKeyDowns { this.keyDownAction = this.keyDownResponder }

	asView { ^view }
	bounds { ^view.bounds }
	bounds_ { arg b; view.bounds_(b) }
	resize_ { arg r; view.resize_(r) }
	enabled { ^view.enabled }
	enabled_ { |b| view.enabled_(b) }
	refresh { view.refresh }
	background { ^view.background }
	background_ { arg b; view.background_(b) }
	focus { arg flag=true; view.focus(flag) }
	visible_ { arg boo; view.visible = boo }

	isClosed { ^(view.isNil or: {view.isClosed}) }
	// should move lower
	font_ { arg f;
		view.font = f;
	}

		// allow all messages the view understands to delegate to the view
	doesNotUnderstand { |selector ... args|
		var	result;
		view.respondsTo(selector).if({
			result = view.performList(selector, args);
			^(result === view).if({ this }, { result });
		}, {
			DoesNotUnderstandError(this, selector, args).throw;
		});
	}
}

// a decorator for use with composite views that are set to relativeOrigin = false
// bounds can then be expressed relative to the parent's
RelativeToParent {
	var <>bounds;
	*new { arg bounds;
		^super.newCopyArgs(bounds)
	}
	place { arg view;
		view.bounds = view.bounds.moveBy(bounds.left,bounds.top)
	}
}

SCViewAdapter : SCViewHolder {}

/**
  * a non-visible view that keeps the place in a flow layout
  * to mark where a row was started.  then when the view is reflowed due to resizing etc.
  * the layout can do a new row when this is encountered.
  * this is only added by FlowView.startRow
  */
StartRow : SCViewHolder {
	*new { arg parent,bounds;
		var new;
		new = super.new;
		if(parent.notNil,{ new.view_(parent) });
		^new
	}
	bounds { ^Rect(0,0,0,0) }
	bounds_ {}
	prClose {}
	remove {
		super.remove;
		if(view.notNil,{
			view.prRemoveChild(this)
		});
	}
}

/**
  * a composite view with a FlowLayout as its decorator
  */
FlowView : SCViewHolder {
		// It is no longer officially supported to use absolute-origin views
		// with FlowView. It "should" work for this but it is not guaranteed.
//	classvar <>relativeOrigin = true;

	var	<parent;
	var	autoRemoves,prevMaxHeight,prevMaxRight;

	*layout { arg f,bounds;
		var v;
		v = this.new(nil,bounds);
		f.value(v);
		^v
	}
	*viewClass { ^GUI.compositeView }
	*new { arg parent, bounds,margin,gap,windowTitle="";
		^super.new.init(parent, bounds,margin,gap,windowTitle);
	}
	init { arg argParent, bounds,margin,gap,windowTitle="";
		var w, parentView, iMadeParent = false;
		parent = argParent ?? {
			iMadeParent = true;
			GUI.window.new(windowTitle,bounds).front
		};
		parentView = parent.asView;
		if(bounds.notNil,{
			bounds = bounds.asRect;
				// this covers FlowView(nil, Rect(...))
			if(iMadeParent) { bounds = bounds.moveTo(0, 0) };
		},{
			bounds = parentView.bounds;//.insetAll(2,2,2,2);
			if((parentView.tryPerform(\prRelativeOrigin) ? true)
					// note, this check (suboptimal, but a necessary compromise
					// until relativeOrigin is gone for good) assumes a scrollTopView
					// will have origin 0,0 - should be a safe assumption
					or: { GUI.schemes.any { |kit| parentView.isKindOf(kit.scrollView) } }) {
				bounds = bounds.setOriginRelativeTo(parentView);
			};
		});
		// this adds the composite view to the parent composite view
		view = this.class.viewClass.new(parentView, bounds);
		// now a tricky hack... the parent needs the FlowView as a child, not the composite view
		// so I will replace the last-added child with THIS
		if(parentView.children[parentView.children.size-1] === view,{
			parentView.children[parentView.children.size-1] = this;
		},{
			Error("FlowView unexpected result : parent's last child is not my view").throw;
		});

		// the parent might be a vertical, horizontal or flow
		// and might now have placed me, so get the bounds
		bounds = view.bounds;
		if(view.tryPerform(\prRelativeOrigin) ? true) {
			bounds = bounds.moveTo(0, 0);
		};
		//view.decorator = FlowLayout(bounds,2@2/*GUI.skin.margin*/,4@4);
		view.decorator = FlowLayout(bounds,margin ?? {2@0},gap ?? {4@4});
		autoRemoves = IdentitySet.new;
	}
	startRow {
		view.add(StartRow.new); //won't really put a view in there yet
		this.decorator.nextLine
	}
	removeOnClose { arg updater;
		autoRemoves.add(updater);
	}
	hr { arg color,height=3,borderStyle=1; // html joke
		this.startRow;
		// should fill all and still return a minimal bounds
		GUI.staticText.new(this,Rect(0,0,this.decorator.innerBounds.width - (2 * 4), height,0))
				.string_("").background_(color ? Color(1,1,1,0.3) ).resize_(2)
	}

	innerBounds { ^this.decorator.innerBounds }
	bounds_ { arg b, reflow = true;
		if(b != view.bounds,{
			view.bounds = b;
			if(this.decorator.notNil,{
				if(view.tryPerform(\prRelativeOrigin) ? true) {
					this.decorator.bounds = b.moveTo(0, 0)
				} {
					this.decorator.bounds = b
				};
				reflow.if({ this.reflowAll; });
			})
		});
	}
	indentedRemaining { ^this.decorator.indentedRemaining }
	used { ^this.decorator.used }

	reflowAll {
		this.decorator.reset;
		view.children.do({ |widget|
			if(widget.isKindOf( StartRow ),{
				this.decorator.nextLine
			},{
				this.decorator.place(widget);
			})
		});
	}
	// returns the new bounds
	resizeToFit { arg reflow = false,tryParent = false;
		var used,new;

		if(reflow,{ this.reflowAll; });

		used = this.decorator.used;

		// should respect any settings !
		//used.width = used.width.clip(this.getProperty(\minWidth),this.getProperty(\maxWidth));
		//used.height = used.height.clip(this.getProperty(\minHeight),this.getProperty(\maxHeight));

		new = view.bounds.resizeTo(used.width,used.height);
		view.bounds = new;

		// ? but if relativeOrigin is true, then the decorator is already relative
		if(view.tryPerform(\prRelativeOrigin) ? true) {
			this.decorator.bounds = new.moveTo(0, 0);
		} {
			this.decorator.bounds = new; // if the left/top moved this buggers it
		};
		if(reflow,{ this.reflowAll; });
		// its better to call reflowDeep on the parent
		if(tryParent,{
			this.parent.tryPerform(\resizeToFit,reflow,tryParent);
		});
		^new
	}

	reflowDeep {
		this.allChildren.reverseDo({ |view|
			if(view.isKindOf(FlowView),{
				view.bounds_(view.bounds.resizeTo(2000,2000),false);
				view.reflowAll.resizeToFit;
			});
		});
		//		best way:
		//		enlarge the view to full bounds within its parent.
		// 		this can only be done if you are the last or only child
		//		call deepReflow on shallow children in reverse order
		//		resize self to fit
	}
	front {
		// window.front 
		
	}

	wouldExceedBottom { arg aBounds; ^this.decorator.wouldExceedBottom(aBounds) }
	anyChildExceeds {
		var r;
		r = view.bounds;
		^view.children.any({ arg c;
			r.containsRect( c.bounds ).not
		});
	}

	// if the view you are adding is unsure how much space it is going to take
	// then take everything
	allocateRemaining {
		prevMaxHeight = this.decorator.maxHeight;
		prevMaxRight = this.decorator.maxRight;
		^this.decorator.indentedRemaining
	}
	// and afterwards
	// state what you actually used.
	// see FlowView-flow
	didUseAllocated { arg vbounds;
		if(prevMaxHeight.isNil,{
			Error("didUseAllocated called without prior call to allocateRemaining").throw;
		});
		this.decorator.left = vbounds.right + this.decorator.margin.x;
		// maxRight is max right of all rows ever laid
		// but maxHeight is the max of this row only
		// but they both have to be rescinded
		this.decorator.maxRight = max( prevMaxRight, vbounds.right );
		this.decorator.maxHeight = max( prevMaxHeight,vbounds.height);
	}

	// to replace PageLayout
	layRight { arg x,y;
		^Rect(0,0,x,y)
	}
	remove {
		autoRemoves.do({ |updater| updater.remove });
		autoRemoves = nil;
		// am I still alive in the window?
		view.notClosed.if({
			// since this is in the parent's children array, view.remove is not enough by itself
			this.parent.prRemoveChild(this);
			view.remove;
		});
	}
	viewDidClose {
		autoRemoves.do({ arg u; u.remove });
		autoRemoves = nil;
		view.viewDidClose;
	}

	// mimic SCLayoutView interface
	children { ^view.children }
	decorator { ^view.decorator }
	decorator_ { |dec| view.decorator = dec }
	add { |child|
		view.add(child);
	}
	removeAll {
		view.removeAll;
		// SCContainerView removeAll is a bit odd
		// it relies on the children to remove themselves
		// but a StartRow doesn't ever know its parent
		// so it doesn't remove itself
		view.children.do({ |child|
			if(child.isKindOf(StartRow),{
				view.prRemoveChild(child)
			})
		});

		this.decorator.reset;
	}

	//private
	prRemoveChild { |child|
		view.prRemoveChild(child);
	}
	prClose { view.prClose }
}

