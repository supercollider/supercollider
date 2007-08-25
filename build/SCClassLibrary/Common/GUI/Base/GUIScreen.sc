
SCWindow {
	classvar <>allWindows;
	
	var dataptr, <name, <>onClose, <view, <userCanClose=true;
	var <alwaysOnTop=false;
	var <>drawHook;
	var <acceptsMouseOver=false;
	var <isClosed = false;
	var <acceptsClickThrough = true;
	
	*initClass {
		UI.registerForShutdown({ this.closeAll });
	}

	*new { arg name = "panel", bounds, resizable = true, border = true, server, scroll = false;
		^super.new.initSCWindow(name, bounds, resizable, border, scroll)
	}
	initSCWindow { arg argName, argBounds, resizable, border, scroll;
		name = argName.asString;
		argBounds = argBounds ?? {Rect(128, 64, 400, 400)};
		allWindows = allWindows.add(this);
		scroll.if({
			view = SCScrollTopView(nil, argBounds.moveTo(0,0));
		},{
			view = SCTopView(nil, argBounds.moveTo(0,0));
		});
		this.prInit(name, argBounds, resizable, border, scroll, view);
	}

	asView { ^view }
	add { arg aView; view.add(aView) }
	
	*closeAll {
		var list;
		list = allWindows.copy;
		allWindows = Array.new(8);
		list.do({ arg window; window.close; });
	}
		
	close {
		if (isClosed) { ^this };
		this.prClose;
	}
	closed {
		isClosed = true;
		onClose.value; // call user function
		dataptr = nil;
		view.prClose;
		allWindows.remove(this);
	}

	fullScreen {
		_SCWindow_BeginFullScreen
		^this.primitiveFailed
	}
	endFullScreen {
		_SCWindow_EndFullScreen
		^this.primitiveFailed
	}
	userCanClose_ { arg boo;
		_SCWindow_SetShouldClose
		^this.primitiveFailed
	}
	acceptsMouseOver_{arg bool;
		acceptsMouseOver = bool;	
		this.prSetAcceptMouseOver(bool);
	}	
	front {
		_SCWindow_ToFront
		^this.primitiveFailed
	}
	
	alwaysOnTop_{|bool=true|
		alwaysOnTop = bool;
		this.prSetAlwaysOnTop(bool);	
	}
	
	prSetAlwaysOnTop{|boolean=true|
		_SCWindow_AlwaysOnTop	
	}
	
	acceptsClickThrough_{|boolean=true|
		acceptsClickThrough = boolean;
		this.prSetAcceptsClickThrough(boolean);	
	}
	
	prSetAcceptsClickThrough{|boolean=true|
		_SCWindow_AcceptsClickThrough	
	}
	
	refresh {
		_SCWindow_Refresh
		^this.primitiveFailed
	}
	minimize {
		_SCWindow_Minimize
		^this.primitiveFailed
	}
	alpha_ { arg alpha;
		_SCWindow_SetAlpha
		^this.primitiveFailed
	}
	
	name_ { arg argName;
		name = argName;
		this.prSetName(argName);
	}
	bounds_ { arg argBounds;
		this.prSetBounds(argBounds);
	}
	setInnerExtent { arg w,h; // resize window keeping top left corner fixed
		var b;
		b = this.bounds;
		w = w ? b.width;
		h = h ? b.height;
		this.bounds = Rect(b.left,b.top + (b.height - h), w,h);
	}
	bounds {
		^this.prGetBounds(Rect.new);
	}
	*screenBounds {
		^this.prGetScreenBounds(Rect.new);
	}
	play { arg function;
		AppClock.play({ 
			if (dataptr.notNil, {
				function.value;
			});
		});
		
	}
	
	findByID { arg id;
		^view.findByID(id)
	}
			
	// PRIVATE
	// primitives
	prInit { arg argName, argBounds, resizable, border, scroll;
		_SCWindow_New
		^this.primitiveFailed
	}
	prClose {
		_SCWindow_Close
		^this.primitiveFailed
	}
	prSetName { arg argName;
		_SCWindow_SetName
		^this.primitiveFailed
	}
	prGetBounds { arg argBounds;
		_SCWindow_GetBounds
		^this.primitiveFailed
	}
	prSetBounds { arg argBounds;
		_SCWindow_SetBounds
		^this.primitiveFailed
	}
	prSetAcceptMouseOver{arg bool;
		_SCWindow_SetAcceptMouseOver
		^this.primitiveFailed		
	}
	*prGetScreenBounds { arg argBounds;
		_SCWindow_GetScreenBounds
		^this.primitiveFailed
	}
	callDrawHook {
		drawHook.value(this);
	}
	
	
	*viewPalette {
		var w, v, f, c;
		w = SCWindow("View Palette", Rect(532, 64, 300, 320)).front;
		w.view.decorator = f = FlowLayout(w.view.bounds);
		
		c = [SCSlider, SCRangeSlider, SC2DSlider, SCPopUpMenu, SCButton, 
			SCNumberBox, SCMultiSliderView,
			SCStaticText, SCDragSource, SCDragSink, SCDragBoth,
		];
		
		c.do({ arg item;
			var n;
			n = SCDragSource(w, Rect(0, 0, 140, 24));
			n.object = item;
		
			item.paletteExample(w, Rect(0,0,140,24));
		});
	}
}
