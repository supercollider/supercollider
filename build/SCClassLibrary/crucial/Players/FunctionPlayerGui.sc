


PlayerEfxFuncGui : AbstractPlayerGui {

	guiBody { arg layout;
		
		model.function.gui(layout);
		Label(layout,"on:");
		model.subject.gui(layout);
	}

}


XFaderPlayerGui : AbstractPlayerGui {
	
	guiBody { arg layout;
		Label(layout,"(");
		model.one.gui(layout);
		
		layout.startRow;
		Label(layout,"levelOne xfade levelTwo"); 
		model.pre1.gui(layout);
		model.xfader.gui(layout);
		model.pre2.gui(layout);
		
		layout.startRow;
		model.two.gui(layout);	
		Label(layout,")");
	}

}


DualSeriesEfxGui : XFaderPlayerGui {

	guiBody { arg layout;
	
		layout.startRow;
		Label(layout,"input:");
		model.input.gui(layout);
		
		layout.startRow;
		Label(layout,"preOne:"); 
		model.pre1.gui(layout);
		
		layout.startRow;
		Label(layout,"efx1:"); 
		model.efx1.gui(layout);

		layout.startRow;
		Label(layout,"xfader1:");
		model.xfader1.gui(layout);
		
		layout.startRow;
		Label(layout,"pre2:");		
		model.pre2.gui(layout);

		layout.startRow;
		Label(layout,"efx2:");
		model.efx2.gui(layout);
		
		layout.startRow;
		Label(layout,"xfader2:"); 
		model.xfader2.gui(layout);

		layout.startRow;
		Label(layout,"post:"); 
		model.post.gui(layout);
		
	}

}
