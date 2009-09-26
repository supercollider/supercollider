
PatchGui : AbstractPlayerGui {

	writeName { arg layout;
		//color it based on whether it has a .path
						// (was loaded from disk)
		this.prWriteName(layout,model.class.asString);
		if(model.path.notNil,{
			ActionButton(layout,"edit file",{
				model.path.openTextFile;
			});
		});
	}
	guiBody { arg layout;
		var bounds, maxHeight,vl,font,argNameWidth;
		bounds = layout.bounds;
		maxHeight = bounds.height - 20 - (model.args.size * 15) / model.args.size;

		this.instrGui(layout);

		font = GUI.font.new("Helvetica",10);
		//vl = SCVLayoutView(layout.startRow,layout.decorator.indentedRemaining);
		vl = layout;
		argNameWidth = model.instr.argNames.maxValue({ |an| an.asString.bounds(font).width + 7 });
		argNameWidth = min(100,argNameWidth ? 100);
		model.args.do({ arg a,i;
			var gui,disclosed=true,box;
			layout.startRow;
			// ArgNameLabel(model.instr.argNames.at(i),layout);
			GUI.dragSink.new(vl,Rect(0,0,argNameWidth,15))
				.background_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
				.font_(font)
				.align_(\left)
				.canReceiveDragHandler_({
					model.instr.specs.at(i).canAccept(GUI.view.currentDrag);
				})
				.object_(" " ++ model.instr.argNames.at(i))
				.action_({ arg sink;
					// assumes to copy the object
					model.setInput(i,sink.object.copy);
					sink.object = model.instr.argNames.at(i); // don't change the name
					if(gui.notNil,{
						gui.remove(true);
						// expand the box
						// layout.bounds = layout.bounds.resizeTo(1000,1000);
						box.bounds = box.bounds.resizeTo(900,900);
						gui = model.args.at(i).gui(box);
						box.resizeToFit(true,true);
						// layout.reflowAll;
					});
				});

			box = vl.flow({ arg layout;
				if(a.tryPerform('path').notNil,{
					Tile(a,layout);
				},{
					gui = a.gui(layout);
				});
			})
		});
	}
	instrGui { arg layout;
		//Tile(this.model.instr,layout);
		ActionButton(layout,this.model.instr.dotNotation,{
			this.model.instr.gui
		})
	}
}

InstrSpawnerGui : PatchGui {
	guiBody { arg layout;
		super.guiBody(layout);
		layout.startRow;
		CXLabel(layout,"delta pattern:");
		model.deltaPattern.gui(layout);
	}
}

InterfaceGui : AbstractPlayerGui {
	guiBody { arg ... args;
		model.performList(\guiBody,args);
	}
	writeName {}
}


