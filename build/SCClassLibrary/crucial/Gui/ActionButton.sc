

SCButtonAdapter : SCViewAdapter {

	classvar <>defaultHeight=17;

	makeView { arg layout,x,y;
		var rect;
		layout = layout.asPageLayout(this.class.name.asString,300,100);
		rect = layout.layRight(x,y);
		view = SCButton(layout.window,rect);
	}
	makeViewWithStringSize { arg layout,stringsize,maxx,maxy;
		this.makeView( layout,
					(stringsize.clip(3,55) * 8.3).max(maxx?20),
						(maxy ? defaultHeight) )
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
		view.states_([[title,color ?? {Color.black}, backcolor ?? {Color.new255(205, 201, 201)}]]);
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
	
	*new { arg layout,title,onFunction,offFunction,init=false,maxx=20,maxy=13;
			^super.new.init(layout,init, title,maxx,maxy)
				.onFunction_(onFunction).offFunction_(offFunction)
	}
	init { arg layout,init,title,maxx,maxy;
		var offc,onc;
		this.makeViewWithStringSize(layout,title.size,maxx,maxy);
		onc=Color( 0.74117647058824, 0.5921568627451, 0.27450980392157 );
		offc=Color( 0.4078431372549, 0.95686274509804, 0.19607843137255 );
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





