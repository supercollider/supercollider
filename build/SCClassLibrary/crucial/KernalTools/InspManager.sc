
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
		InspManager.global.watch(this)
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
			},Rect(180,0,775,900));
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
				Sheet({ arg f;
					var h,fb;
					fb = f.view.bounds;
					h = SCHLayoutView(f,fb.insetAll(0,0,20,20));
					menu = CXMenu.newWith([insp.name->{this.showInsp(insp)}]);
					menu.closeOnSelect = false;
					menu.gui(h); 

					inspView = SCCompositeView(h,Rect(180,0,fb.width - 200,fb.height - 20));
					this.showInsp(insp);

				},"-Insp-",SCWindow.screenBounds.insetAll(20,10,100,100))
				.background_(Color.red(0.2,0.15))
				.removeOnClose(this);
				//.window.alpha_(0.96);
				
				nil; 
			}.defer;
		},{
			{
				while({menu ==\pleaseWait},{ 0.1.wait });
				menu.add(insp.name -> { this.showInsp(insp) });
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
		//"InspManager-remove window shut".debug;
		menu = nil;
		insps.do({ arg in; in.didClose });
		insps = [];
	}
}
