
Insp {
	var <subject,<notes,<name,layout;
	
	*new { arg subject, notes;
		^super.newCopyArgs(subject,notes).init
	}
	init {
		if(notes.notEmpty,{
			name = notes.first.asString + subject;
		},{
			name = subject.asString;
		});
		InspManager.global.watch(this)
	}	
	gui {
		// ideally this will be a proper snapshot at the time of calling
		{	
			// what's the calling method ?
			layout = Sheet({ arg layout;
				notes.do({ arg ag;
					ag.smallGui(layout);
				});
				layout.startRow;
				CXObjectInspector(subject).topGui(layout);
			},"insp:" + name,200);
			nil
		}.defer
	}
	close {
		if(layout.notNil, { layout.close })
	}
}

InspManager {

	classvar <global;

	var <insps,menu,currentInsp;
	
	*initClass { global = this.new }
	
	watch { arg insp;
		insps = insps.add(insp);	
	
		//if window not open, make
		if(menu.isNil, {
			menu = CXMenu.newWith(insps.collect({ arg insp; 
						insp.name -> {this.showInsp(insp) } 
					}));
					menu.closeOnSelect = false;
			{ 	menu.gui; 
				menu.layout.removeOnClose(this);
				this.showInsp(insp);
				nil; 
			}.defer;
		},{
			{
				// add an entry
				menu.add(insp.name -> {this.showInsp(insp) } );
				menu.resize;
				this.showInsp(insp);
				nil
			}.defer
		});		
	}
	showInsp { arg insp;
		if(currentInsp.notNil,{ currentInsp.close });		
		insp.gui;
		currentInsp = insp;
	}
	remove { // the menu shut
		insps = [];
		insps.do({ arg insp; insp.close });
		menu = nil;
	}
	
}