View : QObject {
	classvar <globalKeyDownAction, <globalKeyUpAction;
	classvar <hSizePolicy;
	classvar <vSizePolicy;
	// drag-and-drop
	classvar <currentDrag, <currentDragString;

	var wasRemoved = false;
	// general props
	var <font, <resize = 1, <alpha = 1.0;
	// container props
	var <decorator, <layout;
	// top window props
	var <>userCanClose=true, <>deleteOnClose = true;
	// actions
	var <action;
	var <mouseDownAction, <mouseUpAction, <mouseEnterAction, <mouseLeaveAction;
	var <mouseMoveAction, <mouseOverAction, <mouseWheelAction;
	var <keyDownAction, <keyUpAction, <keyModifiersChangedAction;
	var <>keyTyped;
	// focus
	var <focusGainedAction, <focusLostAction;
	// drag-and-drop
	var <>dragLabel;
	var <beginDragAction, <canReceiveDragHandler, <receiveDragHandler;
	// window actions
	var <toFrontAction, <endFrontAction;
	// hooks
	var <onClose, <onResize, <onMove;


	*initClass {
		hSizePolicy = [1,2,3,1,2,3,1,2,3];
		vSizePolicy = [1,1,1,2,2,2,3,3,3];
	}

	*new { arg parent, bounds;
		var p = parent.asView;
		^super.new( [p, bounds.asRect] ).initView( p );
	}

	*newCustom { arg customArgs;
		^super.new( customArgs ).initView( nil );
	}

	*qtClass { ^'QcDefaultWidget' }

	asView { ^this }

	refresh {
		_QWidget_Refresh
		^this.primitiveFailed;
	}

	remove {
		if( this.parent.notNil and: { this.parent.decorator.notNil } )
		{ this.parent.decorator.remove(this) };
		this.destroy;
		wasRemoved = true;
		this.children.do { |child| child.remove };
	}

	mapToGlobal { arg point;
		_QWidget_MapToGlobal
		^this.primitiveFailed;
	}

	// ----------------- properties --------------------------
	font_ { arg aFont;
		font = aFont;
		this.setProperty( \font, aFont );
	}

	toolTip { ^this.getProperty(\toolTip) }
	toolTip_ { arg t; this.setProperty( \toolTip, t ) }

	palette {
		^this.getProperty( \palette );
	}

	palette_ { arg p;
		this.setProperty( \palette, p );
	}

	background { ^this.getProperty(\background); }
	background_ { arg color; this.setProperty(\background, color); }

	// NOTE: only for backwards compatibility
	backColor_ { arg color;
		this.background = color;
	}

	backgroundImage_ { arg image; this.setBackgroundImage(image) }

	setBackgroundImage { arg image, tileMode=1, alpha=1.0, fromRect;
		if (image.notNil) {
			this.invokeMethod(\setBackgroundImage, [
				image,
				fromRect ?? { image.bounds },
				tileMode.asInteger,
				alpha.asFloat
			]);
		}{
			this.invokeMethod(\removeBackgroundImage);
		};
	}

	absoluteBounds {
		^this.bounds.moveToPoint( this.mapToGlobal( 0@0 ) );
	}

	bounds {
		^this.getProperty(\geometry)
	}

	bounds_ { arg rect;
		this.setProperty(\geometry, rect.asRect )
	}

	sizeHint { ^this.getProperty(\sizeHint) }

	minSizeHint { ^this.getProperty(\minimumSizeHint) }

	// a Point can be passed instead of a Size
	maxSize_ { arg size;
		var max = QLimits(\maxWidgetSize);
		size = size.asSize;
		this.setProperty( \maximumSize, Size( min(max,size.width), min(max,size.height) ) );
	}

	// a Point can be passed instead of a Size
	minSize_ { arg size; this.setProperty( \minimumSize, size.asSize ); }

	fixedSize_ { arg size;
		size = size.asSize;
		this.setProperty( \minimumSize, size );
		this.setProperty( \maximumSize, size );
	}

	fixedWidth_ { arg width;
		this.setProperty( \minimumWidth, width );
		this.setProperty( \maximumWidth, width );
	}

	fixedHeight_ { arg height;
		this.setProperty( \minimumHeight, height );
		this.setProperty( \maximumHeight, height );
	}

	maxWidth_ { arg width;
		this.setProperty( \maximumWidth, min( width, QLimits(\maxWidgetSize) ) );
	}

	minWidth_ { arg width; this.setProperty( \minimumWidth, width ); }

	maxHeight_ { arg height;
		this.setProperty( \maximumHeight, min( height, QLimits(\maxWidgetSize) ) );
	}

	minHeight_ { arg height; this.setProperty( \minimumHeight, height ); }

	// backwards compatibility
	relativeOrigin { ^true }

	moveTo { arg x, y;
		this.bounds_( this.bounds.moveTo( x, y ) );
	}

	resizeTo { arg width, height;
		this.bounds_( this.bounds.resizeTo( width, height ) );
	}

	resizeToBounds { arg rect;
		this.bounds_( this.bounds.resizeTo( rect.width, rect.height ) );
	}

	resizeToHint {
		var size = this.sizeHint;
		this.bounds_( this.bounds.resizeTo( size.width, size.height ) );
	}

	visible {
		^this.getProperty(\visible)
	}
	visible_ { arg bool;
		this.setProperty(\visible, bool, false)
	}

	enabled {
		^this.getProperty(\enabled)
	}
	enabled_ { arg bool;
		this.setProperty(\enabled, bool)
	}

	resize_ { arg anInt;
		this.setProperty(\_qc_hSizePolicy, hSizePolicy[anInt-1]);
		this.setProperty(\_qc_vSizePolicy, vSizePolicy[anInt-1]);
		resize = anInt;
	}

	canFocus {
		var policy = this.getProperty(\focusPolicy);
		^( policy > 0 )
	}

	canFocus_ { arg bool;
		var policy;
		if( bool ) { policy = 16r1 | 16r2 | 16r8 } { policy = 0 };
		this.setProperty(\focusPolicy, policy);
	}

	focus { arg flag=true;
		_QWidget_SetFocus
		^this.primitiveFailed;
	}

	hasFocus {
		^this.getProperty( \focus );
	}

	acceptsMouse {
		_QWidget_AcceptsMouse
		^this.primitiveFailed;
	}

	acceptsMouse_ { arg bool;
		_QWidget_SetAcceptsMouse
		^this.primitiveFailed;
	}

	focusColor_ { arg color;
		this.setProperty(\focusColor, color);
	}

	focusColor {
		^this.getProperty(\focusColor) ? Color();
	}

	// ------------------ container stuff ----------------------------

	children { arg class = View;
		var ch = super.children( class );
		^ch.select { |v| (v.tryPerform(\isClosed) ? false).not };
	}

	parent { arg class = View;
		if (wasRemoved) { ^nil } { ^super.parent(class) };
	}

	parents {
		var allParents;
		var p;
		if (wasRemoved.not) {
			p = this.parent;
			while { p.notNil } {
				allParents = allParents.add( p );
				p = p.parent;
			};
		};
		^allParents;
	}

	getParents {
		^this.parents;
	}

	removeAll {
		var childWidgets = this.children( View );
		childWidgets.do { |child| child.remove };
	}

	layout_ { arg newLayout;
		if( newLayout.notNil && (newLayout != layout) ) {
			this.prSetLayout( newLayout );
			layout = newLayout;
		};
	}

	addFlowLayout { arg margin, gap;
		this.decorator_( FlowLayout( this.bounds.moveTo(0, 0), margin, gap ) );
		^this.decorator;
	}

	decorator_ { arg decor;
		decor.bounds = decor.bounds.moveTo(0, 0);
		decor.reset;
		decorator = decor;
	}

	// ................. top window stuff ............................

	name {
		^this.getProperty( \windowTitle );
	}

	name_ { arg string;
		this.setProperty( \windowTitle, string );
	}

	front {
		_QWidget_BringFront
		^this.primitiveFailed;
	}

	minimize {
		if( this.visible ) { this.invokeMethod( \showMinimized, synchronous:false ) };
	}

	unminimize {
		if( this.getProperty( \minimized ) ) { this.invokeMethod( \showNormal, synchronous:false ) };
	}

	fullScreen {
		this.invokeMethod( \showFullScreen, synchronous:false );
	}

	endFullScreen {
		if( this.getProperty( \fullScreen ) ) { this.invokeMethod( \showNormal, synchronous:false ) };
	}

	alpha_ { arg aFloat;
		alpha = aFloat;
		this.setProperty( \windowOpacity, aFloat );
	}

	alwaysOnTop {
		_QWidget_GetAlwaysOnTop
		^this.primitiveFailed;
	}

	alwaysOnTop_ { arg boolean;
		_QWidget_SetAlwaysOnTop
		^this.primitiveFailed;
	}

	close {
		if( this.getProperty( \fullScreen ) ? false ) { this.invokeMethod( \showNormal, synchronous:false ) };
		if( deleteOnClose )
		{ this.remove; }
		{ this.visible_( false ); }
	}

	isClosed {
		if (wasRemoved) {^true} {^this.isValid.not};
	}

	notClosed { ^this.isClosed.not }

	// TODO: deprecate acceptsMouseOver and expose 'mouseTracking' property directly
	acceptsMouseOver { ^this.getProperty(\_qc_win_mouse_tracking) == true }
	acceptsMouseOver_ { arg flag; this.setProperty(\_qc_win_mouse_tracking, flag == true) }

	// ----------------- actions .....................................

	action_ { arg func;
		// NOTE: not all widgets have action() signal
		try {
			this.manageMethodConnection( action, func, 'action()', \doAction );
		};
		action = func;
	}

	addAction { arg func, selector=\action;
		this.perform(selector.asSetter, this.perform(selector).addFunc(func));
	}

	removeAction { arg func, selector=\action;
		this.perform(selector.asSetter, this.perform(selector).removeFunc(func));
	}

	*globalKeyDownAction_ { arg action;
		globalKeyDownAction = action;
		this.setGlobalEventEnabled( 16r01 /* KeyPress */, true );
	}


	*globalKeyUpAction_ { arg action;
		globalKeyUpAction = action;
		this.setGlobalEventEnabled( 16r02 /* KeyRelease */, true );
	}

	keyDownAction_ { arg aFunction;
		keyDownAction = aFunction;
		this.setEventHandlerEnabled( QObject.keyDownEvent, true );
	}

	keyUpAction_ { arg aFunction;
		keyUpAction = aFunction;
		this.setEventHandlerEnabled( QObject.keyUpEvent, true );
	}

	keyModifiersChangedAction_ { arg aFunction;
		keyModifiersChangedAction = aFunction;
		this.setEventHandlerEnabled( QObject.keyDownEvent, true );
		this.setEventHandlerEnabled( QObject.keyUpEvent, true );
	}

	mouseDownAction_ { arg aFunction;
		mouseDownAction = aFunction;
		this.setEventHandler( QObject.mouseDownEvent, \mouseDownEvent, true );
		this.setEventHandler( QObject.mouseDblClickEvent, \mouseDownEvent, true );
	}

	mouseUpAction_ { arg aFunction;
		mouseUpAction = aFunction;
		this.setEventHandler( QObject.mouseUpEvent, \mouseUpEvent, true );
	}

	mouseMoveAction_ { arg aFunction;
		mouseMoveAction = aFunction;
		this.setEventHandler( QObject.mouseMoveEvent, \mouseMoveEvent, true );
	}

	// mouseOverAction responds to same Qt event as mouseMoveAction,
	// but on different conditions.
	// See View:-mouseMoveEvent method.
	mouseOverAction_ { arg aFunction;
		mouseOverAction = aFunction;
		this.setEventHandler( QObject.mouseMoveEvent, \mouseMoveEvent, true );
		this.setProperty(\mouseTracking, true);
	}

	mouseEnterAction_ { arg aFunction;
		mouseEnterAction = aFunction;
		this.setEventHandler( QObject.mouseEnterEvent, \mouseEnterEvent, true );
	}

	mouseLeaveAction_ { arg aFunction;
		mouseLeaveAction = aFunction;
		this.setEventHandler( QObject.mouseLeaveEvent, \mouseLeaveEvent, true );
	}

	mouseWheelAction_ { arg aFunction;
		mouseWheelAction = aFunction;
		this.setEventHandler( QObject.mouseWheelEvent, \mouseWheelEvent, true );
	}

	beginDragAction_ { arg handler;
		beginDragAction = handler;
		this.setEventHandler( QObject.mouseDownEvent, \mouseDownEvent, true )
	}

	canReceiveDragHandler_ { arg handler;
		canReceiveDragHandler = handler;
		this.setDragEventsEnabled( true );
	}

	receiveDragHandler_ { arg handler;
		receiveDragHandler = handler;
		this.setDragEventsEnabled( true );
	}

	defaultGetDrag { ^nil }
	defaultCanReceiveDrag { ^false }
	defaultReceiveDrag {}

	toFrontAction_ { arg aFunction;
		toFrontAction = aFunction;
		this.setEventHandler( QObject.windowActivateEvent,
			\onWindowActivateEvent );
	}

	endFrontAction_ { arg aFunction;
		endFrontAction = aFunction;
		this.setEventHandler( QObject.windowDeactivateEvent,
			\onWindowDeactivateEvent );
	}

	focusGainedAction_ { arg handler;
		focusGainedAction = handler;
		this.setEventHandler( 8 /* QEvent::FocusIn */, \focusInEvent );
	}

	focusLostAction_ { arg handler;
		focusLostAction = handler;
		this.setEventHandler( 9 /* QEvent::FocusOut */, \focusOutEvent );
	}

	onMove_ { arg aFunction;
		onMove = aFunction;
		this.setEventHandler( 13 /* QEvent::Move */, \moveEvent );
	}

	onResize_ { arg aFunction;
		onResize = aFunction;
		this.setEventHandler( 14 /* QEvent::Resize */, \resizeEvent );
	}

	onClose_ { arg func;
		this.manageFunctionConnection( onClose, func, 'destroyed()', false );
		onClose = func;
	}

	doAction {
		action.value(this);
	}

	defaultKeyDownAction { arg char, modifiers, unicode, keycode, key; }

	defaultKeyUpAction { arg char, modifiers, unicode, keycode, key; }

	keyDown { arg char, modifiers, unicode, keycode, key;
		if( keyDownAction.notNil ) {
			^keyDownAction.value( this, char, modifiers, unicode, keycode, key );
		} {
			^this.defaultKeyDownAction( char, modifiers, unicode, keycode, key );
		};
	}

	keyUp { arg char, modifiers, unicode, keycode, key;
		keyTyped = char;
		if( keyUpAction.notNil ) {
			^keyUpAction.value( this, char, modifiers, unicode, keycode, key );
		} {
			^this.defaultKeyUpAction( char, modifiers, unicode, keycode, key );
		};
	}

	keyModifiersChanged { arg modifiers;
		keyModifiersChangedAction.value( this, modifiers);
	}

	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;
		^mouseDownAction.value( this, x, y, modifiers, buttonNumber, clickCount );
	}

	mouseUp { arg x, y, modifiers, buttonNumber;
		^mouseUpAction.value( this, x, y, modifiers, buttonNumber );
	}

	mouseMove { arg x, y, modifiers;
		^mouseMoveAction.value( this, x, y, modifiers );
	}

	mouseOver { arg x, y;
		^mouseOverAction.value( this, x, y );
	}

	mouseEnter {
		^mouseEnterAction.value(this);
	}

	mouseLeave {
		^mouseLeaveAction.value(this);
	}

	mouseWheel { arg x, y, modifiers, xDelta, yDelta;
		^mouseWheelAction.value( this, x, y, modifiers, xDelta, yDelta );
	}

	/* ---------------- private ----------------------- */

	*prSetCurrentDrag { arg obj; currentDrag = obj; currentDragString = obj.asCompileString; }
	*prClearCurrentDrag { currentDrag = nil; currentDragString = nil; }

	*setGlobalEventEnabled { arg event, enabled;
		_QWidget_SetGlobalEventEnabled
		^this.primitiveFailed;
	}

	initView { arg parent;

		var handleKeyDown, handleKeyUp, overridesMouseDown, handleDrag;

		if (parent.notNil) {
			if( parent.decorator.notNil ) { parent.decorator.place(this) }
		};

		this.setEventHandler( QObject.closeEvent, \onCloseEvent, true );

		// key events
		handleKeyDown = handleKeyUp = this.overrides( \keyModifiersChanged );
		if( handleKeyDown.not )
		{ handleKeyDown = this.overrides( \defaultKeyDownAction ) };
		if( handleKeyUp.not )
		{ handleKeyUp = this.overrides( \defaultKeyUpAction )};

		this.setEventHandler( QObject.keyDownEvent, \keyDownEvent, true, enabled: handleKeyDown );
		this.setEventHandler( QObject.keyUpEvent, \keyUpEvent, true, enabled: handleKeyUp );

		// mouse events
		overridesMouseDown = this.overrides( \mouseDown );
		if( this.respondsTo(\defaultGetDrag) || overridesMouseDown )
		{this.setEventHandler( QObject.mouseDownEvent, \mouseDownEvent, true )};
		if( overridesMouseDown )
		{this.setEventHandler( QObject.mouseDblClickEvent, \mouseDownEvent, true )};
		if( this.overrides( \mouseUp ) )
		{this.setEventHandler( QObject.mouseUpEvent, \mouseUpEvent, true )};
		if( this.overrides( \mouseMove ) || this.overrides( \mouseOver ) )
		{this.setEventHandler( QObject.mouseMoveEvent, \mouseMoveEvent, true )};
		if( this.overrides( \mouseEnter ) )
		{this.setEventHandler( QObject.mouseEnterEvent, \mouseEnterEvent, true )};
		if( this.overrides( \mouseLeave ) )
		{this.setEventHandler( QObject.mouseLeaveEvent, \mouseLeaveEvent, true )};
		if( this.overrides( \mouseWheel ) )
		{this.setEventHandler( QObject.mouseWheelEvent, \mouseWheelEvent, true )};

		// DnD events
		handleDrag = this.respondsTo(\defaultCanReceiveDrag) or: {this.respondsTo(\defaultReceiveDrag)};
		this.setEventHandler( 60, \dragEnterEvent, true, enabled:handleDrag );
		this.setEventHandler( 61, \dragMoveEvent, true, enabled:handleDrag );
		this.setEventHandler( 63, \dropEvent, true, enabled:handleDrag );
	}

	onCloseEvent {
		if( userCanClose != false ) {
			if( deleteOnClose != false ) { this.remove; ^true };
		}{
			^false;
		};
	}

	onWindowActivateEvent {
		toFrontAction.value(this);
	}

	onWindowDeactivateEvent {
		endFrontAction.value(this);
	}

	focusInEvent { focusGainedAction.value(this) }
	focusOutEvent { focusLostAction.value(this) }

	moveEvent { onMove.value(this) }
	resizeEvent { onResize.value(this) }

	keyDownEvent { arg char, modifiers, unicode, keycode, key, spontaneous;
		modifiers = QKeyModifiers.toCocoa(modifiers);

		if( spontaneous ) {
			// this event has never been propagated to parent yet
			View.globalKeyDownAction.value( this, char, modifiers, unicode, keycode, key );
		};

		if( (key == 16r1000020) || (key == 16r1000021) ||
			(key == 16r1000022) || (key == 16r1000023 ) )
		{ this.keyModifiersChanged( modifiers ) };

		^this.keyDown( char, modifiers, unicode, keycode, key );
	}

	keyUpEvent { arg char, modifiers, unicode, keycode, key, spontaneous;
		modifiers = QKeyModifiers.toCocoa(modifiers);

		if( spontaneous ) {
			// this event has never been propagated to parent yet
			View.globalKeyUpAction.value( this, char, modifiers, unicode, keycode, key );
		};

		if( (key == 16r1000020) || (key == 16r1000021) ||
			(key == 16r1000022) || (key == 16r1000023 ) )
		{ this.keyModifiersChanged( modifiers ) };

		^this.keyUp( char, modifiers, unicode, keycode, key );
	}

	mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
		// WARNING: DragView and ListView override this method!

		if( (modifiers & QKeyModifiers.control) > 0 ) { // if Ctrl / Cmd mod
			// Try to get drag obj and start a drag.
			// If successful, block further processing of this event.
			if( this.beginDrag( x, y ) ) { ^true };
		};

		// else continue to handle mouse down event
		modifiers = QKeyModifiers.toCocoa(modifiers);
		^this.mouseDown( x, y, modifiers, buttonNumber, clickCount );
	}

	mouseUpEvent { arg x, y, modifiers, buttonNumber;
		modifiers = QKeyModifiers.toCocoa(modifiers);
		^this.mouseUp(  x, y, modifiers, buttonNumber );
	}

	mouseMoveEvent { arg x, y, modifiers, buttons;
		// WARNING: Overridden in ListView!
		if( buttons != 0 ) {
			modifiers = QKeyModifiers.toCocoa(modifiers);
			^this.mouseMove( x, y, modifiers );
		}{
			^this.mouseOver( x, y )
		}
	}

	mouseEnterEvent {
		var dummy = 0; // prevent this method from being optimized away
		^this.mouseEnter;
	}

	mouseLeaveEvent {
		var dummy = 0; // prevent this method from being optimized away
		^this.mouseLeave;
	}

	mouseWheelEvent { arg x, y, modifiers, xDelta, yDelta;
		modifiers = QKeyModifiers.toCocoa(modifiers);
		^this.mouseWheel( x, y, modifiers, xDelta, yDelta );
	}

	beginDrag { arg x, y;
		var obj, str;
		if( beginDragAction.notNil )
		{ obj = beginDragAction.value( this, x, y ) }
		{ obj = this.tryPerform( \defaultGetDrag, x, y ) };
		if( obj.notNil ) {
			View.prSetCurrentDrag( obj );
			str = obj.asString;
			this.prStartDrag( dragLabel ?? str, obj, str );
			^true;
		};
		^false;
	}

	canReceiveDrag { arg x, y;
		if( canReceiveDragHandler.notNil )
		{ ^this.canReceiveDragHandler.value( this, x, y ) }
		{ ^( this.tryPerform( \defaultCanReceiveDrag, x, y ) ? false ) };
	}

	receiveDrag { arg x, y;
		if( receiveDragHandler.notNil )
		{ this.receiveDragHandler.value( this, x, y ) }
		{ this.tryPerform( \defaultReceiveDrag, x, y ) };
	}

	prStartDrag { arg label, data, dataAsString;
		_QWidget_StartDrag
		^this.primitiveFailed;
	}

	dragEnterEvent { arg internal, data;
		if(internal.not) {
			// dnd incoming from outside SC
			View.prSetCurrentDrag(data);
		};
		// always accept the event
		^true;
	}

	dragMoveEvent { arg x, y;
		// make sure the event is always consumed
		^this.canReceiveDrag( x, y ).switch (
			true, true,
			false, false,
			false
		)
	}

	dropEvent { arg x, y;
		this.receiveDrag( x, y );
		// always accept the event
		^true
	}

	setDragEventsEnabled { arg enabled;
		this.setEventHandlerEnabled( 60, enabled );
		this.setEventHandlerEnabled( 61, enabled );
		this.setEventHandlerEnabled( 63, enabled );
	}

	enableContextMenu_{
		|bool|
		this.setProperty('contextMenuPolicy', bool.if(2, 0));
	}

	enableContextMenu {
		^(this.getProperty('contextMenuPolicy') == 2);
	}

	setContextMenuActions {
		|...actions|
		this.removeAllMenuActions();

		if ((actions.size > 0) && (actions != [nil])) {
			this.enableContextMenu = true;
			actions.do({ |a| this.insertMenuAction(a.asMenuAction) });
		} {
			this.enableContextMenu = false;
		}
	}

	insertMenuAction {
		|action, addBefore|
		^this.class.prInsertMenuAction(this, action, addBefore)
	}

	removeMenuAction {
		|action|
		^this.class.prRemoveMenuAction(this, action)
	}

	removeAllMenuActions {
		^this.class.prRemoveAllMenuActions(this);
	}

	*prInsertMenuAction {
		|view, action, addBefore|
		_QView_AddActionToView
		^this.primitiveFailed;
	}

	*prRemoveMenuAction {
		|view, action|
		_QView_RemoveActionFromView
		^this.primitiveFailed;
	}

	*prRemoveAllMenuActions {
		|view|
		_QView_RemoveAllActionsFromView
		^this.primitiveFailed;
	}

	prSetLayout { arg layout;
		_QWidget_SetLayout
		^this.primitiveFailed;
	}

	manageMethodConnection { arg oldAction, newAction, signal, method, sync=false;
		if( newAction !== oldAction ) {
			case
			{ oldAction.isNil && newAction.notNil } {this.connectMethod (signal, method, sync)}
			{ oldAction.notNil && newAction.isNil } {this.disconnectMethod (signal, method)}
			;
		};
	}

	manageFunctionConnection { arg oldAction, newAction, signal, sync=false;
		if( newAction !== oldAction ) {
			if( oldAction.notNil ) {this.disconnectFunction (signal, oldAction)};
			if( newAction.notNil ) {this.connectFunction (signal, newAction, sync)};
		};
	}

	overrides { arg method; ^this.prOverrides(View, method) }

	prOverrides { arg superclass, method;
		_Qt_IsMethodOverridden
		^this.primitiveFailed
	}

	nonimpl { arg methodName;
		this.class.nonimpl( methodName );
	}

	*nonimpl { arg methodName;
		this.debug( msg: methodName.asString ++ " is not implemented yet" )
	}

	*debug { arg level = 1, msg = "";
		if( QtGUI.debugLevel >= level ) { ("Qt: " ++ this.asString ++ ": " ++ msg).postln }
	}
}
