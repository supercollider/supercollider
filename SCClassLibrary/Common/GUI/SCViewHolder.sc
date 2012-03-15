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

	// delegate to the view
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


FlowViewLayout : FlowLayout {
	
	var rows;
	var fOnViewClose;

	*new { arg bounds, margin, gap;
		^super.new(bounds,margin,gap).prInitFlowViewLayout();
	}

	clear {
		rows = [];
		this.reset;
	}

	place { arg view;
		var row;
		if (rows.size < 1) {this.prAddRow};
		rows.last.add( view );
		// Ensure the action is only added once: it will be removed only if already added.
		view.removeAction( fOnViewClose, \onClose );
		view.addAction( fOnViewClose, \onClose );
		super.place( view );
	}

	remove { arg view, reflow = true;
		rows.copy.do { |row|
			row.remove(view);
			if ( (row.size < 1) and: (row !== rows.last) ) {
				rows.remove(row);
			};
		};
	}

	startRow {
		this.prAddRow;
		this.nextLine;
	}

	reflow {
		var newRows;
		this.reset;
		newRows = [];
		rows.do { |row,i|
			row = row.select(_.notClosed);
			if(row.isEmpty.not,{
				newRows = newRows.add(row)
			});
		};
		rows = newRows;
		rows.do { |row|
			row.do { |view| super.place(view) };
			if (row !== rows.last) { this.nextLine };
		};
	}
	wouldExceedBottom { arg aBounds;
		^(top + aBounds.height + margin.y) > bounds.bottom
	}

	rows { ^rows.copy }

	prInitFlowViewLayout {
		fOnViewClose = { |view| this.remove(view); };
	}

	prAddRow { rows = rows.add(List.new); }
}


FlowView : SCViewHolder {

	// a CompositeView with a FlowLayout as its decorator
	// has the advantage that it preserves startRow when the view is resized

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
			if(iMadeParent) { bounds = bounds.moveTo(0, 0) };
		},{
			bounds = parentView.bounds.moveTo(0,0);
		});
		this.view = this.class.viewClass.new(parentView, bounds);

		// parent has placed me, now get my bounds
		bounds = view.bounds.moveTo(0, 0);

		// note: FlowLayout default is 4@4 4@4
		view.decorator = FlowViewLayout(bounds, margin ?? {2@0}, gap ?? {4@4}, false);
		view.decorator.owner = this;
		autoRemoves = IdentitySet.new;
	}
	startRow {
		this.decorator.startRow;
	}
	removeOnClose { arg updater;
		autoRemoves.add(updater);
	}
	hr { arg color,height=3;
		this.startRow;
		StaticText(this,Rect(0,0,this.decorator.innerBounds.width - (2 * this.decorator.gap.x), height,0))
				.string_("").background_(color ?? {Color(1,1,1,0.3)} ).resize_(2);
		this.startRow;
	}

	innerBounds { ^this.decorator.innerBounds }
	bounds_ { arg b, reflow = true;
		if(b != view.bounds,{
			view.bounds = b;
			if(this.decorator.notNil,{
				this.decorator.bounds = b.moveTo(0, 0);
				reflow.if({ this.reflowAll; });
			})
		});
	}
	indentedRemaining { ^this.decorator.indentedRemaining }
	used { ^this.decorator.used }

	reflowAll {
		this.decorator.reflow;
	}
	// returns the new bounds
	resizeToFit { arg reflow = false,tryParent = false;
		var used,new;

		if(reflow,{ this.reflowAll; });

		used = this.decorator.used;
		new = view.bounds.resizeTo(used.width,used.height);
		view.bounds = new;

		this.decorator.bounds = new.moveTo(0, 0);

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

	// if what you are adding is unsure how much space it is going to take
	// then take everything ...
	allocateRemaining {
		prevMaxHeight = this.decorator.maxHeight;
		prevMaxRight = this.decorator.maxRight;
		^this.decorator.indentedRemaining
	}
	// ... and afterwards
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

	remove {
		autoRemoves.do({ |updater| updater.remove });
		autoRemoves = nil;
		view.notClosed.if({
			view.remove;
		});
	}
	viewDidClose {
		autoRemoves.do({ arg u; u.remove });
		autoRemoves = nil;
		view.tryPerform(\viewDidClose);
	}

	children { ^view.children }
	decorator { ^view.decorator }
	decorator_ { |dec| view.decorator = dec }
	add { |child|
		view.add(child);
	}
	removeAll {
		view.removeAll;
		this.decorator.clear;
	}

	asFlowView {}
	asPageLayout {}

	// only SCView calls these
	prRemoveChild { |child|
		view.prRemoveChild(child);
	}
	prClose { view.prClose }
}

