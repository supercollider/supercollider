
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
		if(name.size > 20,{ name = name.copyRange(0,19) ++ "..."});
		InspManager.global.watch(this)
	}	
	gui { arg origin;
		// ideally this will be a proper snapshot at the time of calling
		// keep same location as last window
		if(layout.notNil and: {layout.isClosed.not},{ layout.front.show; ^this });
		{	
			if(this === InspManager.global.currentInsp, {
				// what's the calling method ?
				if(origin.isNil, { origin = 300@10; });
				layout = Sheet({ arg layout;
					notes.do({ arg ag;
						ag.smallGui(layout);
					});
					layout.startRow;
					CXObjectInspector(subject).gui(layout);
				},"insp:" + name,origin.x,origin.y,800,1000);
			});
			nil
		}.defer
	}
	origin { 
		^if(layout.notNil,{
			layout.bounds.origin
		},{
			nil
		}) 
	}
	hide {
		if(layout.notNil,{
			layout.hide;
		})
	}
	close {
		if(layout.notNil, { layout.close })
	}
}

InspManager {

	classvar <global;

	var <insps,menu,<currentInsp;
	
	*initClass { global = this.new }
	
	watch { arg insp;

		insps = insps.add(insp);	
	
		//if window not open, make
		if(menu.isNil, {
			menu = CXMenu.newWith(insps.collect({ arg insp; 
						insp.name -> {this.showInsp(insp) } 
					}));
					menu.closeOnSelect = false;
			{ 	menu.gui(nil,200,700,180); 
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
		var origin;
		if(currentInsp.notNil,{ 
			//origin = currentInsp.origin;
			currentInsp.hide 
		});			
		currentInsp = insp;
		insp.gui; //(origin);
	}
	remove { // the menu shut
		insps.do({ arg insp; insp.close });
		insps = [];
		menu = nil;
	}
	
}
