
ObjectGui {

	var <>model;
	
	*new { arg model;
		var new;
		new = super.new;
		new.model_(model);
		model.addDependant(new);
		^new
	}
	
	guify { arg layout,title,x,y;
		if(layout.isNil or: {layout.isClosed},{
			layout = PageLayout(title ?? {model.asString},
					600, 600,x ? 100, y ? 100);
		});
		
		NotificationCenter.registerOneShot(layout,\didClose,this,{
			model.removeDependant(this); // when the window shuts
		});
		^layout
	}

	guiBody {arg layout;
		// implement this in your subclass
	}

	gui { arg layout ... args;
		layout=this.guify(layout);
		this.writeName(layout);
		this.performList(\guiBody,[layout] ++ args);
	}
	topGui { arg layout;
		layout=this.guify(layout);
		this.gui(layout);
		layout.resizeWindowToFit;
	}
	
	writeName { arg layout;
		ActionButton(layout,model.asString,{ 	// link to console
			model.inspect;
		},60)
	}
	
	saveConsole { arg layout;
		^SaveConsole(model,"",layout).save.saveAs.print;
	}
	
	// a smaller format gui, defaults to the tileGui
	smallGui { arg layout;
		Tile(this.class,model,layout);
	}		

}

