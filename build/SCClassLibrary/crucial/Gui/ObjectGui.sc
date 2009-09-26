
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
		},{
			/*
			or by default we make a gui showing variables that the class allows
			 see instVarsForGui
			 by default this is NO variables at all.
			*/
			ObjectGui.guiInstVarsOf(model,layout)
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
			layout = MultiPageLayout(title ?? {model.asString.copyRange(0,50)},bounds,front:false);
		},{
			layout = layout.asPageLayout(title,bounds);
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
			layout.resizeToFit(true,true).front
		});
	}
	topGui { arg ... args;
		this.performList(\gui, args);
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
		InspectorLink.icon(model,layout);
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

	saveConsole { arg layout;
		^SaveConsole(model,"",layout).save.saveAs.print;
	}


	*guiInstVarsOf { arg object,layout;
		// this is an easy default gui system
		// each class returns instVarsForGui, a list of \symbols
		// by default an empty array : "don't display any variables"
		// some classes (Pattern so far) can return all of their instance var names
		// and thus gui all of their internal vars
		var varNames,maxWidth=50,font;
		font = GUI.font.new(*GUI.skin.fontSpecs);
		object.class.instVarsForGui.do({ |ivar,i|
			var width;
			varNames = varNames.add([ivar,object.instVarAt(ivar)]);
			width = ivar.asString.bounds(font).width + 5;
			if(width > maxWidth,{ maxWidth = width; });
		});
		varNames.do({ |namevar|
			var guis;
			layout.startRow;
			VariableNameLabel(namevar[0],layout,maxWidth);
			// a safety check so as not to annoy
			// if there already exists a gui for the object, the simply put a Tile
			guis = namevar[1].dependants.select({|dep| dep.isKindOf(ObjectGui) and: dep.class !== StringGui });
			if(guis.size == 0,{
				namevar[1].gui(layout);
			},{
				Tile(namevar[1],layout);
			});
		});
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


