QuartzComposerView : View {
	var <path, <inputKeys, <outputKeys;

	*new {|...args|
		// QuartzComposer is not available in Qt6 and thus will be removed
		this.deprecated(thisMethod);
		super.new(*args)
	}

	*qtClass { ^'QcQuartzComposerView' }

	start{
		this.invokeMethod(\startRendering)
	}

	stop{
		this.setProperty(\stopRendering);
	}

	path_{|argpath|
		path = argpath;
		this.invokeMethod(\loadCompositionFromFile, path);
		inputKeys = this.getProperty(\inputKeys).collect(_.asSymbol);
		outputKeys = this.getProperty(\outputKeys).collect(_.asSymbol);
	}

	openInQC{ ("open" + path.quote + "-a 'Quartz Composer'").unixCmd }

	maxFPS_{|rate|
		this.invokeMethod(\setMaxRenderingFrameRate, rate.asFloat);
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
