

HasSubjectGui : AbstractPlayerGui {
	//smallGui could not gui the subject
	guiBody { arg layout;
		CXLabel(layout,"on:");
		layout.indent(1);
		model.subject.gui(layout);
		layout.indent(-1);
	}
}

KrGui : HasSubjectGui {

//	guiBody { arg layout;
//		CXLabel(layout,"lag:");
//		model.lag.smallGui(layout);
//		super.guiBody(layout);
//	}
}


StreamKrGui : KrGui {

	guiBody { arg layout;
		CXLabel(layout,"durs:");
		model.durations.smallGui(layout);
		super.guiBody(layout);
	}

}


// StreamKrDur can simply inherit
