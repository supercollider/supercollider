
ObjectGui : SCViewAdapter { // aka AbstractController

	var <>model;

	guiBody {arg layout;
		// implement this in your subclass
	}
	
	
	
		
	*new { arg model;
		var new;
		new = super.new;
		new.model_(model);
		model.addDependant(new);
		^new
	}
	guify { arg layout,title,width,height;
		layout = layout ?? {
			//FlowView.new(nil,Rect(10,10,width,height));
			MultiPageLayout(title ?? {model.asString.copyRange(0,50)},width,height);
		};
		layout.removeOnClose(this);
		^layout
	}
	remove {
		model.removeDependant(this);
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
		layout.resizeToFit.front;
	}
	
	writeName { arg layout;
		InspectorLink(model,layout,100)
	}
	
	saveConsole { arg layout;
		^SaveConsole(model,"",layout).save.saveAs.print;
	}
	
	// a smaller format gui, defaults to the tileGui
	smallGui { arg layout;
		this.guify(layout);
		Tile(model,layout);
	}
	
}

