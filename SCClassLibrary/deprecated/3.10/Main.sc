+ Main {
	*scVersionPostfix {
		this.deprecated(thisMethod, this.class.findMethod(\scVersionPatch));
		^"." ++ this.scVersionPatch ++ this.scVersionTweak;
	}
}
