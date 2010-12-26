SCScope : SCView {
	bufnum {
		^this.getProperty(\bufnum)
	}
	bufnum_ { arg num;
		this.setProperty(\bufnum, num);
	}
	x {
		^this.getProperty(\x)
	}
	x_ { arg val;
		this.setProperty(\x, val);
	}
	y {
		^this.getProperty(\y)
	}
	y_ { arg val;
		this.setProperty(\y, val);
	}
	xZoom {
		^this.getProperty(\xZoom)
	}
	xZoom_ { arg val;
		this.setProperty(\xZoom, val);
	}
	yZoom {
		^this.getProperty(\yZoom)
	}
	yZoom_ { arg val;
		this.setProperty(\yZoom, val);
	}

	gridColor {
		^this.getProperty(\gridColor, Color.new)
	}
	gridColor_ { arg color;
		this.setProperty(\gridColor, color);
	}

	waveColors {
		^this.getProperty(\waveColors, Array.fill(128, {Color.new})) // 128 is kMaxScopeChannels
	}
	waveColors_ { arg arrayOfColors;
		this.setProperty(\waveColors, arrayOfColors);
	}

	style_ { arg val;
		this.setProperty(\style, val);
		// 0 = vertically spaced
		// 1 = overlapped
		// 2 = x/y
	}

	properties {
		^super.properties ++ #[\bufnum, \x, \y, \xZoom, \yZoom, \gridColor, \waveColors, \style]
	}
}
