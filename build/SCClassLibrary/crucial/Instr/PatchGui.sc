
PatchGui : AbstractPlayerGui {

//	writeName { arg layout,big=false; 
//		//layout.startRow;
//		super.writeName(layout,big)
//	}

	guiBody { arg layout;
		Tile(this.model.instr,layout);
		//this.durationGui(layout);// usually 'inf' meaning 'endless'
		//layout.indent(1);
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
		//layout.indent(-1);
	}

}
 
