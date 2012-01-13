+ SCWindow {
		// deprecation
	drawHook {
		this.deprecated(thisMethod, this.class.findMethod(\drawFunc));
		^drawFunc
	}
	drawHook_ { |aFunction|
		this.deprecated(thisMethod, this.class.findMethod(\drawFunc_));
		this.drawFunc_(aFunction)
	}
}