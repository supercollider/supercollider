+ LinuxPlatform {
	recompile {
		if ( this.hasFeature( \emacs ) ) {
			Emacs.evalLispExpression( "(sclang-recompile)" )
		} {
			this.prRecompile
		}
	}

	prRecompile {
		_Recompile
	}
}
