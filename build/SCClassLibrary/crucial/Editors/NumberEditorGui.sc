
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
		layout=this.guify(layout);
		this.box(layout);
	}
	box { arg layout;
		var r;
		layout=this.guify(layout);
		r = layout.layRight(40,17);
		numv = SCNumberBox(layout,r)
			.object_(model.poll)
			.action_({ arg nb;
				model.activeValue_(nb.value);
			});		
	}
	update {arg changed,changer; // always has a number box
		//numv.setProperty(\value,model.poll)
		numv.value_(model.poll);
	}
	
	slider { arg layout, x=100,y=15;
		var slv,r;
		r = layout.layRight(x,y);
		slv = SCSlider(layout, r);
		slv.setProperty(\value,model.spec.unmap(model.poll));
		slv.action_({arg th; 
			model.activeValue_(model.spec.map(th.value))
		});		
			
		layout.removeOnClose(	
			Updater(model,{ arg changed,changer;
				// without triggering action
				slv.value_(model.spec.unmap(model.poll))
			});//.removeOnClose(layout);
		);
	}
}

/*
	extra update of slider/box happening
*/



BooleanEditorGui : EditorGui {
	var cb;
	guiBody { arg layout;
		var r;
		r = layout.layRight(14,14);
		cb = SCButton.new( layout.window,r);
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

