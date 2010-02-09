
SCView {  // abstract class
	classvar <>currentDrag, <>currentDragString;
	classvar <>globalKeyDownAction, <>globalKeyUpAction;

	var dataptr, <parent, <>action, <background;
	var <>mouseDownAction, <>mouseUpAction, <>mouseOverAction, <>mouseMoveAction;
	var <>keyDownAction, <>keyUpAction, <>keyTyped, <> keyModifiersChangedAction;
	var <>beginDragAction,<>canReceiveDragHandler,<>receiveDragHandler;
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
		this.prInit(parent.asView, argBounds.asRect,this.class.viewClass);
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
	hasFocus{
		_SCView_HasFocus
		^this.primitiveFailed
	}

	focusColor_{|color|
		this.setProperty(\focusColor, color);
	}

	focusColor{
		^this.getProperty(\focusColor, Color.new);
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
		if(dataptr.notNil,{
			parent.prRemoveChild(this);
			this.prRemove;
			this.prClose;
		},{
			"SCView-remove : this view already removed.".debug(this);
		});
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
	mouseDown{arg x, y, modifiers, buttonNumber, clickCount;
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount);
	}
	mouseUp{arg x, y, modifiers;
		mouseUpAction.value(this, x, y, modifiers);
	}
	mouseMove{arg x, y, modifiers;
		mouseMoveAction.value(this, x, y, modifiers);
	}
	mouseOver{arg x, y;
		mouseOverAction.value(this, x, y);
	}

	keyDown { arg char, modifiers, unicode,keycode;
		globalKeyDownAction.value(this, char, modifiers, unicode, keycode);
		this.handleKeyDownBubbling(this, char, modifiers, unicode, keycode);
	}

	keyModifiersChanged{arg modifiers;
		this.handleKeyModifiersChangedBubbling(this,modifiers)
	}

	handleKeyModifiersChangedBubbling { arg view, modifiers;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyModifiersChangedAction.isNil) {
//			this.defaultKeyDownAction(char,modifiers,unicode,keycode);
			result = nil;
		}{
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
			result = this.defaultKeyDownAction(char,modifiers,unicode,keycode);
		}{
			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	// sc.solar addition
	keyUp { arg char, modifiers, unicode,keycode;
		this.keyTyped = char;
		// always call global keydown action first
		globalKeyUpAction.value(this, char, modifiers, unicode, keycode);
		this.handleKeyUpBubbling(this, char, modifiers, unicode, keycode);
	}
	defaultKeyUpAction { ^nil }
	handleKeyUpBubbling { arg view, char, modifiers,unicode,keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyUpAction.isNil) {
			result = this.defaultKeyUpAction(char,modifiers,unicode,keycode);
		}{
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
		}{
			this.defaultGetDrag
		};
		currentDragString = currentDrag.asCompileString;
	}
	defaultGetDrag { ^nil }
	canReceiveDrag {
		^if(canReceiveDragHandler.notNil,{ canReceiveDragHandler.value(this) },{ this.defaultCanReceiveDrag })
	}
	defaultCanReceiveDrag { ^false }
	receiveDrag {|x,y|
		if(receiveDragHandler.notNil,{ receiveDragHandler.value(this, x, y) },{ this.defaultReceiveDrag(x,y) });
		currentDrag = currentDragString = nil;
	}

	// get the view parent tree up to the SCTopView
	getParents {
		var parents, view;
		view = this;
		parents = List.new;
		while({(view = view.parent).notNil},{ parents.add(view)});
		^parents
	}

	doAction {
		action.value(this);
	}

	properties {
		^#[\bounds, \visible, \enabled, \canFocus, \resize, \background,
				\minWidth,\maxWidth,\minHeight,\maxHeight,\focusColor]
	}
	getPropertyList {
		^this.properties.collect({ arg name;
			[name, this.perform(name)]
		});
	}
	setPropertyList { arg list;
		list.do({ arg item;
			var name, value;
			#name, value = item;
			this.perform(name.asSetter, value);
		});
	}

	// private
	prInit { arg argParent, argBounds,argViewClass;
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
		^this.getProperty(\absoluteBounds,Rect.new)
	}
}

