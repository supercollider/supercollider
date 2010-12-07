
SCView {  // abstract class
	classvar <>currentDrag, <>currentDragString;
	classvar <>globalKeyDownAction, <>globalKeyUpAction;

	var dataptr, <parent, <>action, <background;
	var <>mouseDownAction, <>mouseUpAction, <>mouseOverAction, <>mouseMoveAction;
	var <>keyDownAction, <>keyUpAction, <>keyTyped, <> keyModifiersChangedAction;
	var <>beginDragAction, <>canReceiveDragHandler, <>receiveDragHandler;
	var <>onClose;

	*new { arg parent, bounds;
		^super.new.init(parent, bounds);
	}
	
	*viewClass { ^this }
	
	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds);
	}

	init { arg argParent, argBounds;
		parent = argParent.asView; // actual view
		background = Color.clear;
			// call asView again because parent by this point might be a FlowView
		this.prInit(parent.asView, argBounds.asRect, this.class.viewClass);
		argParent.add(this);//maybe window or viewadapter
	}

	asView { ^this }

	bounds {
		^this.getProperty(\bounds, Rect.new)
	}
	bounds_ { arg rect;
		this.setProperty(\bounds, rect)
	}

	visible {
		^this.getProperty(\visible)
	}
	visible_ { arg bool;
		this.setProperty(\visible, bool)
	}

	enabled {
		^this.getProperty(\enabled)
	}
	enabled_ { arg bool;
		this.setProperty(\enabled, bool)
	}

	canFocus {
		^this.getProperty(\canFocus)
	}
	canFocus_ { arg bool;
		this.setProperty(\canFocus, bool)
	}
	
	focus { arg flag=true;
		_SCView_Focus
		^this.primitiveFailed
	}
	
	hasFocus {
		_SCView_HasFocus
		^this.primitiveFailed
	}

	focusColor_ { |color|
		this.setProperty(\focusColor, color)
	}
	focusColor {
		^this.getProperty(\focusColor, Color.new)
	}

	id {
		^this.getProperty(\id)
	}
	id_ { arg id;
		this.setProperty(\id, id)
	}

	dragLabel_ { arg string;
		this.setProperty(\dragLabel, string)
	}

	refresh {
		_SCView_Refresh
		^this.primitiveFailed
	}
	
	findByID { arg id;
		_SCView_FindByID
		^this.primitiveFailed
	}

	isClosed { ^dataptr.isNil }
	notClosed { ^dataptr.notNil }
	remove {
		if(dataptr.notNil) {
			parent.prRemoveChild(this);
			this.prRemove;
			this.prClose;
		} {
			"SCView-remove : this view already removed.".debug(this);
		}
	}
	/*
	resize behaviour in an SCCompositeView:
		1  2  3
		4  5  6
		7  8  9

		1 - fixed to left, fixed to top
		2 - horizontally elastic, fixed to top
		3 - fixed to right, fixed to top

		4 - fixed to left, vertically elastic
		5 - horizontally elastic, vertically elastic
		6 - fixed to right, vertically elastic

		7 - fixed to left, fixed to bottom
		8 - horizontally elastic, fixed to bottom
		9 - fixed to right, fixed to bottom
	*/
	
	resize {
		^this.getProperty(\resize)
	}
	
	resize_ { arg resize;
		this.setProperty(\resize, resize)
	}

	background_ { arg color;
		background = color;
		this.setProperty(\background, color)
	}
	
	addAction { arg func, selector=\action;
		this.perform(selector.asSetter, this.perform(selector).addFunc(func));
	}
	
	removeAction { arg func, selector=\action;
		this.perform(selector.asSetter, this.perform(selector).removeFunc(func));
	}
	
	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount);
	}
	
	mouseUp { arg x, y, modifiers;
		mouseUpAction.value(this, x, y, modifiers)
	}
	
	mouseMove { arg x, y, modifiers;
		mouseMoveAction.value(this, x, y, modifiers)
	}
	
	mouseOver { arg x, y;
		mouseOverAction.value(this, x, y);
	}

	keyDown { arg char, modifiers, unicode, keycode;
		globalKeyDownAction.value(this, char, modifiers, unicode, keycode);
		this.handleKeyDownBubbling(this, char, modifiers, unicode, keycode);
	}

	keyModifiersChanged { arg modifiers;
		this.handleKeyModifiersChangedBubbling(this, modifiers)
	}

	handleKeyModifiersChangedBubbling { arg view, modifiers;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyModifiersChangedAction.isNil) {
//			this.defaultKeyDownAction(char, modifiers, unicode, keycode);
			result = nil;
		} {
			result = keyModifiersChangedAction.value(view, modifiers);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyModifiersChangedBubbling(view, modifiers);
		};
	}

	defaultKeyDownAction { ^nil }
	
	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyDownAction.isNil) {
			result = this.defaultKeyDownAction(char, modifiers, unicode, keycode);
		} {
			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	// sc.solar addition
	keyUp { arg char, modifiers, unicode, keycode;
		this.keyTyped = char;
		// always call global keydown action first
		globalKeyUpAction.value(this, char, modifiers, unicode, keycode);
		this.handleKeyUpBubbling(this, char, modifiers, unicode, keycode);
	}
	
	defaultKeyUpAction { ^nil }
	
	handleKeyUpBubbling { arg view, char, modifiers, unicode, keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyUpAction.isNil) {
			result = this.defaultKeyUpAction(char, modifiers, unicode, keycode);
		} {
			result = keyUpAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyUpBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	beginDrag {
		currentDrag = if (beginDragAction.notNil)
		 {
			beginDragAction.value(this)
		} {
			this.defaultGetDrag
		};
		currentDragString = currentDrag.asCompileString;
	}
	
	defaultGetDrag { ^nil }
	
	canReceiveDrag {
		^if(canReceiveDragHandler.notNil) { 
			canReceiveDragHandler.value(this) 
		} { 
			this.defaultCanReceiveDrag 
		}
	}
	defaultCanReceiveDrag { ^false }
	receiveDrag { |x, y|
		if(receiveDragHandler.notNil) { 
			receiveDragHandler.value(this, x, y) 
		} { 
			this.defaultReceiveDrag(x, y) 
		};
		currentDrag = currentDragString = nil;
	}

	// get the view parent tree up to the SCTopView
	getParents {
		var parents, view;
		view = this;
		parents = List.new;
		while( {(view = view.asView.parent).notNil}, { parents.add(view)});
		^parents
	}

	doAction {
		action.value(this);
	}

	properties {
		^#[\bounds, \visible, \enabled, \canFocus, \resize, \background, 
				\minWidth, \maxWidth, \minHeight, \maxHeight, \focusColor]
	}
	
	getPropertyList {
		^this.properties.collect( { arg name;
			[name, this.perform(name)]
		});
	}
	
	setPropertyList { arg list;
		list.do( { arg item;
			var name, value;
			#name, value = item;
			this.perform(name.asSetter, value);
		});
	}

	// private
	prInit { arg argParent, argBounds, argViewClass;
		_SCView_New
		^this.primitiveFailed
	}
	
	prClose { dataptr = nil; onClose.value(this); }
	prRemove {
		_SCView_Remove
		^this.primitiveFailed
	}
	setProperty { arg key, value;
		_SCView_SetProperty
		^this.primitiveFailed
	}
	getProperty { arg key, value;
		_SCView_GetProperty
		^this.primitiveFailed
	}
	setPropertyWithAction { arg symbol, obj;
		// setting some properties may need to have the action called.
		if (this.setProperty(symbol, obj), {
			// setProperty returns true if action needs to be called.
			this.doAction;
		});
	}

//	*importDrag {
//		// this is called when an NSString is the drag object
//		// from outside of the SC app
//		// we compile it to an SCObject.
//		currentDragString = currentDrag;
//		currentDrag = currentDrag.interpret;
//	}

	absoluteBounds {
		^this.getProperty(\absoluteBounds, Rect.new)
	}
}

