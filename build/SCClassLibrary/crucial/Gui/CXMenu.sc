
CXMenu { // multiple actions

	var <>nameFuncs,<layout,<>backColor,<>closeOnSelect=true,lastButton,buttonWidth=150;

	*new { arg ... nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	*newWith { arg nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	gui { arg lay,windowWidth=150,height=400,argbuttonWidth=120,title="CXMenu";
		
		buttonWidth = argbuttonWidth;
		height = max(height,nameFuncs.size * 20);
		layout=lay.asPageLayout(title,windowWidth,height,metal: true);

		nameFuncs.do({arg nf;
			this.add(nf);
		});
		// if we are not on somebody else's page...
		if(lay.isNil,{
			layout.resizeWindowToFit.front;
		});
	}		
	
	add { arg nf;
		var ab;
		ab = ActionButton(layout.startRow,nf.key,{
				nf.value.value; 
				if(closeOnSelect,{ 
					layout.close 
				},{
					if(lastButton.notNil,{ 
						lastButton.backColor_(backColor ? Color.white);
						lastButton.labelColor_(Color.black);
					});
					ab.backColor_(Color.new255(112, 128, 144));
					ab.labelColor_(Color.white);
					lastButton = ab;
				});
			},buttonWidth)
			.backColor_(backColor ? Color.white);
	}
	resize {
		layout.resizeWindowToFit;
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
	f=PageLayout.new;
	PopUp(f,"choose",[1,2,3,4],{arg int;  int.postln });
	
	PopUp(f,nil,[1,2,3,4],{arg int;  int.postln },{arg thing; thing.asString });


)	
*/

