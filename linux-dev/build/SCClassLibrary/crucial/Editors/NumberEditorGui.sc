
EditorGui : ObjectGui {
	writeName {}
}

NumberEditorGui : EditorGui {

	var numv;
	
	guiBody { arg layout,slider=true;
		layout=this.guify(layout);
		this.box(layout);
		if(slider,{
			this.slider(layout);
		});
	}
	smallGui { arg layout ... args;
		var l;
		l=this.guify(layout);
		this.box(l);
		if(layout.isNil,{ l.front });
	}
	box { arg layout;
		var r;
		layout=this.guify(layout);
		//r = layout.layRight(40,17);
		numv = SCNumberBox(layout,Rect(0,0,40,17))
			.object_(model.poll)
			.action_({ arg nb;
				model.activeValue_(nb.value).changed(numv);
			});		
	}
	update {arg changed,changer; // always has a number box
		if(changer !== numv,{
			numv.value_(model.poll);
		})
	}
	
	slider { arg layout, x=100,y=15;
		var slv,r;
		//r = layout.layRight(x,y);
		slv = SCSlider(layout, Rect(0,0,100,15));
		slv.setProperty(\value,model.spec.unmap(model.poll));
		slv.action_({arg th; 
			model.activeValue_(model.spec.map(th.value)).changed(slv)
		});		
			
		layout.removeOnClose(	
			Updater(model,{ arg changed,changer;
				// without triggering action
				if(changer !== slv,{
					slv.value_(model.spec.unmap(model.poll))
				});
			});
		);
	}
}



BooleanEditorGui : EditorGui {
	var cb;
	guiBody { arg layout;
		cb = SCButton.new( layout,Rect(0,0,14,14));
		cb.states = [[" ",Color.black,Color.white],[" ",Color.white,Color.black]];
		cb.font = Font("Helvetica",9);
		cb.setProperty(\value,model.value.binaryValue);
		cb.action = { model.activeValue_(cb.value != 0,this) };
	}
	update { arg changed,changer;
		if(changer !== this,{
			cb.setProperty(\value,model.value.binaryValue);
		});
	}
}

