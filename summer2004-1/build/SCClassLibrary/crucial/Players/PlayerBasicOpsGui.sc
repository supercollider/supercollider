

PlayerUnopGui : AbstractPlayerGui {

	guiBody {  arg layout;
		CXLabel(layout,"(");
		model.operator.gui(layout);
		model.a.gui(layout);
		CXLabel(layout,")");
	}
}


PlayerBinopGui : AbstractPlayerGui {

	guiBody { arg layout;
		CXLabel(layout,"(");
		model.a.gui(layout);
		model.operator.gui(layout);
		model.b.gui(layout);
		CXLabel(layout,")");
	}

}
