

PlayerUnopGui : AbstractPlayerGui {

	guiBody {  arg layout;
		Label(layout,"(");
		model.operator.gui(layout);
		model.a.gui(layout);
		Label(layout,")");
	}
}


PlayerBinopGui : AbstractPlayerGui {

	guiBody { arg layout;
		Label(layout,"(");
		model.a.gui(layout);
		model.operator.gui(layout);
		model.b.gui(layout);
		Label(layout,")");
	}

}
