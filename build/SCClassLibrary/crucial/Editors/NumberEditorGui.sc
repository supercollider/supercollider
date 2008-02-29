
EditorGui : ObjectGui {
	writeName {}
}

NumberEditorGui : EditorGui {

	var numv,slv;
	
	smallGui { arg layout;
		var l;
		l=this.guify(layout);
		this.box(l,Rect(0,0,40,GUI.skin.buttonHeight));
		if(layout.isNil,{ l.front });
	}
	guiBody { arg layout,slider=true;
		var bounds,h;
		bounds = layout.indentedRemaining;
		// massive space,
			// box, slider horz
		if(bounds.width >= 140 and: {bounds.height >= GUI.skin.buttonHeight},{
			this.box(layout,Rect(0,0,40,GUI.skin.buttonHeight));
			this.slider(layout,Rect(0,0,100,GUI.skin.buttonHeight));
			^this
		});
		// width < height
			// go vert
		if(bounds.width < bounds.height,{
			// height > 100
				// box, slider
			if(bounds.height > 100 and: {bounds.width >= 30},{
				layout.comp({ |l|
					var y;
					this.box(l,Rect(0,0,y = min(40,bounds.width),GUI.skin.buttonHeight));
					this.slider(l,Rect(0,y,min(30,bounds.width),h-GUI.skin.buttonHeight));
				},Rect(0,0,40,h = bounds.height.max(130)))
				^this
			});
			if(bounds.height > 100 ,{
				this.slider(layout,Rect(0,0,min(40,bounds.width),bounds.height.min(150)));
				^this
			});
			// height < 100, > 30
				// slider
			if(bounds.height >= 30,{
				this.slider(layout,Rect(0,0,min(40,bounds.width),bounds.height));
				^this
			});

			// height < 30
				// box
			if(bounds.height <= 30,{
				this.box(layout,Rect(0,0,min(40,bounds.width),bounds.height));
				^this
			});

		},{// height > width

			// width > 100
				// box, slider

			// width < 100, > 30
				// slider
			if(bounds.width.inclusivelyBetween(30,100),{
				this.slider(layout,Rect(0,0,bounds.width,GUI.skin.buttonHeight));
				^this
			});

			// width < 30
			if(bounds.width <= 30,{
				// box
				this.box(layout,Rect(0,0,bounds.width,bounds.height.max(GUI.skin.buttonHeight)));
			});
		});
		
/*
unmatched bounds
Instance of Rect {    (1744D760, gc=C0, fmt=00, flg=00, set=02)
  instance variables [4]
    left : Integer 2
    top : Integer 0
    width : Float 146.0   40624000 00000000
    height : Float 13.0   402A0000 00000000
}*/
		bounds.die("unmatched bounds");
	}
	box { arg layout,bounds;
		var r;
		numv = GUI.numberBox.new(layout,bounds)
			.object_(model.poll)
			.focusColor_(Color.yellow(1.0,0.2))
			.action_({ arg nb;
				model.activeValue_(nb.value).changed(numv);
			});
		//if(consumeKeyDowns,{ 
		//	numv.keyDownAction = {nil};		
		//});
	}
	slider { arg layout, bounds;// x=100,y=15;
		var r;
		slv = GUI.slider.new(layout, bounds);
		slv.focusColor_(Color.yellow(1.0,0.2));
		slv.setProperty(\value,model.spec.unmap(model.poll));
		slv.action_({arg th; 
			model.activeValue_(model.spec.map(th.value)).changed(slv)
		});
		if(consumeKeyDowns,{ slv.keyDownAction = {nil}; });
	}
	update {arg changed,changer; // always has a number box
		{
			if(changer !== numv and: {numv.notNil},{
				numv.value_(model.poll);
			});
			if(changer !== slv and: {slv.notNil},{
				slv.value_(model.spec.unmap(model.poll));
			});
			nil
		}.defer;
	}
	background { ^Color(0.0,0.2,0.2,0.3) }
}

KrNumberEditorGui : NumberEditorGui {
	background { ^Color(0.0,0.3,0.0,0.3) }
}


PopUpEditorGui : EditorGui {	
	var popV;
	
	guiBody { arg layout;
		var horSize;
		horSize = model.labels.maxValue({arg item; item.size }) * 12;
		popV = GUI.popUpMenu.new(layout,Rect(0,0,horSize,15))
			.items_(model.labels)
			.action_({ arg nb;
				model.selectByIndex(popV.value).changed(this)
			});
		if(consumeKeyDowns,{ popV.keyDownAction = {nil}; });
		popV.setProperty(\value,model.selectedIndex)
	}
	update { arg changed,changer;
		if(changer !== this,{
			popV.setProperty(\value,model.selectedIndex)
		});
	}	
}



BooleanEditorGui : EditorGui {
	var cb;
	guiBody { arg layout;
		var bg;
		bg = layout.background;
		cb = GUI.button.new( layout,Rect(0,0,14,14));
		cb.states = [[" ",bg,bg],["X",Color.black,bg]];
		cb.font = GUI.font.new("Helvetica",9);
		cb.setProperty(\value,model.value.binaryValue);
		cb.action = { model.activeValue_(cb.value != 0,this) };
		if(consumeKeyDowns,{ cb.keyDownAction = {nil}; });
	}
	update { arg changed,changer;
		if(changer !== this,{
			cb.setProperty(\value,model.value.binaryValue);
		});
	}
}

