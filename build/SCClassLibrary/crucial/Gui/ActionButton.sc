
ActionButton  {

	classvar <>offcolor;
	var <>action,<>butt;
	
	*new { arg layout,title,function,maxx=20,maxy=17,borderStyle=3;
		var rect,butt;
		layout=PageLayout.guify(layout);
		butt = SCButton(layout, layout.layRight(
					(title.asString.size * 9).max(maxx?20),
					(maxy ? 17)))
			.states_([[title.asString,Color.black,offcolor]])
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
	*initClass {
		 offcolor = Color(0.84313725490, 0.81960784313, 0.75686274509 );
	}

}


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
		Menu.newWith(
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






ClassNameLabel : ActionButton {

	*new { arg  class,layout,maxx=200;
		^super.new(layout,class.name.asString,{ClassInspector(class)},maxx,13,2)
			.backColor_(Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 ));
	}

}


MethodLabel : ActionButton {

	*new { arg  method,layout,maxx=100;
		^super.new(layout,method.ownerClass.name.asString ++ "::" ++ method.name.asString,
			{MethodInspector(method)},maxx,13,2)
				.backColor_(Color( 0.62745098039216, 0.8078431372549, 0.57647058823529 ));
	}
	*withoutClass { arg  method,layout;
		^super.new(layout, "::" ++ method.name.asString,{MethodInspector(method)},100,13,2)
			.backColor_(Color( 0.62745098039216, 0.8078431372549, 0.57647058823529 ));
	}
//	// search by selector link
//	*browseSelectorLink { arg method,layout;
//		^super.new(layout, "::" ++ method.name.asString,{SelectorBrowser(method.name).gui},100,13,2).backColor_(Color(160,206,147));
//	}
}


SelectorLabel : ActionButton {
	
	*new { arg  selector,layout,maxx=100; // can send it to a MethodBrowser
		^super.new(layout,selector.asString,{},maxx,13,2)
			.backColor_(Color( 0.74509803921569, 0.8078431372549, 0.57647058823529 ));
	}
}

//aka GuiLink
Tile : ActionButton {

	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{ 
				Sheet({ arg f; 
					target.gui(f); 
				},target.asString)
			},maxx,13,3)
			.backColor_(Color( 0.90588235294118, 0.98039215686275, 0.69803921568627 ))
			.labelColor_(Color.blue)
	}

}

InspectorLink : ActionButton {
	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{target.inspect},maxx,13,3)
			.backColor_(Color( 0.5921568627451, 0.56862745098039, 1 ))
			.labelColor_(Color.blue)
	}
}

VariableNameLabel : ActionButton { // also used for arg name
	*new { arg name,layout,maxx=100;
		^super.new(layout,name,maxx: maxx).backColor_(Color( 1, 0.86666666666667, 0.38039215686275 ));
	}
}

ArgNameLabel : ActionButton {
	*new { arg name,layout,maxx=100;
		^super.new(layout,name,maxx: maxx).backColor_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ));
	}
}
				

