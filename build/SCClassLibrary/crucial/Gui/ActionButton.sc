
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

	// should move lower
	font_ { arg f;
		view.font = f;
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
FlowView : SCLayoutView {

	var autoRemoves;
	
	*layout { arg f,bounds;
		var v;
		v = this.new(nil,bounds);
		f.value(v);
		^v
	}
	*viewClass { ^SCCompositeView }
	init { arg parent,bounds;
		var w;
		if(parent.isNil,{ 
			parent = SCWindow.new("",bounds).front;
		});
		bounds = if(bounds.notNil,{
			bounds.asRect.moveTo(0,0)
		},{
			parent.asView.bounds.insetAll(2,2,2,2)
		});
		super.init(parent , bounds);
		// after i am placed by parent...
		decorator = FlowLayout(this.bounds,Point(2,2),Point(4,4));
		autoRemoves = IdentitySet.new;
	}

	reflowAll {
		decorator.reset;
		this.children.do({ arg view;
			if(view.isKindOf(StartRow),{
				decorator.nextLine
			},{
				decorator.place(view);
			})
		});
	}
	innerBounds { ^decorator.innerBounds }
	resizeToFit { arg reflow = false,tryParent = false;
		var used,new;

		if(reflow,{ this.reflowAll; });

		used = decorator.used;
		// should respect any settings !
		//used.width = used.width.clip(this.getProperty(\minWidth),this.getProperty(\maxWidth));
		//used.height = used.height.clip(this.getProperty(\minHeight),this.getProperty(\maxHeight));
		
		new = this.bounds.resizeTo(used.width,used.height);
		super.bounds = new;

		decorator.bounds = new; // if the left/top moved this buggers it
		if(reflow,{ this.reflowAll; });
		if(tryParent,{
			this.parent.tryPerform(\resizeToFit,reflow,tryParent);
		});
		^new
	}
	bounds_ { arg b;
		if(b != this.bounds,{
			super.bounds = b;
			if(decorator.notNil,{
				decorator.bounds = b;
				this.reflowAll;
			})
		});
	}
	wouldExceedBottom { arg aBounds; ^decorator.wouldExceedBottom(aBounds) }
	anyChildExceeds {
		var r;
		r = this.bounds;
		^this.children.any({ arg c;
			r.containsRect( c.bounds ).not
		});
	}
			
	// to replace PageLayout
	layRight { arg x,y;
		^Rect(0,0,x,y)
	}
	startRow {
		this.add(StartRow.new); //won't really put a view in there yet
		decorator.nextLine
	}
	removeOnClose { arg updater;
		autoRemoves.add(updater);
	}
	viewDidClose {
		autoRemoves.do({ arg u; u.remove });
		autoRemoves = nil;
		super.viewDidClose;
	}
	hr { arg color,height=3,borderStyle=1; // html joke
		this.startRow;
		// should fill all and still return a minimal bounds 
		SCStaticText(this,Rect(0,0,decorator.innerBounds.width - (2 * 4), height,0))
				.string_("").background_(color ? Color(1,1,1,0.3) ).resize_(2)
	}
}

// abstract
SCButtonAdapter : SCViewHolder {

	classvar <>defaultHeight=17;

	makeView { arg layout,x,y;
		var rect;
		//if((layout.isKindOf(SCLayoutView) or: layout.isKindOf(FlowView)).not,{
		//	layout = layout.asFlowView;
		//});
		if(layout.isNil,{ layout = nil.asFlowView; });
		this.view = SCButton(layout,Rect(0,0,x,y ? defaultHeight));
		if(consumeKeyDowns,{ this.view.keyDownAction_({nil}) });
	}
	flowMakeView { arg layout,x,y;
		this.view = SCButton(layout.asFlowView,Rect(0,0,x,y ? defaultHeight));
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
		view.font_(font ?? {Font("Helvetica",12.0)});
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