SCContainerView : SCView { // abstract class

	var <children, <decorator;

	add { arg child;
		children = children.add(child);
		if (decorator.notNil, { decorator.place(child); });
	}

	init { arg argParent, argBounds;
		super.init(argParent, argBounds);
	}

	removeAll {
		children.copy.do { |child| child.remove };
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

	prRemoveChild { arg child;
		children.remove(child);
		// ... decorator should re-place all
	}

	//bounds_  ... replace all

	prClose {
		super.prClose;
		children.do( { arg item; item.prClose });
	}

}

SCCompositeView : SCContainerView {
}

SCTopView : SCCompositeView {
	
	// created by SCWindow
	handleKeyModifiersChangedBubbling { arg view, modifiers;
		keyModifiersChangedAction.value(view, modifiers);
	}
	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
		var currentAppModal, window;
		keyDownAction.value(view, char, modifiers, unicode, keycode);
	}
	handleKeyUpBubbling { arg view, char, modifiers, unicode, keycode;
		keyUpAction.value(view, char, modifiers, unicode, keycode);
	}

	//only in construction mode, handled internally
	canReceiveDrag { ^currentDrag.isKindOf(Class)}
//	remove { this.removeAll }

	findWindow {
		SCWindow.allWindows.do { |win|
			if(win.view == this) {
				^win
			}
		}
	}

	/* construction mode */

	constructionGrid_ { arg point;
		this.setProperty( \constructionGrid, point );
	}

	constructionGrid {
		^this.getProperty( \constructionGrid, Point.new );
	}

	enableConstructionGrid_ { arg flag;
		this.setProperty( \enableConstructionGrid, flag );
	}

	enableConstructionGrid {
		^this.getProperty( \enableConstructionGrid );
	}

	//private called from lang
	setConstructionMode { |flag|
		this.setProperty( \setConstructionMode, flag )
	}

	defaultReceiveDrag { |x, y|
		var win, view;
		win = this.findWindow;
		view = currentDrag.paletteExample(win, Rect(x, y, 140, 24)).enabled_(false);
		view.keyDownAction_( { |view, char, modifiers, unicode, keycode|
			if(keycode == 51) {
				view.remove;
			}
		});
	}
}

SCScrollTopView : SCTopView {
	var <autohidesScrollers = true, <hasHorizontalScroller = true, <hasVerticalScroller = true;
	var <autoScrolls = true;

	autohidesScrollers_ { |bool|
		autohidesScrollers = bool;
		this.setProperty(\setAutohidesScrollers, bool);
	}

	hasHorizontalScroller_ { |bool|
		hasHorizontalScroller = bool;
		this.setProperty(\setHasHorizontalScroller, bool);
	}

	hasVerticalScroller_ { |bool|
		hasVerticalScroller = bool;
		this.setProperty(\setHasVerticalScroller, bool);
	}

	visibleOrigin_ { arg point;
		this.setProperty( \clipViewOrigin, point );
	}

	visibleOrigin { ^this.getProperty( \clipViewOrigin, Point.new );}

	autoScrolls_ { |bool|
		autoScrolls = bool;
		this.setProperty(\setAutoScrolls, bool);
	}

	innerBounds {
		^this.getProperty(\innerBounds, Rect.new)
	}

	bounds {
		var	bounds = this.absoluteBounds;
		this.getParents.do( { |parent|
			bounds = bounds.moveBy(parent.bounds.left.neg, parent.bounds.top.neg)
		});
		^bounds
	}

//	handleKeyModifiersChangedBubbling { arg view, modifiers;
//		var result;
//		// nil from keyDownAction --> pass it on
//		if (keyModifiersChangedAction.isNil) {
//			result = nil;
//		} {
//			result = keyModifiersChangedAction.value(view, modifiers);
//		};
//		if(result.isNil) {
//			// call keydown action of parent view
//			parent.handleKeyModifiersChangedBubbling(view, modifiers);
//		};
//	}
//
//	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
//		var result;
//		// nil from keyDownAction --> pass it on
//		if (keyDownAction.isNil) {
//			result = this.defaultKeyDownAction(char, modifiers, unicode, keycode);
//		} {
//			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
//		};
//		if(result.isNil) {
//			// call keydown action of parent view
//			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
//		};
//	}
//
//	handleKeyUpBubbling { arg view, char, modifiers, unicode, keycode;
//		var result;
//		// nil from keyDownAction --> pass it on
//		if (keyUpAction.isNil) {
//			result = this.defaultKeyUpAction(char, modifiers, unicode, keycode);
//		} {
//			result = keyUpAction.value(view, char, modifiers, unicode, keycode);
//		};
//		if(result.isNil) {
//			// call keydown action of parent view
//			parent.handleKeyUpBubbling(view, char, modifiers, unicode, keycode);
//		};
//	}

}

