SCQuartzComposerView : SCView{
	var <path, <inputKeys, <outputKeys;

	start{
		this.setProperty(\start);
	}

	stop{
		this.setProperty(\stop);
	}

	path_{|argpath|
		path = argpath;
		this.setProperty(\loadCompositionFromFile, argpath);
		inputKeys = this.getProperty(\getInputKeys).collect(_.asSymbol);
		outputKeys = this.getProperty(\getOutputKeys).collect(_.asSymbol);
	}

	openInQC{ ("open" + path.quote).unixCmd }

	setInputValue{|key, value|
		this.setProperty(\setInputValue, [key.asSymbol, value]);
	}

	getInputValue{|key|
		^this.getProperty(\getInputValue, key.asSymbol);
	}

	getOutputValue{|key|
		^this.getProperty(\getOutputValue, key.asSymbol);
	}

//	erase{
//		this.setProperty(\erase);
//	}
//
//	eraseColor_{|color|
//		this.setProperty(\eraseColor, color);
//	}

	maxFPS_{|rate|
		this.setProperty(\setMaxRenderingFrameRate, rate);
	}

	// check if the method corresponds to a port key
	// if not call super
	doesNotUnderstand { arg selector ... args;
		if(selector.isSetter && inputKeys.includes(selector.asGetter), {
			this.setInputValue(selector.asGetter, *args); ^this;},
			{
				if(outputKeys.includes(selector), {^this.getOutputValue(selector)}, {
					if(inputKeys.includes(selector), {^this.getInputValue(selector)})
				})
			}
		);
		super.doesNotUnderstand(selector, *args);
	}

}
