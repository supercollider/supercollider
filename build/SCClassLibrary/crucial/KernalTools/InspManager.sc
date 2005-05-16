
Insp {
	var <subject,<notes,<name,layout,box,hidden = false;
	
	*new { arg subject, notes;
		^super.newCopyArgs(subject,notes).init
	}
	init {
		if(notes.notEmpty,{
			name = notes.first.asString + subject;
		},{
			name = subject.asString;
		});
		if(name.size > 20,{ name = name.copyRange(0,19) ++ "..."});
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
					ag.smallGui(box);
				});
				box.startRow;
				CXObjectInspector(subject).gui(box);
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
				f = SCWindow.new("inspect",SCWindow.screenBounds.insetAll(20,10,100,100));
				h = f.bounds.height - 50;
				w = f.bounds.width;
				menu = SCListView.new(f,100@h);
				menu.items = [insp.name];
				menu.action = { this.showInsp(insps.at(menu.value)) };
				
				inspView = SCCompositeView(f, Rect(120,0,w - 150,h));

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
	}
	remove {
		menu = nil;
		insps.do({ arg in; in.didClose });
		insps = [];
	}
}