SCScrollView : SCScrollTopView {
	var <hasBorder = false;

	hasBorder_ { arg bool = true;
		this.setProperty(\border, bool);
	}

	init { |argParent, argBounds|
		super.init(argParent, argBounds);
	}

	handleKeyModifiersChangedBubbling { arg view, modifiers;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyModifiersChangedAction.isNil) {
			result = nil;
		} {
			result = keyModifiersChangedAction.value(view, modifiers);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyModifiersChangedBubbling(view, modifiers);
		};
	}

	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyDownAction.isNil) {
			result = this.defaultKeyDownAction(char, modifiers, unicode, keycode);
		} {
			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	handleKeyUpBubbling { arg view, char, modifiers, unicode, keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyUpAction.isNil) {
			result = this.defaultKeyUpAction(char, modifiers, unicode, keycode);
		} {
			result = keyUpAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyUpBubbling(view, char, modifiers, unicode, keycode);
		};
	}

}

SCLayoutView : SCContainerView {
	properties { ^super.properties ++ #[\spacing] }

	spacing {
		^this.getProperty(\spacing, 0)
	}
	spacing_ { arg distance;
		this.setProperty(\spacing, distance)
	}
	setProperty { |key, value|
		super.setProperty(key, value);
	}
}

SCHLayoutView : SCLayoutView {}
SCVLayoutView : SCLayoutView {}


SCControlView : SCView { // abstract class
}

SCSliderBase : SCControlView {

	var <>shift_scale = 100.0, <>ctrl_scale = 10.0, <>alt_scale = 0.1;

	getScale { |modifiers|
		^case
			 { modifiers & 131072 == 131072 } { shift_scale }
			 { modifiers & 262144 == 262144 } { ctrl_scale }
			 { modifiers & 524288 == 524288 } { alt_scale }
			 { 1 };
	}

	knobColor {
		^this.getProperty(\knobColor, Color.new)
	}
	knobColor_ { arg color;
		this.setProperty(\knobColor, color)
	}

	step_ { arg stepSize;
	//	this.setPropertyWithAction(\step, stepSize); // action really needed?
		this.setProperty(\step, stepSize);
	}
	
	step {
		^this.getProperty(\step)
	}

	properties {
		^super.properties ++ #[\knobColor, \step]
	}

}

SCSlider : SCSliderBase {

	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}
	valueAction_ { arg val;
		this.setPropertyWithAction(\value, val);
	}

	increment { |zoom=1| ^this.valueAction = this.value + (max(this.step, this.pixelStep) * zoom) }
	decrement { |zoom=1| ^this.valueAction = this.value - (max(this.step, this.pixelStep) * zoom) }

	pixelStep {
		var bounds = this.bounds;
		^(bounds.width.max(bounds.height) - this.thumbSize).reciprocal
	}

	defaultKeyDownAction { arg char, modifiers, unicode, keycode;
		var zoom = this.getScale(modifiers);

		// standard keydown
			// rand could also use zoom factors
		if (char == $r, { this.valueAction = 1.0.rand; ^this });
		if (char == $n, { this.valueAction = 0.0; ^this });
		if (char == $x, { this.valueAction = 1.0; ^this });
		if (char == $c, { this.valueAction = 0.5; ^this });

		if (char == $], { this.increment(zoom); ^this });
		if (char == $[, { this.increment(zoom); ^this });
		if (unicode == 16rF700, { this.increment(zoom); ^this });
		if (unicode == 16rF703, { this.increment(zoom); ^this });
		if (unicode == 16rF701, { this.decrement(zoom); ^this });
		if (unicode == 16rF702, { this.decrement(zoom); ^this });

		^nil		// bubble if it's an invalid key
	}

	defaultGetDrag {
		^this.value
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}

	thumbSize {
		^this.getProperty(\thumbSize, 12);
	}
	thumbSize_ { arg size;
		this.setProperty(\thumbSize, size);
	}

	properties {
		^super.properties ++ #[\thumbSize];
	}
}

SCRangeSlider : SCSliderBase {

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.lo = 0.2;
		v.hi = 0.7;
		^v
	}

	lo {
		^this.getProperty(\lo)
	}
	lo_ { arg val;
		this.setProperty(\lo, val);
	}
	activeLo_ { arg val;
		this.setPropertyWithAction(\lo, val);
	}
	hi {
		^this.getProperty(\hi)
	}
	hi_ { arg val;
		this.setProperty(\hi, val);
	}
	activeHi_ { arg val;
		this.setPropertyWithAction(\hi, val);
	}
	range {
		^this.getProperty(\range)
	}
	range_ { arg val;
		this.setProperty(\range, val);
	}
	activeRange_ { arg val;
		this.setPropertyWithAction(\range, val);
	}

	setSpan { arg lo, hi;
		this.lo = lo;
		this.hi = hi;
	}

	setSpanActive { arg lo, hi;
		this.setSpan( lo, hi );
		this.doAction;
	}

	setDeviation { arg deviation, average;
			var lo = ( 1 - deviation ) * average;
			this.setSpan(lo, lo + deviation);
	}

	properties {
		^super.properties ++ #[\lo, \hi]
	}

	pixelStep {
		var bounds = this.bounds;
		^(bounds.width.max(bounds.height)).reciprocal
	}

	increment { |zoom=1|
		var inc = (max(this.step, this.pixelStep) * zoom);
		var newHi = (this.hi + inc);
		if (newHi > 1) {
			inc = 1 - this.hi;
			newHi = 1;
		};
		this.lo_(this.lo + inc).activeHi_(newHi);
	}

	decrement { |zoom=1|
		var inc = (max(this.step, this.pixelStep) * zoom);
		var newLo = (this.lo - inc);
		if (newLo < 0) {
			inc =  this.lo;
			newLo = 0;
		};
		this.lo_(newLo).activeHi_(this.hi - inc);
	}

	defaultKeyDownAction { arg char, modifiers, unicode;
		var a, b;
		var zoom = this.getScale(modifiers);
		// standard keydown
		if (char == $r, {
			a = 1.0.rand;
			b = 1.0.rand;
			this.activeLo_(min(a, b));
			this.activeHi_(max(a, b));
			^this
		});
		if (char == $n, { this.activeLo_(0.0); this.activeHi_(0.0); ^this });
		if (char == $x, { this.activeLo_(1.0); this.activeHi_(1.0); ^this });
		if (char == $c, { this.activeLo_(0.5); this.activeHi_(0.5); ^this });
		if (char == $a, { this.activeLo_(0.0); this.activeHi_(1.0); ^this });
		if (unicode == 16rF700, { this.increment(zoom); ^this });
		if (unicode == 16rF703, { this.increment(zoom); ^this });
		if (unicode == 16rF701, { this.decrement(zoom); ^this });
		if (unicode == 16rF702, { this.decrement(zoom); ^this });
		^nil		// bubble if it's an invalid key
	}
	defaultGetDrag { ^Point(this.lo, this.hi) }
	defaultCanReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	defaultReceiveDrag {
		// changed to x, y instead of lo, hi
		this.lo = currentDrag.x;
		this.hi = currentDrag.y;
		this.doAction
	}
}

