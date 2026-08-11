LevelIndicator : View {

	*qtClass {^'QcLevelIndicator'}

	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}
	valueAction_ { arg val;
		this.setProperty(\value, val);
		this.doAction;
	}

	warning_ {arg val;
		this.setProperty(\warning, val);
	}

	critical_ {arg val;
		this.setProperty(\critical, val);
	}

	style_ {arg val;
		this.setProperty(\style, QLevelIndicatorStyle(val));
	}

	stepWidth_{arg val;
		this.setProperty(\stepWidth, val);
	}

	background { ^this.getProperty(\grooveColor) }
	background_ { arg color; this.setProperty(\grooveColor, color) }

	meterColor_{ |color| this.setProperty(\meterColor, color) }
	warningColor_{ |color| this.setProperty(\warningColor, color) }
	criticalColor_{ |color| this.setProperty(\criticalColor, color) }

	numSteps_ {arg val;
		var stepWidth, length = max(this.bounds.width, this.bounds.height);
		stepWidth = length / val;
		stepWidth = stepWidth - (stepWidth < 3).if(1, 2);
		this.stepWidth = stepWidth;
	}

	image_ {arg image;
		this.nonimpl("image");
	}

	numTicks_ {arg number;
		this.setProperty(\ticks, number);
	}

	numMajorTicks_ {arg number;
		this.setProperty(\majorTicks, number);
	}

	drawsPeak_ {arg bool;
		this.setProperty(\drawPeak, bool);
	}

	peakLevel_ { arg val;
		this.setProperty(\peak, val);
	}
}
