
//NodeGui : ObjectGui {
//
//	guiBody { arg layout;
//		Tile(model.server,layout);
//		Tile(model.group,layout);
//		ActionButton(layout,"trace",{
//			model.trace;
//		});
//		ActionButton(layout,"query",{
//			model.query;
//		});
//	}
//}
//
//SynthGui : NodeGui {
//
//	guiBody { arg layout;
//		CXLabel(layout,model.defName);
//		super.guiBody(layout);
//	}
//}