SCContainerView : SCView { // abstract class
	var <children, <decorator, < relativeOrigin = true;

	add { arg child;
		children = children.add(child);
		if (decorator.notNil, { decorator.place(child); });
	}

	init { |argParent, argBounds|
		super.init(argParent, argBounds);
			// if user changed default relativeOrigin to true,
			// the client would be out of sync with the cocoa widget
			// without resetting the view property
		this.relativeOrigin = relativeOrigin;
	}

	removeAll {
		children.copy.do {|child| child.remove };
	}

	relativeOrigin_{ |bool|
		relativeOrigin = bool;
		this.setProperty(\relativeOrigin, bool);
	}

	addFlowLayout { |margin, gap|
		this.relativeOrigin.if
			{this.decorator_( FlowLayout( this.bounds.moveTo(0,0), margin, gap ) )}
			{this.decorator_( FlowLayout( this.bounds, margin, gap ) )};
		^this.decorator;
		}

	decorator_ {|decor|
		if(relativeOrigin, {
			decor.bounds = decor.bounds.moveTo(0, 0);
			decor.reset;
		});
		decorator = decor;
	}

	prRemoveChild { arg child;
		children.remove(child);
		// ... decorator should re-place all
	}

	//bounds_  ... replace all

	prClose {
		super.prClose;
		children.do({ arg item; item.prClose });
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

	findWindow{
		SCWindow.allWindows.do {|win|
			if(win.view == this){
				^win
			}
		}
	}

	/* construction mode */

	constructionGrid_{ arg point;
		this.setProperty( \constructionGrid, point );
	}

	constructionGrid {
		^this.getProperty( \constructionGrid, Point.new );
	}

	enableConstructionGrid_{arg flag;
		this.setProperty( \enableConstructionGrid, flag );
	}

	enableConstructionGrid{
		^this.getProperty( \enableConstructionGrid );
	}

	//private called from lang
	setConstructionMode {|flag|
		this.setProperty( \setConstructionMode, flag )
	}

	defaultReceiveDrag{|x,y|
		var win, view;
		win = this.findWindow;
		view = currentDrag.paletteExample(win, Rect(x,y,140,24)).enabled_(false);
		view.keyDownAction_({|view, char, modifiers, unicode, keycode|
			if(keycode == 51){
				view.remove;
			}
		});
	}
}

SCScrollTopView : SCTopView {
	var <autohidesScrollers = true, <hasHorizontalScroller = true, <hasVerticalScroller = true;
	var <autoScrolls = true;

	autohidesScrollers_{|bool|
		autohidesScrollers = bool;
		this.setProperty(\setAutohidesScrollers, bool);
	}

	hasHorizontalScroller_{|bool|
		hasHorizontalScroller = bool;
		this.setProperty(\setHasHorizontalScroller, bool);
	}

	hasVerticalScroller_{|bool|
		hasVerticalScroller = bool;
		this.setProperty(\setHasVerticalScroller, bool);
	}

	visibleOrigin_ { arg point;
		this.setProperty( \clipViewOrigin, point );
	}

	visibleOrigin { ^this.getProperty( \clipViewOrigin, Point.new );}

	autoScrolls_ {|bool|
		autoScrolls = bool;
		this.setProperty(\setAutoScrolls, bool);
	}

	innerBounds {
		^this.getProperty(\innerBounds, Rect.new)
	}

	bounds {
		var	bounds = this.absoluteBounds;
		this.getParents.do({ |parent|
			(parent.tryPerform(\relativeOrigin) == true).if({
				bounds = bounds.moveBy(parent.bounds.left.neg, parent.bounds.top.neg)
			}, {
				^bounds
			});
		});
		^bounds
	}

//	handleKeyModifiersChangedBubbling { arg view, modifiers;
//		var result;
//		// nil from keyDownAction --> pass it on
//		if (keyModifiersChangedAction.isNil) {
//			result = nil;
//		}{
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
//			result = this.defaultKeyDownAction(char,modifiers,unicode,keycode);
//		}{
//			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
//		};
//		if(result.isNil) {
//			// call keydown action of parent view
//			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
//		};
//	}
//
//	handleKeyUpBubbling { arg view, char, modifiers,unicode,keycode;
//		var result;
//		// nil from keyDownAction --> pass it on
//		if (keyUpAction.isNil) {
//			result = this.defaultKeyUpAction(char,modifiers,unicode,keycode);
//		}{
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

		relativeOrigin = false; // scroll views are never relative, although they really are ;-)
	}

	relativeOrigin_ {  }

	handleKeyModifiersChangedBubbling { arg view, modifiers;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyModifiersChangedAction.isNil) {
			result = nil;
		}{
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
			result = this.defaultKeyDownAction(char,modifiers,unicode,keycode);
		}{
			result = keyDownAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {
			// call keydown action of parent view
			parent.handleKeyDownBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	handleKeyUpBubbling { arg view, char, modifiers,unicode,keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyUpAction.isNil) {
			result = this.defaultKeyUpAction(char,modifiers,unicode,keycode);
		}{
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
			// layout views don't recognize relativeOrigin in the backend
		if(key != \relativeOrigin) {
			super.setProperty(key, value);
		};
	}
}

SCHLayoutView : SCLayoutView {}
SCVLayoutView : SCLayoutView {}


SCControlView : SCView { // abstract class
}

SCSliderBase : SCControlView {

	knobColor {
		^this.getProperty(\knobColor, Color.new)
	}
	knobColor_ { arg color;
		this.setProperty(\knobColor, color)
	}

	step_ { arg stepSize;
		this.setPropertyWithAction(\step, stepSize);
	}
	step {
		^this.getProperty(\step)
	}

	properties {
		^super.properties ++ #[\knobColor, \step]
	}

}

SCSlider : SCSliderBase
{
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

	defaultKeyDownAction { arg char, modifiers, unicode,keycode;
		// standard keydown
		if (char == $r, { this.valueAction = 1.0.rand; ^this });
		if (char == $n, { this.valueAction = 0.0; ^this });
		if (char == $x, { this.valueAction = 1.0; ^this });
		if (char == $c, { this.valueAction = 0.5; ^this });
		if (char == $], { this.increment; ^this });
		if (char == $[, { this.decrement; ^this });
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
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
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
		^nil		// bubble if it's an invalid key
	}
	defaultGetDrag { ^Point(this.lo, this.hi) }
	defaultCanReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	defaultReceiveDrag {
		// changed to x,y instead of lo, hi
		this.lo = currentDrag.x;
		this.hi = currentDrag.y;
		this.doAction
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
		}{
			this.action = currentDrag;
		};
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


SCNumberBox : SCStaticTextBase {
	var <> keyString, <>step=1;
	var <>typingColor, <>normalColor;
	var <>clipLo = -inf, <>clipHi = inf, hit, inc=1.0, <>scroll=true, <>shift_step=0.1, <>ctrl_step=10;

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
		this.prInit(parent.asView, argBounds.asRect,this.class.viewClass);
		argParent.add(this);//maybe window or viewadapter
	}

	increment { this.valueAction = this.value + step; }
	decrement { this.valueAction = this.value - step; }

	defaultKeyDownAction { arg char, modifiers, unicode;
		// standard chardown
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
		if ((char == 3.asAscii) || (char == $\r) || (char == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
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
		hit = Point(x,y);
		if (scroll == true, {
			inc = 1.0;
			case
				{ modifiers & 131072 == 131072 } // shift defaults to step x 0.1
					{ inc = shift_step }
				{ modifiers & 262144 == 262144 } // control defaults to step x 10
					{ inc = ctrl_step };
		});
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount)
	}

	mouseMove { arg x, y, modifiers;
		var direction;
		if (scroll == true, {
			direction = 1.0;
				// horizontal or vertical scrolling:
			if ( (x - hit.x) < 0 or: { (y - hit.y) > 0 }) { direction = -1.0; };

			this.valueAction = (this.value + (inc * this.step * direction));
			hit = Point(x, y);
		});
		mouseMoveAction.value(this, x, y, modifiers);
	}
}
