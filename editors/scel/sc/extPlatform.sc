+ Platform {
	recompile{
		if ( this.hasFeature( \emacs ) ) {
			Emacs.evalLispExpression( "(sclang-start)" );
		};
	}
}