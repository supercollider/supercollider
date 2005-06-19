
EditorGui : ObjectGui {
	writeName {}
}

NumberEditorGui : EditorGui {

	var numv,slv;
	
	smallGui { arg layout;
		var l;
		l=this.guify(layout);
		this.box(l);
		if(layout.isNil,{ l.front });
	}
	guiBody { arg layout,slider=true;
		this.box(layout);
		if(slider,{
			this.slider(layout);
		});
	}
	box { arg layout;
		var r;
		numv = SCNumberBox(layout,Rect(0,0,40,17))
			.object_(model.poll)
			//.stringColor_(this.background)
			.action_({ arg nb;
				model.activeValue_(nb.value).changed(numv);
			});
		if(consumeKeyDowns,{ numv.keyDownAction = {nil};	});
	}
	slider { arg layout, x=100,y=15;
		var r;
		slv = SCSlider(layout, Rect(0,0,100,17));
		slv.setProperty(\value,model.spec.unmap(model.poll));
		//slv.background_(this.background);
		slv.action_({arg th; 
			model.activeValue_(model.spec.map(th.value)).changed(slv)
		});
		if(consumeKeyDowns,{ slv.keyDownAction = {nil}; });
	}
	update {arg changed,changer; // always has a number box
		{
			if(changer !== numv,{
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
		popV = SCPopUpMenu(layout,Rect(0,0,horSize,15))
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
		cb = SCButton.new( layout,Rect(0,0,14,14));
		cb.states = [[" ",bg,bg],["X",Color.black,bg]];
		cb.font = Font("Helvetica",9);
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

