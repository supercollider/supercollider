
PatchGui : AbstractPlayerGui {

	guiBody { arg layout;

		layout.startRow;
		
		Tile(this.model.instr,layout);

		this.durationGui(layout);// usually 'inf' meaning 'endless'
		layout.indent(1);
		model.args.do({ arg a,i;
			layout.startRow;
			ArgNameLabel(i.asString + model.instr.argNames.at(i),layout);
			// will be replaced with some kind of minimizing display
			if(a.tryPerform('path').notNil,{
				Tile(a,layout);
			},{
				a.gui(layout)
			});
		});
		layout.indent(-1);
	}

}
 
