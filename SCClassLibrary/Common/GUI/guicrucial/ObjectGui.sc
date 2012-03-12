

ObjectGui : SCViewHolder {
	/*
		this is a controller class:
			it creates the views and implements the relationship between them and the model
		model: the object for which this is a graphical user interface
		view: this.view is the flow view (aka the arg layout) that is passed to guiBody
			individual views/widgets are placed in it and their actions talk to either this object or the model
	*/
	var <model,<dragSource;

	guiBody { arg layout,bounds ... args;
		/* implement this method in your gui subclass */

		// if your model implement guiBody then call that
		// this is a lazy way to write simple guis for simple objects
		// where model/gui code separation is not especially important
		// and where the controller class doesn't need to maintain any state or vars
		if(model.respondsTo(\guiBody) and: {model.isKindOf(ObjectGui).not},{
			model.guiBody(layout,bounds,*args)
		})
	}

	*new { arg model;
		var new;
		new = super.new;
		new.model_(model);
		^new
	}

	gui { arg parent, bounds ... args;
		var layout;
		layout=this.guify(parent,bounds);
		layout.flow({ arg layout;
			this.view = layout;
			this.writeName(layout);
			this.performList(\guiBody,[layout,bounds] ++ args);
		},bounds).background_(this.background);
		//if you created it, front it
		if(parent.isNil,{
			layout.resizeToFit(true,true);
			layout.front;
		});
	}
	guify { arg parent,bounds,title;
		// converts the parent to a FlowView or compatible object
		// thus creating a window from nil if needed
		// registers to remove self as dependent on model if window closes
		if(bounds.notNil,{
			bounds = bounds.asRect;
		});
		if(parent.isNil,{
            parent = PageLayout(title ?? {model.asString.copyRange(0,50)},bounds,front:false);
		},{
			parent = parent.asPageLayout(bounds);
		});
		// i am not really a view in the hierarchy
		parent.removeOnClose(this);
		^parent
	}
	model_ { |newModel|
		if(model.notNil,{ // a view has its model swapped with another
			model.removeDependant(this);
			model = newModel;
			model.addDependant(this);
			this.update;
		},{
			model = newModel;
			model.addDependant(this);
		})
	}
	viewDidClose {
		model.removeDependant(this);
		model = nil;
		super.viewDidClose;
	}

	background { ^Color.clear }

	writeName { |layout|
		this.prWriteName(layout,model.asString)
	}
	prWriteName { arg layout,name;
		var string,font;
		font = GUI.font.new(*GUI.skin.fontSpecs);
		string = " " ++ (name);
		if(string.size > 40,{
			string = string.copyRange(0,40) ++ "...";
		});
		dragSource = GUI.dragSource.new(layout,Rect(0,0,(string.bounds(font).width + 10).max(70),GUI.skin.buttonHeight))
			.stringColor_(Color.new255(70, 130, 200))
			.font_(font)
			.background_(Color.white)
			.align_(\left)
			.beginDragAction_({ model })
			.object_(string);
	}
}

