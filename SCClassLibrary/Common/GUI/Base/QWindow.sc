TopScrollWidget : ScrollCanvas {
	var <>win;
	doDrawFunc { win.drawFunc.value(win) }
}

ScrollTopView : ScrollView {
	var >window;

	*qtClass { ^'QcScrollWindow' }

	*new { |win, name, bounds, resizable, border|
		^super.newCustom([name, bounds, resizable, border])
		.initScrollTopView(win)
	}

	initScrollTopView { |win|
		var cnv;
		window = win;
		// NOTE: The canvas widget must not be a View, so that asking its
		// children for parent will skip it and hit this view instead.
		cnv = TopScrollWidget.new;
		cnv.win = win;
		this.canvas = cnv
	}

	bounds {
		var r;
		r = this.getProperty(\geometry);
		^r.moveTo(0, 0)
	}

	bounds_ { |rect|
		var rNew = rect.asRect;
		var rOld = this.getProperty(\geometry);
		this.setProperty(\geometry, rOld.resizeTo(rNew.width, rNew.height))
	}

	drawingEnabled_ { |bool| canvas.setProperty(\drawingEnabled, bool) }

	findWindow { ^window }
}

TopView : View {
	var >window;

	*qtClass { ^'QcWindow' }

	*new { |win, name, bounds, resizable, border|
		^super.newCustom([name, bounds, resizable, border])
		.initTopView(win)
	}

	initTopView { |win| window = win }

	bounds {
		var r;
		r = this.getProperty(\geometry);
		^r.moveTo(0, 0)
	}

	bounds_ { |rect|
		var rNew = rect.asRect;
		var rOld = this.getProperty(\geometry);
		this.setProperty(\geometry, rOld.resizeTo(rNew.width, rNew.height))
	}

	drawingEnabled_ { |bool| this.setProperty(\drawingEnabled, bool) }

	findWindow { ^window }

	doDrawFunc { window.drawFunc.value(window) }
}

Window {
	classvar <allWindows, <>initAction;

	var resizable, <drawFunc;
	var <view;

	//TODO
	var <>acceptsClickThrough = false;
	var <currentSheet;

	*screenBounds {
		_QWindow_ScreenBounds
		^this.primitiveFailed
	}

	*availableBounds {
		_QWindow_AvailableGeometry
		^this.primitiveFailed
	}

	*closeAll {
		allWindows.copy.do { |win| win.close }
	}

	*lowestPosition {
		^this.screenBounds.bottom - this.availableBounds.bottom
	}

	/* NOTE:
	- all args have to be of correct type for Widget constructor to match!
	*/
	*new { |name = "",
			bounds,
			resizable = true,
			border = true,
			deprecatedServerArgument,
			scroll = false|

		if(deprecatedServerArgument.notNil) {
			// see https://github.com/supercollider/supercollider/pull/5312#discussion_r553546303
			"this argument is deprecated, it was kept for SwingOSC compatibility, but will be removed".warn
		};

		if(bounds.isNil) {
			bounds = Rect(0, 0, 400, 400).center_(Window.availableBounds.center)
		} {
			bounds = Window.flipY(bounds.asRect)
		};
		^super.new.initWindow(name, bounds, resizable, border, scroll)
	}

	//------------------------ Window specific  -----------------------//

	initWindow { |name, bounds, resize, border, scroll|
		if(scroll)
			{ view = ScrollTopView.new(this, name, bounds, resize, border) }
			{ view = TopView.new(this, name, bounds, resize, border) };

		// set some necessary object vars
		resizable = resize == true;

		// allWindows array management
		Window.addWindow(this);
		view.connectFunction('destroyed()', { Window.removeWindow(this) }, false);

		// action to call whenever a window is created
		Window.initAction.value(this)
	}

	asView {
		^view
	}

	bounds_ { |aRect|
		var r = Window.flipY(aRect.asRect);
		view.setProperty(\geometry, r);
		if(resizable.not) { view.fixedSize = r.size }
	}

	bounds {
		^Window.flipY(view.getProperty(\geometry))
	}

	setInnerExtent { |w, h|
		// bypass this.bounds, to avoid Window flipping the y coordinate
		var r = view.getProperty(\geometry);
		view.setProperty(\geometry, r.resizeTo(w, h);)
	}

	background { ^view.background }

	background_ { |aColor| view.background = aColor }

	drawFunc_ { |aFunction|
		view.drawingEnabled = aFunction.notNil;
		drawFunc = aFunction
	}

	setTopLeftBounds{ |rect, menuSpacer = 45|
		view.setProperty(\geometry, rect.moveBy(0, menuSpacer))
	}

	onClose { ^view.onClose }
	onClose_ { |func| view.onClose_(func) }

	moveToBottom { |margin = 0|
		this.bounds = this.bounds.moveTo(
			this.bounds.left,
			Window.lowestPosition + margin
		)
	}

	// TODO
	addToOnClose{ |function| }
	removeFromOnClose{ |function| }

	//------------------- simply redirected to View ---------------------//

	sizeHint { ^view.sizeHint }
	minSizeHint { ^view.minSizeHint }
	resizeToHint { ^view.resizeToHint }
	alpha_ { |value| view.alpha_(value) }
	addFlowLayout { |margin, gap| ^view.addFlowLayout(margin, gap) }
	close { view.close }
	isClosed { ^view.isClosed }
	visible { ^view.visible }
	visible_ { |boolean| view.visible_(boolean) }
	front { view.front }
	fullScreen { view.fullScreen }
	endFullScreen { view.endFullScreen }
	minimize { view.minimize }
	unminimize { view.unminimize }
	name { ^view.name }
	name_ { |string| view.name_(string) }
	refresh { view.refresh }
	userCanClose { ^view.userCanClose }
	userCanClose_ { |boolean| view.userCanClose_(boolean) }
	alwaysOnTop { ^view.alwaysOnTop }
	alwaysOnTop_ { |boolean| view.alwaysOnTop_(boolean) }
	layout { ^view.layout }
	layout_ { |layout| view.layout_(layout) }
	toFrontAction_ { |action| view.toFrontAction_(action) }
	toFrontAction { ^view.toFrontAction }
	endFrontAction_ { |action| view.endFrontAction_(action) }
	endFrontAction { ^view.endFrontAction }
	acceptsMouseOver { ^view.acceptsMouseOver }
	acceptsMouseOver_ { |flag| view.acceptsMouseOver_(flag) }

	// ---------------------- private ------------------------------------

	*flipY { |aRect|
		var flippedTop = Window.screenBounds.height - aRect.top - aRect.height;
		^Rect(aRect.left, flippedTop, aRect.width, aRect.height)
	}

	*addWindow { |window|
		allWindows = allWindows.add(window)
	}

	*removeWindow { |window|
		allWindows.remove(window)
	}
}
