
ObjectGui : SCViewHolder { // aka AbstractController

	var <>model,<dragSource;

	guiBody { arg layout;
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
			layout = MultiPageLayout(title ?? {model.asString.copyRange(0,50)},bounds);
		},{
			layout = layout.asPageLayout(title,bounds);
		});
		// i am not really a view in the hierarchy
		layout.removeOnClose(this);
		^layout
	}
	viewDidClose {
		model.removeDependant(this);
		super.viewDidClose;
	}
	gui { arg lay, bounds ... args;
		var layout;
		layout=this.guify(lay,bounds);
		layout.flow({ arg layout;
			this.view = layout;
			this.writeName(layout);
			this.performList(\guiBody,[layout] ++ args);
		},bounds).background_(this.background);
		//if you created it, front it
		if(lay.isNil,{ layout.resizeToFit.front });
	}
	topGui { arg ... args;
		this.performList(\gui, args);
	}

	background { ^Color.yellow(0.2,0.15) }
	
	writeName { arg layout;
		var n;
		n = model.asString;
		InspectorLink.icon(model,layout);
		dragSource = SCDragSource(layout,Rect(0,0,(n.size * 7.5).max(70),17))
			.stringColor_(Color.new255(70, 130, 200))
			.background_(Color.white)
			.align_(\center)
			.beginDragAction_({ model })
			.object_(n);	
	}
	
	saveConsole { arg layout;
		^SaveConsole(model,"",layout).save.saveAs.print;
	}
	
}

ModelImplementsGuiBody : ObjectGui {

	gui { arg lay, bounds ... args;
		var layout;
		layout=this.guify(lay,bounds);
		layout.flow({ arg layout;
			this.view = layout;
			this.writeName(layout);
			model.performList(\guiBody,[layout] ++ args);
		},bounds).background_(this.background);
		//if you created it, front it
		if(lay.isNil,{ layout.resizeToFit.front });
	}
}

