// just a label with the name of the object

StringGui : ObjectGui {

	writeName {}
	gui { arg parent, bounds ... args;
		var layout,string,font;
		var width,height;
		layout=this.guify(parent,bounds);
		font = GUI.font.new(*GUI.skin.fontSpecs);
		if(model.isString,{
			string = " "++model;
		},{
			// floats, integers, symbols will show more clearly what they are
			string = " "++model.asCompileString;
		});
		if(string.size > 1024,{
			string = string.copyRange(0,1024) ++ "...";
		});
		if(bounds.notNil,{
			bounds = bounds.asRect;
		},{
			bounds = Rect(0,0,
						string.bounds.width(font).max(30),
						GUI.skin.buttonHeight);
		});
		this.view = GUI.staticText.new(layout,bounds)
			.stringColor_(GUI.skin.fontColor)
			.font_(font)
			.background_(GUI.skin.background)
			.align_(\left)
			.object_(string);

		if(parent.isNil,{
			layout.resizeToFit;
			layout.front
		});
	}
}

