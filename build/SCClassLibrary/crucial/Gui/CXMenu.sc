
CXMenu : SCViewAdapter { // multiple actions

	var <nameFuncs,<layout,<>backColor,<>stringColor,
			<>closeOnSelect=true,lastButton,buttonWidth=150,view,focus = 0;

	*new { arg ... nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	*newWith { arg nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	gui { arg lay,windowWidth=150,height=400,argbuttonWidth=160;
		buttonWidth = argbuttonWidth;
		layout= lay ?? {MultiPageLayout.new};
		view = SCVLayoutView.new(layout,Rect(0,0,buttonWidth,24 * nameFuncs.size));
		this.guiBody;
		this.enableKeyDowns;
		if(lay.isNil,{ layout.resizeToFit.front });
	}
	guiBody {
		nameFuncs.do({arg nf;
			this.add(nf);
		});
	}
	add { arg nf;
		var ab;
		ab = ActionButton(view,nf.key,{
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
			.backColor_(backColor ? Color.new255(112, 128, 144))
			.labelColor_(stringColor ? Color.white);
	}
	resize {
		layout.resizeToFit;
	}
	nameFuncs_ { arg nf;
		nameFuncs = nf;
		if(view.notNil,{
			//view.children.do({ arg c; view.bounds_(Rect(0,0,1,1)).visible_(false) });
			//view.removeAll;
			//view.visible = false;
			//view.bounds = Rect(0,0,0,0);
			//view.parent.resizeToFit;
			view.bounds = Rect(0,0,buttonWidth,24 * nameFuncs.size);
			focus = focus.clip(0,nameFuncs.size - 1);
			this.guiBody;
		})
	}
	doAction {
		view.children.at(focus).doAction
	}
	keyDownResponder {
		var kdr;
		kdr = KeyCodeResponder.new;
		kdr.registerKeycode(10485760,125,{ 
			this.focusOn(focus + 1);
		});
		kdr.registerKeycode(10485760,126,{ 
			this.focusOn(focus - 1);
		});
		// enter
		kdr.registerKeycode(0,52,{
			this.doAction
		});
		// return
		kdr.registerKeycode(0,36,{
			this.doAction
		});
		^kdr
	}
	focusOn { arg f;
		focus = f.clip(0,view.children.size - 1);
		view.children.at( focus ).focus;
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

