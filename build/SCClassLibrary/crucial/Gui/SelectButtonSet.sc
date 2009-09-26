
SelectButtonSet  {

	var butts,<selected=0,<>action,<>colorFunc,<>selectedColor,<>labelArray;

	*new { arg layout,
		/*buttonSizeX,
		buttonSizeY,*/
		labelArrayOrQnty=10,	// integer generates numerical labels
		action,				// action.value(selectedIndex,this)
		color,
		selectedColor,
		width=20,height=20;

		^super.new.init(layout.asFlowView,width,height,labelArrayOrQnty,action,color,selectedColor)
	}

	init { arg layout,x,y,arglabelArray,argaction,
					argcolorFunc,argselectedColor;

		//layout=layout.asFlowView;//PageLayout;
		//layout=layout.asPageLayout;
		action=argaction;
		colorFunc=argcolorFunc ?? { Color.white };
		selectedColor=argselectedColor ?? {Color.red(alpha:0.7)};

		if(arglabelArray.isNumber,{
			 labelArray=Array.series(arglabelArray,0,1)
		},{
			labelArray=arglabelArray
		});

	   butts=
		labelArray.collect({ arg la,i;
			GUI.button.new(layout,(x@y))
				.states_([[la.asString,Color.black,colorFunc.value(i)],
						[la.asString,Color.black,selectedColor.value(i)]])
				.action_({this.select(i)})
		});
		this.refresh;
	}

	select { arg x;
		this.passiveSelect(x);
		^action.value(selected,this);
	}
	passiveSelect { arg x;
		if(butts.at(x).notNil,{
			this.colorNormal(selected);
			this.colorSelected(x);
			selected=x;
		});
	}

	update { arg x;
		this.passiveSelect(x)
	}

	refresh {
		//butts.do({arg bt,i; this.setButtonColor(bt,colorFunc.value(i)) });
		this.colorSelected(selected);
	}

	selectedIndex { ^selected }
	selectedLabel { ^labelArray.at(selected) }
	selectedItem { ^labelArray.at(selected) }

	// leaves the selected number set
	deselect { this.colorNormal(selected) }

	setButtonColor { arg butt,color;
		var s;
		s = butt.states;
		s.at(0).put(2,color);
		butt.states_(s);//.refresh;
	}
	colorNormal { arg i;
		butts.at(i).setProperty(\value, 0);
		this.setButtonColor(butts.at(i),colorFunc.value(i))
	}
	colorSelected { arg i;
		butts.at(i).setProperty(\value, 1);
		this.setButtonColor(butts.at(i),selectedColor.value(i))
	}

	doAction {  ^action.value(selected,this) }

	addAction { arg func;
		var old;
		old=action;
		action={ func.value(selected); old.value(selected) }
	}
	font_ { arg f;
		butts.do({ |b| b.font_(f); });
	}

}

