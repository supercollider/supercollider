

SCViewAdapter { // SCViewHolder
	
	// SCViewAdapter makes it possible to wrap more capabilities by holding, not subclassing
	// "has a"  not "is a"
	
	var <>view;
	
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
	refresh { view.refresh }
	background_ { arg b; view.background_(b) }
	focus { arg flag=true; view.focus(flag) }


	// move lower
	font_ { arg f;
		view.font = f;
	}
	prClose { 
		view.prClose 
	}
}



StartRow : SCViewAdapter {
	*new { arg parent,bounds;
		^super.new.view_(parent)
	}
	bounds { ^Rect(0,0,0,0) }
	bounds_ {}
	prClose {}
}


FlowView : SCLayoutView {

	var autoRemoves;
	
	*viewClass { ^SCCompositeView }
	init { arg parent,bounds;
		super.init(parent = parent ?? { var w;
								w = SCWindow.new("",bounds).front;
								if(bounds.notNil,{ bounds = bounds.moveTo(0,0) });
								w
							},
							bounds ?? {parent.asView.bounds});
		decorator = FlowLayout(this.bounds);// after i am placed by parent
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
	resizeToFit {
		var used,new;
		used = decorator.used;
		super.bounds = new = this.bounds.resizeTo(used.width,used.height);
		decorator.bounds = new;
		// don't reflow unless asked
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
	// to replace PageLayout
	layRight { arg x,y;
		^Rect(0,0,x,y)
	}
	startRow {
		// add a StartRow object
		this.add(StartRow.new); //won't really put a view in there yet
		decorator.nextLine
	}
	removeOnClose { arg updater;
		autoRemoves.add(updater);
	}
	prClose {
		autoRemoves.do({ arg u; u.remove });
		super.prClose; // close the view		
	}
	indent {
	
	}
	hr { arg color,height=3,borderStyle=1; // html joke
		this.startRow;
		// should fill all and still return a minimal bounds 
		SCStaticText(this,Rect(0,0,decorator.innerBounds.width - (2 * 4), height,0))
				.string_("").background_(color ? Color(1,1,1,0.3) ).resize_(2)
	}
}


SCButtonAdapter : SCViewAdapter {

	classvar <>defaultHeight=17;

	makeView { arg layout,x,y;
		var rect;
		layout = layout.asFlowView;
		//rect = layout.layRight(x,y ? defaultHeight);
		view = SCButton(layout,Rect(0,0,x,y ? defaultHeight));
	}
	flowMakeView { arg layout,x,y;
		view = SCButton(layout.asFlowView,Rect(0,0,x,y ? defaultHeight));
	}		

	makeViewWithStringSize { arg layout,stringsize,maxx,maxy;
		//maxx is now dependant on font size !
		// NSFont-boundingRectForFont
		// or NSString- (NSSize) sizeWithAttributes: (NSDictionary *) attributes 
		this.makeView( layout,
					(stringsize.clip(3,55) * 8.3).max(maxx?20),
						(maxy ) )
	}
	initOneState { arg name,textcolor,backcolor;
		view.states_([[name,textcolor ? Color.black, backcolor ? Color.white]])
	}
	// all state
	label_ { arg string;
		view.states = view.states.collect({ arg st;
			st.put(0,string.asString);
			st
		});
	}
	// assumes 1 state
	backColor_ { arg color;
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
// no align
//	align_ { arg align;
//		view.align_(align)
//	}
	
}

ActionButton : SCButtonAdapter { // one state

	var <action;
	
	*new { arg layout,title,function,maxx=20,maxy,color,backcolor,font;
		^super.new.init(layout,title,function,maxx,maxy,color,backcolor,font)
	}
	init { arg layout,title,function,maxx=20,maxy,color,backcolor,font;
		title = title.asString;
		this.makeViewWithStringSize(layout,title.size,maxx,maxy);
		view.states_([[title,color ?? {Color.black}, 
			backcolor ?? {Color.new255(205, 201, 201)}]]);
		this.font_(font ?? {Font("Helvetica",12.0)});
		view.action_(function);
	}
}

// CXPopUp
PopUp : ActionButton { // change to use SCPopUp
	
	var <>title,<>list,<>menuLabelsFunc,<>onSelect,index=0;
	
	*new { arg layout,
				title,// or function
				list,//or list-delivering-function
				onSelect,// thing,i
				//optional...
				menuLabelsFunc,initIndex=0,maxx=100;
		var b;
		^b = super.new(
			layout,
			title.value 	?? {menuLabelsFunc.value(list.value.at(initIndex),initIndex)} 
						?? {list.value.at(initIndex).asString},
			{b.doAction},
			maxx,
			13
			)
			.title_(title)
			.list_(list)
			.menuLabelsFunc_(menuLabelsFunc)
			.onSelect_(onSelect)
	}
	doAction {
		CXMenu.newWith(
			list.value.collect({ arg thing,i;
				var name;
				name = menuLabelsFunc.value(thing,i) ?? {thing.asString};
				name -> {index= i; onSelect.value(thing,i);  this.updateTitle(name);  }
			})
		).gui	
	}
	updateTitle { arg default="choose...";
		this.label_(title.value 
						?? {menuLabelsFunc.value(list.value.at(index),index)} 
						?? {list.value.at(index).asString} 
						??  {"choose..."});
	}

}



ToggleButton : SCButtonAdapter {

	var <state,<>onFunction,<>offFunction;
	
	*new { arg layout,title,onFunction,offFunction,init=false,maxx=20,maxy;
			^super.new.init(layout,init, title,maxx,maxy)
				.onFunction_(onFunction).offFunction_(offFunction)
	}
	init { arg layout,init,title,maxx,maxy;
		var offc,onc;
		this.makeViewWithStringSize(layout,title.size,maxx,maxy);
		offc=Color.new255(154, 205, 50);
		onc=Color.new255(245, 222, 179);
		view.states = [
			[title,Color.black,onc],
			[title,Color.black,offc]
		];
		state=init;
		view.setProperty(\value,state.binaryValue);
		view.action_({this.prSetState(state.not)});
	}

	toggle { arg newState;
		this.prSetState(newState ? state.not);
		view.setProperty(\value,state.binaryValue);
	}		
	passiveToggle { arg newState;
		state = newState ? state.not;
		view.setProperty(\value,state.binaryValue);
	}
	value { ^state }
	prSetState { arg newstate; 
		state = newstate;
		if(state,{
			onFunction.value(this)
		},{
			(offFunction ? onFunction).value(this)
		});
	}
}
//
//
//LabelledNumericalView : SCViewAdapter {
//	
//	*new { arg parent,rect,name;
//		^super.new(parent,rect).init(name)
//	}
//	*viewClass { ^SCSlider }
//	init { arg name;
//		
//	
//	}
//
//}
//