SC2DSlider : SCSliderBase {
	x {
		^this.getProperty(\x)
	}
	x_ { arg val;
		this.setProperty(\x, val);
	}
	activex_ { arg val;
		this.setPropertyWithAction(\x, val);
	}
	y {
		^this.getProperty(\y)
	}
	y_ { arg val;
		this.setProperty(\y, val);
	}
	activey_ { arg val;
		this.setPropertyWithAction(\y, val);
	}

	setXY { arg x, y;
		this.x = x;
		this.y = y;
	}

	setXYActive { arg x, y;
		this.setXY( x, y );
		this.doAction;
	}

	properties {
		^super.properties ++ #[\x, \y]
	}

	pixelStepX { ^(this.bounds.width).reciprocal }
	pixelStepY { ^(this.bounds.height).reciprocal }

	incrementY { |zoom=1| ^this.y = this.y + (this.pixelStepY * zoom) }
	decrementY { |zoom=1| ^this.y = this.y - (this.pixelStepY * zoom) }
	incrementX { |zoom=1| ^this.x = this.x + (this.pixelStepX * zoom) }
	decrementX { |zoom=1| ^this.x = this.x - (this.pixelStepX * zoom) }

	defaultKeyDownAction { arg char, modifiers, unicode, keycode;
		var zoom = this.getScale(modifiers);
		// standard keydown
		if (char == $r, { this.x = 1.0.rand; this.y = 1.0.rand; ^this });
		if (char == $n, { this.x = 0.0; this.y = 0.0; ^this });
		if (char == $x, { this.x = 1.0; this.y = 1.0; ^this });
		if (char == $c, { this.x = 0.5; this.y = 0.5; ^this });
		if (unicode == 16rF700, { this.incrementY(zoom); ^this });
		if (unicode == 16rF703, { this.incrementX(zoom); ^this });
		if (unicode == 16rF701, { this.decrementY(zoom); ^this });
		if (unicode == 16rF702, { this.decrementX(zoom); ^this });
		^nil		// bubble if it's an invalid key
	}
	defaultGetDrag {
		^Point(this.x, this.y)
	}
	defaultCanReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	defaultReceiveDrag {
		this.setXYActive(currentDrag.x, currentDrag.y);
	}
}

SC2DTabletSlider : SC2DSlider {

//	var <>mouseDownAction, <>mouseUpAction;

	mouseDown { arg x, y, pressure, tiltx, tilty, deviceID, 
			 buttonNumber, clickCount, absoluteZ, rotation;
		mouseDownAction.value(this, x, y, pressure, tiltx, tilty, deviceID, 
			buttonNumber, clickCount, absoluteZ, rotation);
	}
	mouseUp { arg x, y, pressure, tiltx, tilty, deviceID, 
			buttonNumber, clickCount, absoluteZ, rotation;
		mouseUpAction.value(this, x, y, pressure, tiltx, tilty, deviceID, 
			buttonNumber, clickCount, absoluteZ, rotation);
	}
	doAction { arg x, y, pressure, tiltx, tilty, deviceID, 
			buttonNumber, clickCount, absoluteZ, rotation;
		action.value(this, x, y, pressure, tiltx, tilty, deviceID, 
			buttonNumber, clickCount, absoluteZ, rotation);
	}
}

SCButton : SCControlView {
	var <font, <states;

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.states = [
			["Push", Color.black, Color.red], 
			["Pop", Color.white, Color.blue]];
		^v
	}

	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}
	valueAction_ { arg val;
		this.setPropertyWithAction(\value, val);
	}

	doAction { arg modifiers;
		action.value(this, modifiers);
	}

	defaultKeyDownAction { arg char, modifiers, unicode;
		if (char == $ , { this.valueAction = this.value + 1; ^this });
		if (char == $\r, { this.valueAction = this.value + 1; ^this });
		if (char == $\n, { this.valueAction = this.value + 1; ^this });
		if (char == 3.asAscii, { this.valueAction = this.value + 1; ^this });
		^nil		// bubble if it's an invalid key
	}

	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}

	states_ { arg array;
		states = array;
		this.setProperty(\states, states);
	}

	properties {
		^super.properties ++ #[\value, \font, \states]
	}

	defaultGetDrag {
		^this.value
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber or: { currentDrag.isKindOf(Function) };
	}
	defaultReceiveDrag {
		if (currentDrag.isNumber) {
			this.valueAction = currentDrag;
		} {
			this.action = currentDrag;
		};
	}
}


SCPopUpMenu : SCControlView {
	var <font, <items;

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.items = #["linear", "exponential", "sine", "welch", "squared", "cubed"];
		^v
	}

	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}
	valueAction_ { arg val;
		this.setPropertyWithAction(\value, val);
	}

	defaultKeyDownAction { arg char, modifiers, unicode;
		if (char == $ , { this.valueAction = this.value + 1; ^this });
		if (char == $\r, { this.valueAction = this.value + 1; ^this });
		if (char == $\n, { this.valueAction = this.value + 1; ^this });
		if (char == 3.asAscii, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF700, { this.valueAction = this.value - 1; ^this });
		if (unicode == 16rF703, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF701, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF702, { this.valueAction = this.value - 1; ^this });
		^nil		// bubble if it's an invalid key
	}
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	items_ { arg array;
		items = array;
		this.setProperty(\items, items);
	}

	item {
		^items[this.value]
	}

	stringColor {
		^this.getProperty(\stringColor, Color.new)
	}
	stringColor_ { arg color;
		this.setProperty(\stringColor, color)
	}

	properties {
		^super.properties ++ #[\value, \font, \items, \stringColor]
	}

	defaultGetDrag {
		^this.value
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber;
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}
}



