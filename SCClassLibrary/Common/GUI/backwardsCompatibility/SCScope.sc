SCScope : View {
	var <bufnum, <style = 0, <xZoom = 1.0, <yZoom = 1.0, <x = 0.0, <y = 0.0;
	var <waveColors;

	*qtClass { ^'QcScope' }

	bufnum_ { |anInt|
		bufnum = anInt;
		this.setProperty(\bufferNumber, anInt)
	}

	style_ { |anInt|
		style = anInt;
		this.setProperty(\style, anInt)
	}

	xZoom_ { |aFloat|
		xZoom = aFloat;
		this.setProperty(\xZoom, aFloat)
	}

	yZoom_ { |aFloat|
		yZoom = aFloat;
		this.setProperty(\yZoom, aFloat)
	}

	x_ { |aFloat|
		x = aFloat;
		this.setProperty(\xOffset, aFloat)
	}

	y_ { |aFloat|
		y = aFloat;
		this.setProperty(\yOffset, aFloat)
	}

	waveColors_ { |aColorArray|
		waveColors = aColorArray;
		this.setProperty(\waveColors, aColorArray)
	}
}
