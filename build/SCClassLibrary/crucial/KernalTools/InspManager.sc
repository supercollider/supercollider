
Insp {
	var <subject,<notes,<guiInstead,<name,layout,box,hidden = false;

	*new { arg subject, notes,guiInstead;
		^super.newCopyArgs(subject,notes,guiInstead ? false).init
	}
	init {
		if(notes.notEmpty,{
			name = subject.asString + "{"++notes.first.asString++"}";
		},{
			name = subject.asString;
		});
		if(name.size > 45,{ name = name.copyRange(0,45) ++ "..."});
		if(InspManager.global.notNil,{
			InspManager.global.watch(this)
		});
	}
	show { arg inspView;
		hidden = false;
		if(box.isNil,{
			this.gui(inspView);
		},{
			box.visible = true;
		})
	}
	gui { arg inspView;
		{
			box = inspView.flow({ arg box;
				// what's the calling method ?
				notes.do({ arg ag;
					if(ag.isString or: {ag.isKindOf(Symbol)},{
						CXLabel(box,ag.asString)
					},{
						InspectorLink(ag,box);
					});
				});
				box.startRow;
				if(guiInstead,{
					subject.gui(box)
				},{
					CXObjectInspector(subject).gui(box);
				});
			}/*,Rect(180,0,775,900)*/);
			box.visible = hidden.not;

			nil
		}.defer
	}
	hide {
		if(box.notNil,{
			box.visible = false;
		});
		hidden = true;
	}
	remove {
		if(box.notNil,{
			box.remove;
			box = nil;
		});
	}
	didClose {
		box = nil;
	}
}

InspManager {

	classvar <global;
	var <insps,menu,<currentInsp,inspView;

	*initClass { global = this.new }

	watch { arg insp;
		insps = insps.add(insp);
		if(menu.isNil, {
			menu = \pleaseWait;
			{
				var h,fb,f,w;
				f = GUI.window.new("inspect",Rect(440,500,1100,900));
				f.view.background = Color.white;
				h = f.bounds.height - 50;
				w = f.bounds.width;
				menu = GUI.listView.new(f,Rect(3,0,200,h));
				menu.font = GUI.font.new("Courier",10);
				menu.background = Color(0.7,0.7,0.7,0.5);
				menu.items = [insp.name];
				menu.action = { this.showInsp(insps.at(menu.value)) };

				inspView = GUI.compositeView.new(f, Rect(203,0,w - 205,h));
//				inspView.tryPerform(\relativeOrigin_,true);
				inspView.background = Color(0.17,0.1,0.1,0.15);
				this.showInsp(insp);

				f.onClose = { this.remove; };
				f.front;
				nil;
			}.defer;
		},{
			{
				while({menu ==\pleaseWait},{ 0.1.wait });
				menu.items = menu.items.add(insp.name);
				this.showInsp(insp);
				nil
			}.defer;
		});
	}
	showInsp { arg insp;
		if(currentInsp.notNil,{
			currentInsp.hide
		});
		currentInsp = insp;
		insp.show(inspView);
		menu.value = insps.indexOf(insp);
	}
	remove {
		menu = nil;
		insps.do({ arg in; in.didClose });
		insps = [];
	}
}
