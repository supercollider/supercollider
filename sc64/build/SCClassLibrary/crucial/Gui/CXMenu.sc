
CXMenu : SCViewHolder { // multiple actions

	var <nameFuncs,<layout,<>backColor,<>stringColor,
			<>closeOnSelect=true,lastButton,buttonWidth=150,focus = 0;
	/**
	  * name => { }, name => { }, ...
	  */
	*new { arg ... nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	*newWith { arg nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	gui { arg lay,windowWidth=150,height=400,argbuttonWidth=160;
		buttonWidth = argbuttonWidth;
		layout= lay ?? {MultiPageLayout.new("menu",Rect(20,20,windowWidth,height))};
		view = SCVLayoutView.new(layout,Rect(0,0,buttonWidth,24 * nameFuncs.size));
		this.guiBody;
		this.enableKeyDowns;
		if(lay.isNil,{ layout.front });
	}
	guiBody {
		nameFuncs.do({arg nf;
			this.addToGui(nf);
		});
	}
	add { arg nf;
		nameFuncs = nameFuncs.add(nf);
		if(layout.notNil,{ this.addToGui(nf); });
	}
	addToGui { arg nf;
		var ab;
		ab = ActionButton(view,nf.key,{
				nf.value.value; 
				if(closeOnSelect,{ 
					layout.close
				},{
					if(lastButton.notNil,{ 
						lastButton.background_(Color.new255(112, 128, 144));
						lastButton.labelColor_(Color.white).refresh;
					});
					ab.background_(backColor ? Color.white);
					ab.labelColor_(Color.black).refresh;
					lastButton = ab;
				});
			},buttonWidth)
			.background_(backColor ? Color.new255(112, 128, 144))
			.labelColor_(stringColor ? Color.white);
		view.bounds = view.bounds.resizeTo(buttonWidth,24 * nameFuncs.size);
	}
	resize {
		// what if its not my layout ?
		//layout.resizeToFit;
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
		kdr = UnicodeResponder.new;
		kdr.registerUnicode(KeyCodeResponder.functionKeyModifier , 63233, { 
			this.focusOn(focus + 1);
		});
		kdr.registerUnicode(KeyCodeResponder.functionKeyModifier , 63232 ,{ 
			this.focusOn(focus - 1);
		});
		// enter
		kdr.registerUnicode(KeyCodeResponder.normalModifier,3,{
			this.doAction
		});
		// return
		kdr.registerUnicode(KeyCodeResponder.normalModifier,13,{
			this.doAction
		});
		^kdr
	}
	focus { this.focusOn(0) }
	focusOn { arg f=0;
		focus = f.wrap(0,view.children.size - 1);
		view.children.at( focus ).focus;
	}
}

