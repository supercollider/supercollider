


PlayerEfxFuncGui : AbstractPlayerGui {

	guiBody { arg layout;
		
		model.function.gui(layout);
		CXLabel(layout,"on:");
		model.subject.gui(layout);
	}

}


XFaderPlayerGui : AbstractPlayerGui {
	
	guiBody { arg layout;
		CXLabel(layout,"(");
		model.one.gui(layout);
		
		layout.startRow;
		CXLabel(layout,"levelOne xfade levelTwo"); 
		model.pre1.gui(layout);
		model.xfader.gui(layout);
		model.pre2.gui(layout);
		
		layout.startRow;
		model.two.gui(layout);	
		CXLabel(layout,")");
	}

}


DualSeriesEfxGui : XFaderPlayerGui {

	guiBody { arg layout;
	
		layout.startRow;
		CXLabel(layout,"input:");
		model.input.gui(layout);
		
		layout.startRow;
		CXLabel(layout,"preOne:"); 
		model.pre1.gui(layout);
		
		layout.startRow;
		CXLabel(layout,"efx1:"); 
		model.efx1.gui(layout);

		layout.startRow;
		CXLabel(layout,"xfader1:");
		model.xfader1.gui(layout);
		
		layout.startRow;
		CXLabel(layout,"pre2:");		
		model.pre2.gui(layout);

		layout.startRow;
		CXLabel(layout,"efx2:");
		model.efx2.gui(layout);
		
		layout.startRow;
		CXLabel(layout,"xfader2:"); 
		model.xfader2.gui(layout);

		layout.startRow;
		CXLabel(layout,"post:"); 
		model.post.gui(layout);
		
	}

}
