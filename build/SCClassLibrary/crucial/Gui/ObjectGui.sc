
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
	guify { arg layout,bounds,title;
		if(layout.isNil,{
			//FlowView.new(nil,Rect(10,10,width,height));
			layout = MultiPageLayout(title ?? {model.asString.copyRange(0,50)},bounds);
		},{
			layout = layout.asPageLayout(title,bounds);
		});
		//layout.removeOnClose(this);
		^layout
	}
	remove {
		model.removeDependant(this);
		super.remove;
	}

	gui { arg lay, bounds ... args;
		var layout;
		layout=this.guify(lay,bounds);
		layout.flow({ arg layout;
			view = layout;
			this.writeName(layout);
			this.performList(\guiBody,[layout] ++ args);
		}).background_(Color.yellow(0.2,0.15));
		//if you created it, front it
		if(lay.isNil,{ layout.resizeToFit.front });
	}
	topGui { arg layout ... args;
		this.performList(\gui,[layout] ++ args);
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

