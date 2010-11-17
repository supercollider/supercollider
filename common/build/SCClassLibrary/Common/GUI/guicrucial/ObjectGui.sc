
ObjectGui : SCViewHolder {
	/*
		this is a controller class. it creates the views and implements the relationship between them and the model
		model: the object for which this is a graphical user interface
		view: this.view is the flow view (aka the arg layout) that is passed to guiBody
			individual views/widgets are placed in it and their actions talk to either this object or the model
	*/
	var <model,<dragSource;

	guiBody { arg layout;
		/* implement this method in your gui subclass */

		// if your model implement guiBody then call that
		if(model.respondsTo(\guiBody) and: {model.isKindOf(ObjectGui).not},{
			model.guiBody(layout)
		})
	}

	*new { arg model;
		var new;
		new = super.new;
		new.model_(model);
		^new
	}
	guify { arg layout,bounds,title;
		if(layout.isNil,{
			layout = FlowView(nil,bounds,windowTitle:title ?? {model.asString.copyRange(0,50)});
		},{
			layout = layout.asFlowView(bounds);
		});
		// i am not really a view in the hierarchy
		layout.removeOnClose(this);
		^layout
	}
	viewDidClose {
		model.removeDependant(this);
		model = nil;
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
		if(lay.isNil,{
			layout.resizeToFit(true,true);
			layout.front;
		});
	}

	background { ^Color.clear }//^Color.yellow(0.2,0.08) }

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
		// InspectorLink.icon(model,layout);
	}
	model_ { |newModel|
		if(model.notNil,{
			model.removeDependant(this);
			model = newModel;
			model.addDependant(this);
			this.update;
		},{
			model = newModel;
			model.addDependant(this);
		})
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


