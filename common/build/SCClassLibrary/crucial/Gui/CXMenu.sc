
CXMenu : SCViewHolder { // multiple actions

	var <nameFuncs, <layout, <>backColor, <>stringColor, <>closeOnSelect=true;

	var lastButton,buttonWidth=150,focus = 0,isMyLayout=false;

	// name -> { }, name -> { }, ...
	*new { arg ... nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	*newWith { arg nameFuncs;
		^super.new.nameFuncs_(nameFuncs)
	}
	gui { arg lay,windowWidth=170,height=400,argButtonWidth=160;
		var w,wbounds;
		buttonWidth = argButtonWidth;
		if(windowWidth.isNumber,{ // backwards compat
			wbounds = Rect(20,20,windowWidth ? buttonWidth + 10,height);		},{
			wbounds = windowWidth.asRect
		});
		layout = lay ?? {
			isMyLayout = true;
			MultiPageLayout.new("menu",wbounds,scroll:true);
		};
		//view = GUI.scrollView.new(layout,Rect(0,0,buttonWidth+4,height));
		view = GUI.vLayoutView.new(layout,
			Rect(0,0,buttonWidth, (GUI.skin.buttonHeight + GUI.skin.gap.y) * nameFuncs.size));
		this.guiBody(false);
		this.enableKeyDowns;
		if(isMyLayout,{ layout.resizeToFit });
		if(lay.isNil,{ layout.front });
	}
	guiBody { arg resize;
		nameFuncs.do({arg nf;
			this.addToGui(nf,resize);
		});
	}
	add { arg nf;
		nameFuncs = nameFuncs.add(nf);
		if(layout.notNil,{ this.addToGui(nf); });
	}
	addToGui { arg nf,resize=true;
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
		view.bounds = view.bounds.resizeTo(buttonWidth,
						(GUI.skin.buttonHeight + GUI.skin.gap.y) * nameFuncs.size);
		/*if(isMyLayout,{
			if(layout.window.view.bounds.containsRect(view.bounds).not,{
				layout.resizeToFit;
			})
		});*/
	}
	resize {
		if(isMyLayout,{
			layout.resizeToFit;
		});
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

