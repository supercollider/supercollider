
ObjectGui {

	var <>model;
	
	*new { arg model;
		var new;
		new = super.new;
		new.model_(model);
		model.addDependant(new);
		^new
	}
	
	guify { arg layout,title,width,height;
		layout = layout ?? {
			PageLayout(title ?? {model.asString},width,height);
		};
		layout.removeOnClose(this);
//		NotificationCenter.registerOneShot(layout,\didClose,this,{
//			model.removeDependant(this); // when the window shuts
//		});
		^layout
	}
	remove {
		model.removeDependant(this);
	}
	guiBody {arg layout;
		// implement this in your subclass
	}

	gui { arg lay ... args;
		var layout;
		layout=this.guify(lay);
		this.writeName(layout);
		this.performList(\guiBody,[layout] ++ args);
		//if you created it, front it
		if(lay.isNil,{ layout.front });
	}
	topGui { arg layout ... args;
		layout=this.guify(layout);
		this.performList(\gui,[layout] ++ args);
		layout.resizeWindowToFit.front;
	}
	
	writeName { arg layout;
		InspectorLink(model,layout,100)
	}
	
	saveConsole { arg layout;
		^SaveConsole(model,"",layout).save.saveAs.print;
	}
	
	// a smaller format gui, defaults to the tileGui
	smallGui { arg layout;
		Tile(model,layout);
	}		

}