SCStaticTextBase : SCView {
	var <string, <font, <object, <>setBoth=true;

	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}

	string_ { arg argString;
		string = argString.asString;
		this.setProperty(\string, string)
	}
	align_ { arg align;
		this.setProperty(\align, align)
	}

	stringColor {
		^this.getProperty(\stringColor, Color.new)
	}
	stringColor_ { arg color;
		this.setProperty(\stringColor, color)
	}

	object_ { arg obj;
		object = obj;
		if (setBoth) { this.string = object.asString(80); };
	}

	properties {
		^super.properties ++ #[\string, \font, \stringColor]
	}
}

SCStaticText : SCStaticTextBase {
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.string = "The lazy brown fox";
		^v
	}
}


SCNumberBoxOld : SCStaticTextBase {
	var <> keyString, <>step=1, <>scroll_step=1;
	var <>typingColor, <>normalColor;
	var <>clipLo = -inf, <>clipHi = inf, hit, inc=1.0, <>scroll=true;
	var <>shift_scale = 100.0, <>ctrl_scale = 10.0, <>alt_scale = 0.1;

	getScale { |modifiers|
		^case
			 { modifiers & 131072 == 131072 } { shift_scale }
			 { modifiers & 262144 == 262144 } { ctrl_scale }
			 { modifiers & 524288 == 524288 } { alt_scale }
			 { 1 };
	}

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.value = 123.456;
		^v
	}

	init { arg argParent, argBounds;
		typingColor = Color.red;
		normalColor = Color.black;
		background = Color.white;
		parent = argParent.asView; // actual view
		this.prInit(parent.asView, argBounds.asRect, this.class.viewClass);
		argParent.add(this);//maybe window or viewadapter
	}

	increment { arg mul=1; this.valueAction = this.value + (step*mul); }
	decrement { arg mul=1; this.valueAction = this.value - (step*mul); }

	defaultKeyDownAction { arg char, modifiers, unicode;
		var zoom = this.getScale(modifiers);

		// standard chardown
		if (unicode == 16rF700, { this.increment(zoom); ^this });
		if (unicode == 16rF703, { this.increment(zoom); ^this });
		if (unicode == 16rF701, { this.decrement(zoom); ^this });
		if (unicode == 16rF702, { this.decrement(zoom); ^this });

		if ((char == 3.asAscii) || (char == $\r) || (char == $\n), { // enter key
			if (keyString.notNil, { // no error on repeated enter
				this.valueAction_(keyString.asFloat);
			});
			^this
		});
		if (char == 127.asAscii, { // delete key
			keyString = nil;
			this.string = object.asString;
			this.stringColor = normalColor;
			^this
		});
		if (char.isDecDigit || "+-.eE".includes(char), {
			if (keyString.isNil, {
				keyString = String.new;
				this.stringColor = typingColor;
			});
			keyString = keyString.add(char);
			this.string = keyString;
			^this
		});



		^nil		// bubble if it's an invalid key
	}

	value { ^object }
	value_ { arg val;
		keyString = nil;
		this.stringColor = normalColor;
		object = val !? { val.clip(clipLo, clipHi) };
		this.string = object.asString;
	}
	valueAction_ { arg val;
		var prev;
		prev = object;
		this.value = val !? { val.clip(clipLo, clipHi) };
		if (object != prev, { this.doAction });
	}

	boxColor {
		this.deprecated(thisMethod, SCView.findMethod(\background));
		^this.background;
	}
	boxColor_ { arg color;
		this.deprecated(thisMethod, SCView.findMethod(\background_));
		this.background_(color)
	}

	properties {
		^super.properties ++ #[\boxColor]
	}
	defaultGetDrag {
		^object.asFloat
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber;
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}

	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;
		hit = Point(x, y);
		if (scroll == true, { inc = this.getScale(modifiers) });
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount)
	}

	mouseMove { arg x, y, modifiers;
		var direction;
		if (scroll == true, {
			direction = 1.0;
				// horizontal or vertical scrolling:
			if ( (x - hit.x) < 0 or: { (y - hit.y) > 0 }) { direction = -1.0; };

			this.valueAction = (this.value + (inc * this.scroll_step * direction));
			hit = Point(x, y);
		});
		mouseMoveAction.value(this, x, y, modifiers);
	}
	mouseUp {
		inc=1
	}
}

SCListView : SCControlView {
	var <font, <items, <>enterKeyAction;

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.items = #["linear", "exponential", "sine", "welch", "squared", "cubed"];
		^v
	}

	init { arg argParent, argBounds;
		parent = argParent.asView; // actual view
			// call asView again because parent by this point might be a FlowView
		this.prInit(parent.asView, argBounds.asRect, this.class.viewClass);
		argParent.add(this);//maybe window or viewadapter
		this.items = []; // trick to draw right in scrollviews
	}

	item {
		^items[this.value]
	}
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}
	valueAction_ { arg val;
		this.setPropertyWithAction(\value, val);
	}

	defaultKeyDownAction { arg char, modifiers, unicode;
		var index;
		if (char == $ , { this.valueAction = this.value + 1; ^this });
		if (char == $\r, { this.enterKeyAction.value(this); ^this });
		if (char == $\n, { this.enterKeyAction.value(this); ^this });
		if (char == 3.asAscii, { this.enterKeyAction.value(this); ^this });
		if (unicode == 16rF700, { this.valueAction = this.value - 1; ^this });
		if (unicode == 16rF703, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF701, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF702, { this.valueAction = this.value - 1; ^this });
		if (char.isAlpha, {
			char = char.toUpper;
			index = items.detectIndex( { |item| item.asString.at(0).toUpper >= char });
			if (index.notNil, {
				this.valueAction = index
			});
			^this
		});
		^nil		// bubble if it's an invalid key
	}
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	items_ { arg array;
		items = array;
		this.setProperty(\items, items);
	}

	colors_ { arg incolors;
		this.setProperty(\itemColors, incolors);
	}

	colors {
		^this.getProperty(\itemColors, {Color.new}!items.size);
	}

	stringColor {
		^this.getProperty(\stringColor, Color.new)
	}
	stringColor_ { arg color;
		this.setProperty(\stringColor, color)
	}

	selectedStringColor {
		^this.getProperty(\selectedStringColor, Color.new)
	}
	selectedStringColor_ { arg color;
		this.setProperty(\selectedStringColor, color)
	}

	hiliteColor {
		^this.getProperty(\hiliteColor, Color.new)
	}
	hiliteColor_ { arg color;
		this.setProperty(\hiliteColor, color)
	}

	properties {
		^super.properties ++ #[\value, \font, \items, \stringColor, \align, \itemColors]
	}

	defaultGetDrag {
		^this.value
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber;
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}
}


