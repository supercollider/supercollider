
ActionButton  {

	classvar <>offcolor;
	var <>action,<>butt;
	
	*new { arg layout,title,function,maxx=20,maxy=17;
		^this.prNew(layout,title,function,maxx,maxy)
				.font_(Font("Helvetica",12.0))
	}
	*prNew { arg layout,title,function,maxx=20,maxy=17;
		var rect,butt;
		layout = layout.asPageLayout(title,300,100);
		title = title.asString;
		rect = layout.layRight(
					(title.size * 8.3).max(maxx?20),
					(maxy ? 17));
		butt = SCButton(layout, rect )
			.states_([[title,Color.black,offcolor]])
			.action_(function);

		^super.new.butt_(butt)
	}

	backColor_ { arg color;
		var s;
		s = butt.states;
		s.at(0).put(2,color);
		butt.states = s;
	}	
	labelColor_ { arg color; 	
		var s;
		s = butt.states;
		s.at(0).put(1,color);
		butt.states = s;
	}
	label_ { arg string;
		var s;
		s = butt.states;
		s.at(0).put(0,string.asString);
		butt.states = s;
	}
	font_ { arg f;
		butt.font = f;
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

	var <>offc,<>onc;
	var <state,<>onFunction,<>offFunction;
	
	*new { arg layout,title,onFunction,offFunction,init=false,maxx=20,maxy=13,borderStyle=4;
	
			^super.new(layout,title,nil,maxx,maxy,borderStyle)
				.onFunction_(onFunction).offFunction_(offFunction)
				.tbinit(init)
	}

	tbinit { arg init;
		state=init;
		offc=Color( 0.74117647058824, 0.5921568627451, 0.27450980392157 );
		onc=Color( 0.4078431372549, 0.95686274509804, 0.19607843137255 );
		this.backColor_(if(state,onc,offc));
		this.action_({this.toggle});
	}

	toggle { arg newState;
		state=newState ? state.not;
		this.backColor_(if(state,onc,offc));
		if(state,{
			onFunction.value(this)
		},{
			(offFunction ? onFunction).value(this)
		});
	}		
	passiveToggle { arg newState;
		state=newState ?? {state.not};
		this.backColor_(if(state,onc,offc));
	}
	value { ^state }
}





