QuartzComposerView : View {
	var <path, <inputKeys, <outputKeys;

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

	setInputValue{|key, value|
		_QQuartzComposer_SetInputPort
		^this.primitiveFailed;
	}

	getInputValue{|key|
		_QQuartzComposer_GetInputPort
		^this.primitiveFailed;
	}

	getOutputValue{|key|
		_QQuartzComposer_GetOutputPort
		^this.primitiveFailed;
	}

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