SCDragView : SCStaticTextBase {

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.object = \something;
		^v
	}
	defaultGetDrag {
		^object
	}
}

SCDragSource : SCDragView {

}

SCDragSink : SCDragView {
	defaultCanReceiveDrag { ^true;	}
	defaultReceiveDrag {
		this.object = currentDrag;
		this.doAction;
	}
}

SCDragBoth : SCDragSink {
	defaultGetDrag { ^object }
}


SCUserView : SCView {
	var <>drawFunc;
//	var <>mouseBeginTrackFunc, <>mouseTrackFunc, <>mouseEndTrackFunc;
	var < clearOnRefresh = true;
	var < drawingEnabled = true;

	init { |argParent, argBounds|
		super.init(argParent, argBounds);
	}

	draw {
		drawFunc.value(this) ;
	}

	clearDrawing {
			this.setProperty(\clearDrawing);
	}

	mousePosition {
		this.deprecated(thisMethod);
		^this.getProperty(\mousePosition, Point.new)
	}

	clearOnRefresh_ { |bool|
		clearOnRefresh = bool;
		this.setProperty(\clearOnRefresh, bool);			}

	animate_ { |bool|
		this.setProperty(\animate, bool);
	}

	frame {
		^this.getProperty(\frame);
	}

	frameRate {
		^this.getProperty(\frameRate);
	}

	drawingEnabled_ { |bool|
		drawingEnabled = bool;
		this.setProperty(\drawingEnabled, bool);	}

	/* backwards compatibility */
	keyDownFunc_ { |action|
		"SCUserView:keyDownFunc deprecated, use SCUserView:keyDownAction".warn;
		keyDownAction = action;
	}

	keyDownFunc {
		"SCUserView:keyDownFunc deprecated, use SCUserView:keyDownAction".warn;
		^keyDownAction
	}

	mouseBeginTrackFunc_ { |action|
		"SCUserView:mouseBeginTrackFunc deprecated, use SCUserView:mouseDownAction".warn;
		mouseDownAction = action;
	}
	mouseBeginTrackFunc {
		"SCUserView:mouseBeginTrackFunc deprecated, use SCUserView:mouseDownAction".warn;
		^mouseDownAction;
	}

	mouseTrackFunc_ { |action|
		"SCUserView:mouseTrackFunc deprecated, use SCUserView:mouseMoveAction".warn;
		mouseMoveAction = action;
	}
	mouseTrackFunc {
		"SCUserView:mouseTrackFunc deprecated, use SCUserView:mouseMoveAction".warn;
		^mouseMoveAction;
	}

	mouseEndTrackFunc_ { |action|
		"SCUserView:mouseEndTrackFunc deprecated, use SCUserView:mouseUpAction".warn;
		mouseUpAction = action;
	}

	mouseEndTrackFunc {
		"SCUserView:mouseEndTrackFunc deprecated, use SCUserView:mouseUpAction".warn;
		^mouseUpAction;
	}

}


//
//SCFuncUserView : SCUserView {
//	var <>keyDownFunc, <>drawFunc;
//	var <>mouseBeginTrackFunc, <>mouseTrackFunc, <>mouseEndTrackFunc;
//
//	draw { drawFunc.value(this) }
//	mouseBeginTrack { arg x, y, modifiers;
//		mouseBeginTrackFunc.value(this, x, y, modifiers);
//	}
//	mouseTrack { arg x, y, modifiers;
//		mouseTrackFunc.value(this, x, y, modifiers);
//	}
//	mouseEndTrack { arg x, y, modifiers;
//		mouseEndTrackFunc.value(this, x, y, modifiers);
//	}
//	keyDown { arg key, modifiers, unicode;
//		keyDownFunc.value(this, key, modifiers, unicode)
//	}
//}

