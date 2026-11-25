UserView : View {
	var <drawFunc, <drawingEnabled = true, <animate = false;

	*qtClass { ^'QcCustomPainted' }

	*new { |parent, bounds|
		var me = super.new(parent, bounds ?? { this.sizeHint });
		me.canFocus = true;
		^me
	}

	*sizeHint {
		^Point(150, 150)
	}

	drawingEnabled_ { |boolean|
		// Allow setting the property apart from the instance variable
		// to optimize when drawFunc is nil. See drawFunc_ implementation.
		drawingEnabled = boolean;
		this.setProperty(\drawingEnabled, boolean)
	}

	clearOnRefresh { ^this.getProperty(\clearOnRefresh) }
	clearOnRefresh_ { |boolean| this.setProperty(\clearOnRefresh, boolean) }

	clearDrawing { this.invokeMethod(\clear) }

	drawFunc_ { |aFunction|
		this.setProperty(\drawingEnabled, aFunction.notNil);
		drawFunc = aFunction
	}

	draw {
		// NOTE: it is only allowed to call this while a QPaintEvent is being
		// processed by this QWidget, or an error will be thrown.
		drawFunc.value(this)
	}

	animate_ { |bool| this.invokeMethod(\animate, bool); animate = bool }

	frameRate_ { |fps| this.setProperty(\frameRate, fps.asFloat) }
	frameRate { ^this.getProperty(\frameRate) }

	frame { ^this.getProperty(\frameCount) }

	// override View's action_ to not connect to 'action()' signal
	action_ { |func|
		action = func
	}

	doDrawFunc { drawFunc.value(this) }
}
