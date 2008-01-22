
SCViewHolder {
	
	classvar <>consumeKeyDowns = false;// should the view by default consume keydowns

	// SCViewHolder makes it possible to add more capabilities by holding an SCView, not subclassing it
	var <view;
	
	view_ { arg v;
		// subclasses need ALWAYS use this method to set the view
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

// for using inside composite views
// bounds are expressed relative to the parent's
RelativeToParent {
	var <>bounds;
	*new { arg bounds;
		^super.newCopyArgs(bounds)
	}
	place { arg view;
		view.bounds = view.bounds.moveBy(bounds.left,bounds.top)
	}
}


// backwards compat because i changed the name of the class
SCViewAdapter : SCViewHolder { }


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
}

/**
  * a composite view with a FlowLayout as its decorator
  */
FlowView : SCViewHolder {

	var	<parent;
	var	autoRemoves;
	
	*layout { arg f,bounds;
		var v;
		v = this.new(nil,bounds);
		f.value(v);
		^v
	}
	*viewClass { ^GUI.compositeView }
	*new { arg parent, bounds;
		^super.new.init(parent, bounds);
	}
	init { arg argParent, bounds;
		var w;
//">> FlowView:init".postln;
		parent = argParent ?? { GUI.window.new("",bounds).front };
//try { parent.asView.instVarAt(0).postln };
//bounds.debug("bounds");
		if(bounds.notNil,{
			bounds = bounds.asRect  // .moveTo(0,0)  // why was this here? bad idea
		},{
			bounds = parent.asView.bounds.insetAll(2,2,2,2);
			if(parent.tryPerform(\relativeOrigin) ? false) {
				bounds = bounds.setOriginRelativeTo(parent);
			};
		});
			// this adds the composite view to the parent composite view
		view = this.class.viewClass.new(parent.asView, bounds);
//"created composite view [".post; view.instVarAt(0).post; "]".postln;
			// now a tricky hack... the parent needs the FlowView as a child, not the composite view
			// so I will replace the last-added child with THIS
		parent.asView.children[parent.asView.children.size-1] = this;

		if(view.tryPerform(\relativeOrigin) ? false) {
			bounds = view.bounds.moveTo(0, 0);
		} {
			bounds = view.bounds;
		};
//bounds.debug("initialize decorator bounds to");
		// after i am placed by parent...
		view.decorator = FlowLayout(bounds,Point(2, 2),Point(4, 4));
		autoRemoves = IdentitySet.new;
//"<< FlowView:init".postln;
	}

	reflowAll {
//">> FlowView:reflowAll - ".post;
//view.instVarAt(0).postln;
		view.decorator/*.bounds_(this.bounds)*/.reset;
		view.children.do({ |widget|
			if(widget.isKindOf( StartRow ),{
				view.decorator.nextLine
			},{
				view.decorator.place(widget);
			})
		});
//"<< FlowView:reflowAll - ".post;
//view.instVarAt(0).postln;
	}
	innerBounds { ^view.decorator.innerBounds }
	resizeToFit { arg reflow = false,tryParent = false;
		var used,new;

		if(reflow,{ this.reflowAll; });

		used = view.decorator.used;
		// should respect any settings !
		//used.width = used.width.clip(this.getProperty(\minWidth),this.getProperty(\maxWidth));
		//used.height = used.height.clip(this.getProperty(\minHeight),this.getProperty(\maxHeight));
		
		new = view.bounds.resizeTo(used.width,used.height);
		view.bounds = new;

		if(view.tryPerform(\relativeOrigin) ? false) {
			view.decorator.bounds = new.moveTo(0, 0);
		} {
			view.decorator.bounds = new; // if the left/top moved this buggers it
		};
		if(reflow,{ this.reflowAll; });
		if(tryParent,{
			this.parent.tryPerform(\resizeToFit,reflow,tryParent);
		});
		^new
	}
	bounds_ { arg b, reflow = true;
//">> FlowView:bounds_: %, ".postf(b);
//try { view.instVarAt(0).post };
//"\n".post;
		if(b != view.bounds,{
			view.bounds = b;
			if(view.decorator.notNil,{
				if(view.tryPerform(\relativeOrigin) ? false) {
					view.decorator.bounds = b.moveTo(0, 0) //.debug("set decorator bounds to");
				} {
					view.decorator.bounds = b //.debug("set decorator bounds to");
				};
				reflow.if({ this.reflowAll; });
			})
		}/*, {
			view.decorator.bounds.debug("decorator bounds remain");
		}*/);
//"<< FlowView:bounds_ ".post;
//try { view.instVarAt(0).post };
//"\n".post;
	}
	wouldExceedBottom { arg aBounds; ^view.decorator.wouldExceedBottom(aBounds) }
	anyChildExceeds {
		var r;
		r = view.bounds;
		^view.children.any({ arg c;
			r.containsRect( c.bounds ).not
		});
	}
			
	// to replace PageLayout
	layRight { arg x,y;
		^Rect(0,0,x,y)
	}
	startRow {
		view.add(StartRow.new/*(this)*/); //won't really put a view in there yet
		view.decorator.nextLine
	}
	removeOnClose { arg updater;
		autoRemoves.add(updater);
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
	hr { arg color,height=3,borderStyle=1; // html joke
		this.startRow;
		// should fill all and still return a minimal bounds 
		GUI.staticText.new(this,Rect(0,0,view.decorator.innerBounds.width - (2 * 4), height,0))
				.string_("").background_(color ? Color(1,1,1,0.3) ).resize_(2)
	}
	
		// other messages to delegate -- must mimic SCLayoutView interface
	children { ^view.children }
	decorator { ^view.decorator }
	decorator_ { |dec| view.decorator = dec }
	add { |child|
		view.add(child);
	}
	removeAll {
		view.removeAll;
	}
	prRemoveChild { |child|
		view.prRemoveChild(child);
	}
	prClose { view.prClose }
}

// abstract
SCButtonAdapter : SCViewHolder {

	classvar <>defaultHeight=17;

	makeView { arg layout,x,y;
		var rect;
		//if((layout.isKindOf(SCLayoutView) or: layout.isKindOf(FlowView)).not,{
		//	layout = layout.asFlowView;
		//});
		if((layout.isNil or: { layout.isKindOf(MultiPageLayout) }),{ layout = layout.asFlowView; });
		this.view = GUI.button.new(layout,Rect(0,0,x,y ? defaultHeight));
		if(consumeKeyDowns,{ this.view.keyDownAction_({nil}) });
	}
	flowMakeView { arg layout,x,y;
		this.view = GUI.button.new(layout.asFlowView,Rect(0,0,x,y ? defaultHeight));
		if(consumeKeyDowns,{ this.view.keyDownAction_({nil}); });
	}		

	makeViewWithStringSize { arg layout,stringsize,minWidth,minHeight;
		// minWidth is now dependant on font size !
		// NSFont-boundingRectForFont
		// or NSString- (NSSize) sizeWithAttributes: (NSDictionary *) attributes 
		this.makeView( layout,
					(stringsize.clip(3,55) * 7.9).max(minWidth?20),
						(minHeight ) )
	}
	initOneState { arg name,textcolor,backcolor;
		view.states_([[name,textcolor ? Color.black, backcolor ? Color.white]])
	}
	// sets all states
	label_ { arg string;
		view.states = view.states.collect({ arg st;
			st.put(0,string.asString);
			st
		});
	}
	// assumes 1 state
	background_ { arg color;
		var s;
		s = view.states;
		s.at(0).put(2,color);
		view.states = s;
	}	
	labelColor_ { arg color; 	
		var s;
		s = view.states;
		s.at(0).put(1,color);
		view.states = s;
	}
}

// abreviation for a one state button
ActionButton : SCButtonAdapter {

	var <action;
	
	*new { arg layout,title,function,minWidth=20,minHeight,color,backcolor,font;
		^super.new.init(layout,title,function,minWidth,minHeight,color,backcolor,font)
	}
	init { arg layout,title,function,minWidth=20,minHeight,color,backcolor,font;
		var environment;
		title = title.asString;
		this.makeViewWithStringSize(layout,title.size,minWidth,minHeight);
		view.states_([[title,color ?? {Color.black}, 
			backcolor ?? {Color.new255(205, 201, 201)}]]);
		view.font_(font ?? {GUI.font.new("Helvetica",12.0)});
		view.action_(function);
		if(consumeKeyDowns,{ this.keyDownAction = {nil}; });
	}
}


ToggleButton : SCButtonAdapter {

	var <state,<>onFunction,<>offFunction;
	
	*new { arg layout,title,onFunction,offFunction,init=false,minWidth=20,minHeight;
			^super.new.init(layout,init, title,minWidth,minHeight)
				.onFunction_(onFunction).offFunction_(offFunction)
	}
	value { ^state }
	toggle { arg way,doAction = true;
		if(doAction,{		
			this.prSetState(way ? state.not)
		},{
			state = way ? state.not;
		});
		view.setProperty(\value,state.binaryValue);
	}
	// private
	init { arg layout,init,title,minWidth,minHeight;
		var offc,onc;
		this.makeViewWithStringSize(layout,title.size,minWidth,minHeight);
		offc=Color.new255(154, 205, 50);
		onc=Color.new255(245, 222, 179);
		view.states = [
			[title,Color.black,layout.asView.background],
			[title,Color.black,onc]
		];
		state=init;
		view.setProperty(\value,state.binaryValue);
		view.action_({this.prSetState(state.not)});
	}
	prSetState { arg newstate; 
		state = newstate;
		if(state,{
			onFunction.value(this)
		},{
			(offFunction ? onFunction).value(this)
		});
	}
}