//by jt v.0.22
SCMultiSliderView : SCView {

	var <>metaAction;
//	var <>mouseUpAction;
	var <>size ;
	var <gap;
	var <editable = true;
	var <elasticMode = 0;

	draw {}
	mouseBeginTrack { arg x, y, modifiers;}
	mouseTrack { arg x, y, modifiers; 	}
	mouseEndTrack { arg x, y, modifiers;
		mouseUpAction.value(this);
	}
	properties {
		^super.properties ++ #[\value, \thumbSize, \fillColor, \strokeColor, \xOffset, \x, \y, \showIndex, \drawLines, \drawRects, \selectionSize, \startIndex, \referenceValues, \thumbWidth, \absoluteX, \isFilled, \step, \elasticResizeMode]
	}



	elasticMode_ { arg mode;
		elasticMode =mode;
		this.setProperty(\elasticResizeMode, mode);
	}

	step_ { arg stepSize;
		this.setPropertyWithAction(\step, stepSize);
	}

	step {
		^this.getProperty(\step)
	}

	value { //returns array
		^this.getProperty(\value, Array.newClear(this.size))
	}
	value_ { arg val;
		this.size = val.size;
		this.setProperty(\value, val)
	}

	valueAction_ { arg val;
		this.size = val.size;
		this.setPropertyWithAction(\value, val);
	}

	reference { //returns array
		^this.getProperty(\referenceValues, Array.newClear(this.size))
	}
	reference_ { arg val;
		//this.size = val.size;
		this.setProperty(\referenceValues, val)
	}
	index { //returns selected index
		^this.getProperty(\x)
	}
	index_ { arg inx;
		this.setProperty(\x, inx)
	}
	isFilled_ { arg abool;
		this.setProperty(\isFilled, abool);
	}
	xOffset_ { arg aval;
		this.setProperty(\xOffset, aval);
	}
	gap_ { arg inx;
		gap = inx;
		this.setProperty(\xOffset, inx)
	}

	startIndex_ { arg val; this.setProperty( \startIndex, val )}

	selectionSize {
		^this.getProperty(\selectionSize)
	}
	selectionSize_ { arg aval;
		this.setProperty(\selectionSize, aval)
	}
	currentvalue { //returns value of selected index
		^this.getProperty(\y)
	}
	fillColor_ { arg acolor;
		this.setProperty(\fillColor, acolor)
	}
	strokeColor_ { arg acolor;
		this.setProperty(\strokeColor, acolor)
	}
	colors_ { arg strokec, fillc;
		this.strokeColor_(strokec);
		this.fillColor_(fillc);
	}
	currentvalue_ { arg iny;
		this.setProperty(\y, iny)
	}
	showIndex_ { arg abool;
		this.setProperty(\showIndex, abool)
		}
	drawLines { arg abool;
		this.setProperty(\drawLines, abool)
	}
	drawLines_ { arg abool;
		this.drawLines(abool)
	}
	drawRects_ { arg abool;
		this.setProperty(\drawRects, abool)
	}
	readOnly_ { arg val;
		editable = val.not;
		this.setProperty(\readOnly, val);
	}
	editable_ { arg val;
		editable = val;
		this.setProperty(\readOnly, editable.not);
	}
	thumbSize_ { arg val;
		this.setProperty(\thumbSize, val)
	}
	indexThumbSize_ { arg val;
		this.setProperty(\indexThumbSize, val)
	}
	valueThumbSize_ { arg val;
		this.setProperty(\valueThumbSize, val)
	}
	indexIsHorizontal_ { arg val;
		this.setProperty(\isHorizontal, val);
	}
	defaultCanReceiveDrag {	^true; }
	defaultReceiveDrag {
		if(currentDrag.at(0).isSequenceableCollection, {
			this.value_(currentDrag.at(0));
			this.reference_(currentDrag.at(1));
		}, {
			this.value_(currentDrag);
		});
	}
	defaultGetDrag {
		var setsize, vals, rvals, outval;
		rvals = this.reference;
		vals = this.value;
		if(this.selectionSize > 1, {
			vals = vals.copyRange(this.index, this.selectionSize + this.index);});
		if(rvals.isNil, {
			^vals
		}, {
		if(this.selectionSize > 1, {
			rvals = rvals.copyRange(this.index, this.selectionSize + this.index);});
			outval = outval.add(vals);
			outval = outval.add(rvals);
		});
		^outval
	}

	defaultKeyDownAction { arg key, modifiers, unicode;
		//modifiers.postln; 16rF702
		if (unicode == 16rF703, { this.index = this.index + 1; ^this });
		if (unicode == 16rF702, { this.index = this.index - 1; ^this });
		if (unicode == 16rF700, { this.gap = this.gap + 1; ^this });
		if (unicode == 16rF701, { this.gap = this.gap - 1; ^this });
		^nil		// bubble if it's an invalid key
	}

	*paletteExample { arg parent, bounds;
		var example;
		example = this.new(parent, bounds).valueThumbSize_(2);
		^example
	}

	doMetaAction {
		metaAction.value(this)
	} //on ctrl click
}

