
SCViewAdapter {
	var <>view;
	
	action_ { arg f; view.action_(f) }

	font_ { arg f;
		view.font = f;
	}
	keyDownAction_ { arg f;
		view.keyDownAction_(f);
	}

}

// 1 state
SCButtonAdapter : SCViewAdapter {

//	*new { arg layout,x,y;
//		^super.new.scbainit(layout,x,y)
//	}
//	
	makeView { arg layout,x,y;
		var rect;
		rect = layout.layRight(x,y);
		view = SCButton(layout.window,rect);
		//view.states_([[title,Color.black,offcolor]]);
	}
	
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
	label_ { arg string;
		var s;
		s = view.states;
		s.at(0).put(0,string.asString);
		view.states = s;
	}
	
}

ActionButton : SCButtonAdapter {

	classvar <>offcolor,<>defaultHeight=17;
	var <action;
	
	*new { arg layout,title,function,maxx=20,maxy;
		^this.prNew(layout,title,function,maxx,maxy)
				.font_(Font("Helvetica",12.0))
	}
	*prNew { arg layout,title,function,maxx=20,maxy;
		var rect,view;
		layout = layout.asPageLayout(title,300,100);
		title = title.asString;
		rect = layout.layRight(
					(title.size.clip(3,55) * 8.3).max(maxx?20),
					(maxy ? defaultHeight));
		view = SCButton(layout, rect )
			.states_([[title,Color.black,offcolor]])
			.action_(function);

		^super.new.view_(view)
	}

	*initClass {
		 offcolor = Color(0.84313725490, 0.81960784313, 0.75686274509 );
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
				menuLabelsFunc,initIndex=0,maxx=100,borderStyle=4;
		var b;
		^b = super.new(
			layout,
			title.value 	?? {menuLabelsFunc.value(list.value.at(initIndex),initIndex)} 
						?? {list.value.at(initIndex).asString},
			{b.doAction},
			maxx,
			13, 
			borderStyle)
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



ToggleButton : ActionButton {

	var <state,<>onFunction,<>offFunction;
	
	*new { arg layout,title,onFunction,offFunction,init=false,maxx=20,maxy=13,borderStyle=4;
			^super.new(layout,title,nil,maxx,maxy,borderStyle)
				.onFunction_(onFunction).offFunction_(offFunction)
				.tbinit(init,title)
	}

	tbinit { arg init,title;
		var offc,onc;
		state=init;
		onc=Color( 0.74117647058824, 0.5921568627451, 0.27450980392157 );
		offc=Color( 0.4078431372549, 0.95686274509804, 0.19607843137255 );
		view.states = [
			[title,Color.black,onc],
			[title,Color.black,offc]
		];
		view.setProperty(\value,state.binaryValue);
		view.action_({this.toggle});
	}

	toggle { arg newState;
		state=newState ? state.not;
		view.setProperty(\value,state.binaryValue);
		if(state,{
			onFunction.value(this)
		},{
			(offFunction ? onFunction).value(this)
		});
	}		
	passiveToggle { arg newState;
		state=newState ? state.not;
		view.setProperty(\value,state.binaryValue);
	}
	value { ^state }
}





