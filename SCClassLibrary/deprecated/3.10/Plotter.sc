+ Plotter {
	plotColors {
		this.deprecated(thisMethod, this.class.findMethod(\plotColor));
		^this.plotColor;
	}

	plotColors_ { |argColors|
		this.deprecated(thisMethod, this.class.findMethod(\plotColor_));
		^this.plotColor_(argColors);
	}
}
