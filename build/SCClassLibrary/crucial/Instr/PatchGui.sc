
PatchGui : AbstractPlayerGui {
	
	guiBody { arg layout;
		Tile(this.model.instr,layout);
		model.args.do({ arg a,i;
			var gui,disclosed=true,box;
			layout.startRow;
			//ArgNameLabel(model.instr.argNames.at(i),layout);
			SCDragSink(layout,Rect(0,0,100,15))
				.background_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
				.font_(Font("Helvetica",10))
				.align_(\left)
				.acceptDrag_({  
					model.instr.specs.at(i).canAccept(SCView.currentDrag);
				})
				.object_(model.instr.argNames.at(i))
				.action_({ arg sink;
					// assumes to copy the object
					model.setInput(i,sink.object.copy);
					sink.object = model.instr.argNames.at(i); // don't change the name
					if(gui.notNil,{
						gui.remove(true);
						// expand the box
						box.bounds = box.bounds.resizeTo(600,600);
						gui = model.args.at(i).gui(box);
						box.resizeToFit;
						layout.reflowAll;
					});
				});

			box = layout.flow({ arg layout;
				if(a.tryPerform('path').notNil,{
					Tile(a,layout);
				},{
					gui = a.gui(layout);/*
					ToggleButton(layout,"³",
						{  gui = model.args.at(i).gui(box); },
						{  gui.remove(true); box.refresh;  },
						true
					);
					layout.startRow;
					box = layout.flow({ arg box;
							gui = model.args.at(i).gui(box);
						})*/
				});
			})
		});
	}
}
 
InstrSpawnerGui : PatchGui {

	guiBody { arg layout;
		super.guiBody(layout);
		layout.startRow;
		CXLabel(layout,"delta:");
		model.delta.gui(layout);
	}
}