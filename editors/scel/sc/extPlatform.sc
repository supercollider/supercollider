+ Platform {
	recompile{
		if ( this.hasFeature( \emacs ) ) {
			Emacs.evalLispExpression( "(sclang-start)" );
		};
	}
}

+ LinuxPlatform {
	recompile{
		if ( this.hasFeature( \emacs ) ) {
			Emacs.evalLispExpression( "(sclang-recompile)" );
		};
	}
}
