
PatchGui : AbstractPlayerGui {

	guiBody { arg layout;
		Tile(this.model.instr,layout);
		model.args.do({ arg a,i;
			layout.startRow;
			ArgNameLabel(model.instr.argNames.at(i),layout);
			// will be replaced with some kind of minimizing/droppable display
			// InputGui
			layout.flow({ arg layout;
				if(a.tryPerform('path').notNil,{
					Tile(a,layout);
				},{
					a.gui(layout)
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