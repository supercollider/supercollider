
CXMenu { // multiple actions

	var <>nameFuncs,<layout,<>backColor;

	*new { arg ... nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	*newWith { arg nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	gui { arg lay,width=150,height=75,xtent=120;
		
		layout=lay.asPageLayout("CXMenu",width,height.clip(700,nameFuncs.size * 20),metal: true);
		layout.backColor_(Color(0.12549019607843, 0.16862745098039, 0.49803921568627, 1));
		
		nameFuncs.do({arg nf;
			ActionButton(layout.startRow,nf.key,{nf.value.value; layout.close},xtent,13,3)
				.backColor_(backColor ? Color.white)
		});
		// if we are not on somebody else's page...
		if(lay.isNil,{
			layout.resizeWindowToFit;
		});
	}
	
	add { arg ass,xtent=85;
		ActionButton(layout.startRow,ass.key,{ass.value.value; layout.close},xtent)
					.backColor_(backColor ? Color.white);
	}
	
}

/*
PopUp : ActionButton {
	
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
			title.value ?? {menuLabelsFunc.value(list.value.at(initIndex),initIndex)} ?? {list.value.at(initIndex).asString},
			{b.doAction},maxx,13, borderStyle)
			.title_(title)
			.list_(list ?? {[]})
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
		this.label_(title.value ?? {menuLabelsFunc.value(list.value.at(index),index)} ?? {list.value.at(index).asString} ? "choose...");
	}

}
*/

/*
(
	f=FlowLayout.new;
	PopUp(f,"choose",[1,2,3,4],{arg int;  int.postln });
	
	PopUp(f,nil,[1,2,3,4],{arg int;  int.postln },{arg thing; thing.asString });


)	
*/