SCEnvelopeView : SCView {
	var connection, <>allConnections, doOnce;
//	var <>mouseUpAction;
	var <>items;
	var < fixedSelection = false;
	var <>metaAction;
	var <>size ;
	var <editable = true;
	var <curves = \linear;
	var <gridOn, <gridColor, <grid;

	init { arg argParent, argBounds;
		super.init(argParent, argBounds);
		gridOn = false;
		gridColor = Color(0, 0, 0.8, 0.3);
		grid = Point(0.1, 0.1);
	}

	draw {}
	mouseBeginTrack { arg x, y, modifiers;}
	mouseTrack { arg x, y, modifiers; 	}
	mouseEndTrack { arg x, y, modifiers;
		mouseUpAction.value(this);
	}

	grid_ { arg point;
		this.setProperty(\setGrid, [point.x, point.y]);
		grid = point;
	}

	gridOn_ { arg bool;
		this.setProperty(\showGrid, bool);
		gridOn = bool;
	}

	gridColor_ { arg color;
		this.setProperty(\setGridColor, color);
		gridColor = color;
	}

	properties {
		^super.properties ++ #[\value, \thumbSize, \fillColor, \strokeColor, \xOffset, \x, \y, \showIndex, \drawLines, \drawRects, \selectionSize, \startIndex, \thumbWidth, \absoluteX, \isFilled, \step, \setCurve, \setCurves, \showGrid, setGridColor, \setGrid]
	}

	step_ { arg stepSize;
		this.setPropertyWithAction(\step, stepSize);
	}

	step {
		^this.getProperty(\step)
	}

	valueAction_ { arg val;
		this.value_(val);
		this.doAction;
	}

	currentvalue { //returns value of selected index
		^this.getProperty(\y)
	}
	currentvalue_ { arg iny;
		this.setProperty(\y, iny)
	}

	strokeColor_ { arg acolor;
		this.setProperty(\strokeColor, acolor)
	}
	colors_ { arg strokec, fillc;
		this.strokeColor_(strokec);
		this.fillColor_(fillc);
	}

	drawLines { arg abool;
		this.setProperty(\drawLines, abool)
	}
	drawLines_ { arg abool;
		this.drawLines(abool)
	}
	drawRects_ { arg abool;
		this.setProperty(\drawRects, abool)
	}

	defaultCanReceiveDrag {	^true; }

    defaultKeyDownAction { arg key, modifiers, unicode;
	    	(modifiers==10617090).if { // shift key
	       		(this.index > 1.neg).if {
	       	 	if (unicode == 16rF703, { this.x = this.value[0][this.index] + this.step; ^this });
	        	if (unicode == 16rF702, { this.x = this.value[0][this.index] - this.step; ^this });
	        	};
	        } {
	       		if (unicode == 16rF703, { this.selectIndex((this.index+1)%this.size) ^this });	        	if (unicode == 16rF702, { this.selectIndex((this.index-1)%this.size) ^this });
	        };
	       	(this.index > 1.neg).if {
	     	   if (unicode == 16rF700, { this.y = this.value[1][this.index] + this.step; ^this });	     	   if (unicode == 16rF701, { this.y = this.value[1][this.index] - this.step; ^this });
	     	};
        ^nil        // bubble if it's an invalid key    }

	doMetaAction {
		metaAction.value(this)
	} //on ctrl click

	value_ { arg val;
		if(val.at(1).size != val.at(0).size, {
			// otherwise its a fatal crash
			Error("SCEnvelopeView got mismatched times/levels arrays").throw;
		});
		this.size = val.at(0).size;
		this.setProperty(\value, val)
	}
	setString { arg index, astring;
		//items = items.add(astring);
		this.setProperty(\string, [index, astring])
	}

	strings_ { arg astrings;
		astrings.do( { arg str, i;
			this.setString(i, str);
		});
	}
	value {
		var ax, ay, axy;
		ax = Array.newClear(this.size);
		ay = Array.newClear(this.size);
		axy = Array.with(ax, ay);
		^this.getProperty(\value, axy)
	}

	setThumbHeight { arg index, height;
		this.setProperty(\thumbHeight, [index, height]);
	}
	thumbHeight_ { arg height;
		this.setThumbHeight(-1, height);
	}
	setThumbWidth { arg index, width;
		this.setProperty(\thumbWidth, [index, width]);
	}
	thumbWidth_ { arg width;
		this.setThumbWidth(-1, width);
	}
	setThumbSize { arg index, size;
		this.setProperty(\thumbSize, [index, size]);
	}
	thumbSize_ { arg size;
		this.setThumbSize(-1, size);
	}
	setFillColor { arg index, color;
		this.setProperty(\fillColor, [index, color]);
	}
	fillColor_ { arg color;
		this.setFillColor(-1, color);
	}

	curves_ { arg inCurves;
		var curveNumbers;
		var shapeNum, curveNum=0;
		curves = inCurves;
		if(curves.isKindOf(Array)) {
			curveNumbers = curves.collect { |it|
				shapeNum = this.shapeNumber(it);
				if(shapeNum == 5) {
					curveNum = it;
				} {
					curveNum = 0;
				};
				[shapeNum, curveNum]
			};
//			this.debug(curveNumbers);
			this.prSetCurves(curveNumbers);
		} {
			shapeNum = this.shapeNumber(curves);
				if(shapeNum == 5) {
					curveNum = curves;
				} {
					curveNum = 0;
				};
			this.prSetCurve([-1, shapeNum, curveNum]);
		}
	}

	setEnv { arg env, minValue, maxValue, minTime, maxTime;
		var times, levels;
		var spec;

		times = Array.newClear(env.times.size + 1);
		times[0] = 0;
		for(1, env.times.size, { arg i;
			times[i] = times[i-1] + env.times[i-1];
		});
		maxTime = maxTime ? times.last;
		minTime = minTime ? 0;
		levels = env.levels;
		minValue = minValue ? levels.minItem;
		maxValue = maxValue ? levels.maxItem;

		levels = levels.linlin(minValue, maxValue, 0, 1);
		times = times.linlin(minTime, maxTime, 0, 1);

		this.value_([times.asFloat, levels.asFloat]);
		this.curves_(env.curves);

	}

	editEnv { arg env, minValue, maxValue, duration;
		var vals, levels, times, lastTime = 0;
		vals = this.value;
		levels = vals[0].linlin(0, 1, minValue, maxValue);
		times = vals[1].collect { |it|
			var out;
			out = it - lastTime;
			lastTime = it;
			out
		};
		env.times_(times);
		env.levels_(levels);
		env.curves_(this.curves);
	}

	asEnv { arg env, minValue, maxValue, duration;
		var vals, levels, times, lastTime = 0;
		vals = this.value;
		levels = vals[0].linlin(0, 1, minValue, maxValue);
		times = vals[1].collect { |it|
			var out;
			out = it - lastTime;
			lastTime = it;
			out
		};
		env.times_(times);
		env.levels_(levels);
		env.curves_(this.curves);
		^Env(levels, times, this.curves);
	}

	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds).setEnv(Env([0.1, 0.3, 0.4, 0.01], [0.3, 1, 3]));
	}

	prSetCurves { |arr|
		this.setProperty(\setCurves, arr);
	}

	prSetCurve { |arr|
//		this.debug([\setCurves, arr]);
		this.setProperty(\setCurve, arr);
	}

	shapeNumber { arg shapeName;
		var shape;
		shape = Env.shapeNames.at(shapeName);
		if (shape.notNil) { ^shape } {^5};
	}

	connect { arg from, aconnections;
		this.setProperty(\connect, [from, aconnections.asFloat]);
	}

	select { arg index; //this means no refresh;
		this.setProperty(\setIndex, index);
	}
	selectIndex { arg index; //this means that the view will be refreshed
		this.setProperty(\selectedIndex, index);
	}
	x { 						//returns selected x
		^this.getProperty(\x);
	}
	y {
		^this.getProperty(\y);
	}
	x_ { arg ax;
		this.setProperty(\x, ax);
	}
	y_ { arg ay;
		this.setProperty(\y, ay);
	}
	index {
		^this.getProperty(\selectedIndex)
	}
	lastIndex {
		^this.getProperty(\lastIndex)
	}

	setEditable { arg index, boolean;
		this.setProperty(\editable, [index, boolean]);
	}

	editable_ { arg boolean;
		this.setEditable(-1, boolean);
	}

	selectionColor_ { arg acolor;
		this.setProperty(\selectionColor, acolor)
	}
	defaultGetDrag {
		^this.value
	}
	defaultReceiveDrag {
		if(currentDrag.isString, {
			this.addValue;
			items = items.insert(this.lastIndex + 1, currentDrag);
			this.strings_(items);
		}, {
			this.value_(currentDrag);
		});
	}

	addValue { arg xval, yval;
		var arr, arrx, arry, aindx;
		arr = this.value;
		arrx = arr@0;
		arry = arr@1;
		arrx.do { |val, i|
			if(val>xval and: aindx.isNil) {
				aindx = i;
			}
		};
//		this.debug(aindx);
		arrx = arrx.insert(aindx , xval);
		arry = arry.insert(aindx , yval);
		this.value_([arrx, arry]);
	}

	fixedSelection_ { arg bool;
		fixedSelection =  bool;
		this.setProperty(\setFixedSelection, bool);
	}
}

//including keydown by Heiko:
// bug fixed in SCRangeSlider-receiveDrag
// bug fixed in SCView-getPropertyList
// allowing children in SCContainerView and parent in SCView to be accessed
// globalKeyDownAction as classvar for all Views
// KeyDownAction for all Views
// bug fixed in SCNumberBox-defaultKeyDownAction
// passing keydown up the tree
// consuming keydown in function
// added keyup
// and SCMultiSlider by jan t.
// added customizable drag handlers, cx
