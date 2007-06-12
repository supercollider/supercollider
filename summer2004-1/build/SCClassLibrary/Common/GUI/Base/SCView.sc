
SCView {  // abstract class
	classvar <>currentDrag;
	classvar <>globalKeyDownAction, <>globalKeyUpAction;

	var dataptr, <parent, <>action, <background, <>keyDownAction, <>keyUpAction, <>keyTyped;
	var <>beginDragAction;
	
	*new { arg parent, bounds;
		^super.new.init(parent, bounds);
	}
	*viewClass { ^this }
	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds);
	}
	
	init { arg argParent, argBounds;
		parent = argParent.asView; // actual view
		this.prInit(parent, argBounds.asRect,this.class.viewClass);
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

	id {
		^this.getProperty(\id)
	}
	id_ { arg id;
		this.setProperty(\id, id)
	}
	
	refresh {
		_SCView_Refresh
		^this.primitiveFailed
	}
	findByID { arg id;
		_SCView_FindByID
		^this.primitiveFailed
	}
	remove {
		if(dataptr.notNil,{
			parent.prRemoveChild(this);
			this.prRemove;
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

	keyDown { arg char, modifiers, unicode,keycode;
		globalKeyDownAction.value(this, char, modifiers, unicode, keycode); 
		this.handleKeyDownBubbling(this, char, modifiers, unicode, keycode);
	}
	defaultKeyDownAction { ^nil }
	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
		var result;
		// nil from keyDownAction --> pass it on
		if (keyDownAction.isNil) {
			this.defaultKeyDownAction(char,modifiers,unicode,keycode);
			result = nil;
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
			this.defaultKeyUpAction(char,modifiers,unicode,keycode);
			result = nil;
		}{
			result = keyUpAction.value(view, char, modifiers, unicode, keycode);
		};
		if(result.isNil) {  
			// call keydown action of parent view
			parent.handleKeyUpBubbling(view, char, modifiers, unicode, keycode);
		};
	}

	canReceiveDrag {
		^false;
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
				\minWidth,\maxWidth,\minHeight,\maxHeight]
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
	prClose { dataptr = nil; }
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
	
	*importDrag { 
		// this is called when an NSString is the drag object.
		// we compile it to an SCObject.
		currentDrag = currentDrag.interpret;
	}
	*exportDrag {
		// this is called when an SCObject is dragged onto a text window
		^currentDrag.asCompileString
	}
}

SCContainerView : SCView { // abstract class
	var <children, <>decorator;
			
	add { arg child;
		children = children.add(child);
		if (decorator.notNil, { decorator.place(child); });
	}
	
	prRemoveChild { arg child;
		children.remove(child);
		// ... decorator replace all
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
	handleKeyDownBubbling { arg view, char, modifiers, unicode, keycode;
		keyDownAction.value(view, char, modifiers, unicode, keycode);
	}
	handleKeyUpBubbling { arg view, char, modifiers, unicode, keycode;
		keyUpAction.value(view, char, modifiers, unicode, keycode);
	}
}

SCLayoutView : SCContainerView {
	properties { ^super.properties ++ #[\spacing] }
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
	
	increment { ^this.valueAction = this.value + this.bounds.width.reciprocal }
	decrement { ^this.valueAction = this.value - this.bounds.width.reciprocal }
	
	defaultKeyDownAction { arg char, modifiers, unicode,keycode;
		// standard keydown
		if (char == $r, { this.valueAction = 1.0.rand; });
		if (char == $n, { this.valueAction = 0.0; });
		if (char == $x, { this.valueAction = 1.0; });
		if (char == $c, { this.valueAction = 0.5; });
		if (char == $], { this.increment; ^this });
		if (char == $[, { this.decrement; ^this });
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
	}
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.value, 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.valueAction = currentDrag;
		currentDrag = nil;
	}
}

SCKnob : SCSlider
{
}

SCRangeSlider : SCSliderBase {

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.lo = 0.2;
		v.hi = 0.7;
	}
	
	lo {
		^this.getProperty(\lo)
	}
	lo_ { arg val;
		this.setPropertyWithAction(\lo, val);
	}	
	hi {
		^this.getProperty(\hi)
	}
	hi_ { arg val;
		this.setPropertyWithAction(\hi, val);
	}	
	range {
		^this.getProperty(\range)
	}
	range_ { arg val;
		this.setPropertyWithAction(\range, val);
	}	

	properties {
		^super.properties ++ #[\lo, \hi]
	}
	
	increment { 
		var inc, val; 
		inc = this.bounds.width.reciprocal;
		val = this.hi + inc;
		if (val > 1, {
			inc = 1 - this.hi;
			val = 1;
		});
		this.lo = this.lo + inc;
		this.hi = val;
	}
	decrement { 
		var inc, val; 
		inc = this.bounds.width.reciprocal;
		val = this.lo - inc;
		if (val < 0, {
			inc = this.lo;
			val = 0;
		});
		this.lo = val;
		this.hi = this.hi - inc;
	}

	defaultKeyDownAction { arg char, modifiers, unicode;
		var a, b;
		// standard keydown
		if (char == $r, { 
			a = 1.0.rand; 
			b = 1.0.rand; 
			this.lo = min(a, b);
			this.hi = max(a, b);
		});
		if (char == $n, { this.lo = 0.0; this.hi = 0.0; });
		if (char == $x, { this.lo = 1.0; this.hi = 1.0; });
		if (char == $c, { this.lo = 0.5; this.hi = 0.5; });
		if (char == $a, { this.lo = 0.0; this.hi = 1.0; });
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
	}
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			Point(this.lo, this.hi);, 
		{
			beginDragAction.value(this);
		});
	}	
	canReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	receiveDrag {
		// changed to x,y instead of lo, hi
		this.lo = currentDrag.x;
		this.hi = currentDrag.y;
		currentDrag = nil;
	}
}

SC2DSlider : SCSliderBase {
	x {
		^this.getProperty(\x)
	}
	x_ { arg val;
		this.setPropertyWithAction(\x, val);
	}	
	y {
		^this.getProperty(\y)
	}
	y_ { arg val;
		this.setPropertyWithAction(\y, val);
	}	
	properties {
		^super.properties ++ #[\x, \y]
	}

	incrementY { ^this.y = this.y + this.bounds.height.reciprocal }
	decrementY { ^this.y = this.y - this.bounds.height.reciprocal }
	incrementX { ^this.x = this.x + this.bounds.width.reciprocal }
	decrementX { ^this.x = this.x - this.bounds.width.reciprocal }

	defaultKeyDownAction { arg char, modifiers, unicode,keycode;
		// standard keydown
		if (char == $r, { this.x = 1.0.rand; this.y = 1.0.rand; ^this });
		if (char == $n, { this.x = 0.0; this.y = 0.0; ^this });
		if (char == $x, { this.x = 1.0; this.y = 1.0; ^this });
		if (char == $c, { this.x = 0.5; this.y = 0.5; ^this });
		if (unicode == 16rF700, { this.incrementY; ^this });
		if (unicode == 16rF703, { this.incrementX; ^this });
		if (unicode == 16rF701, { this.decrementY; ^this });
		if (unicode == 16rF702, { this.decrementX; ^this });
	}
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			Point(this.x, this.y), 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	receiveDrag {
		this.x = currentDrag.x;
		this.y = currentDrag.y;
		currentDrag = nil;
	}
}

SC2DTabletSlider : SC2DSlider {

	var <>mouseDownAction,<>mouseUpAction;
	
	mouseDown { arg x,y,pressure,tiltx,tilty,deviceID,
			 buttonNumber,clickCount,absoluteZ,rotation;
		mouseDownAction.value(this,x,y,pressure,tiltx,tilty,deviceID, 
			buttonNumber,clickCount,absoluteZ,rotation);
	}
	mouseUp { arg x,y,pressure,tiltx,tilty,deviceID, 
			buttonNumber,clickCount,absoluteZ,rotation;
		mouseUpAction.value(this,x,y,pressure,tiltx,tilty,deviceID, 
			buttonNumber,clickCount,absoluteZ,rotation);
	}
	doAction { arg x,y,pressure,tiltx,tilty,deviceID, 
			buttonNumber,clickCount,absoluteZ,rotation;
		action.value(this,x,y,pressure,tiltx,tilty,deviceID, 
			buttonNumber,clickCount,absoluteZ,rotation);
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
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.value, 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isNumber or: { currentDrag.isKindOf(Function) };
	}
	receiveDrag {
		if (currentDrag.isNumber) {
			this.valueAction = currentDrag;
		}{
			this.action = currentDrag;
		};
		currentDrag = nil;
	}
}


SCPopUpMenu : SCControlView {
	var <font, <items;
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.items = #["linear","exponential","sine","welch","squared","cubed"];
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
		if (unicode == 16rF700, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF703, { this.valueAction = this.value + 1; ^this });
		if (unicode == 16rF701, { this.valueAction = this.value - 1; ^this });
		if (unicode == 16rF702, { this.valueAction = this.value - 1; ^this });
	}
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	items_ { arg array;
		items = array;
		this.setProperty(\items, items);
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

	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.value, 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.valueAction = currentDrag;
		currentDrag = nil;
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
	var	<>typingColor, <>normalColor;
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.value = 123.456;
		^v
	}

	init { arg argParent, argBounds;
		typingColor = Color.red;
		normalColor = Color.black;
		parent = argParent.asView; // actual view
		this.prInit(parent, argBounds.asRect,this.class.viewClass);
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
		});
	}
	value { ^object }
	value_ { arg val;
		keyString = nil;
		this.stringColor = normalColor;
		object = val;
		this.string = object.asString;
	}	
	valueAction_ { arg val;
		var prev;
		prev = object;
		this.value = val;
		if (object != prev, { this.doAction });
	}	
	boxColor {
		^this.getProperty(\boxColor, Color.new)
	}
	boxColor_ { arg color;
		this.setProperty(\boxColor, color)
	}

	properties {
		^super.properties ++ #[\boxColor]
	}
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			object.asFloat, 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.valueAction = currentDrag;
		
		currentDrag = nil;
	}
}



SCListView : SCControlView {
	var <font, <items, <>enterKeyAction;
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.items = #["linear","exponential","sine","welch","squared","cubed"];
		^v
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
			index = items.detectIndex({|item| item.asString.at(0).toUpper >= char });
			if (index.notNil, {
				this.valueAction = index
			});
		});
	}
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	items_ { arg array;
		items = array;
		this.setProperty(\items, items);
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
		^super.properties ++ #[\value, \font, \items, \stringColor]
	}

	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.value, 
		{
			beginDragAction.value(this);
		});
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.valueAction = currentDrag;
		currentDrag = nil;
	}
}


SCDragView : SCStaticTextBase {
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.object = \something;
		^v
	}
}

SCDragSource : SCDragView {
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			{ object },
			{
				beginDragAction.value(this);
			});
	}
}

SCDragSink : SCDragView
{	
	var <>acceptDrag = true;
	
	canReceiveDrag {
		^acceptDrag.value(this);
	}
	receiveDrag {
		this.object = currentDrag;
		this.doAction;
		currentDrag = nil;
	}
}

SCDragBoth : SCDragSink {
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			object, 
		{
			beginDragAction.value(this);
		});
	}
}


SCUserView : SCView { // abstract class
	draw {}
	mouseBeginTrack { arg x, y, modifiers; }
	mouseTrack { arg x, y, modifiers;x.postln }
	mouseEndTrack { arg x, y, modifiers; }
}

SCFuncUserView : SCUserView {
	var <>keyDownFunc, <>drawFunc;
	var <>mouseBeginTrackFunc, <>mouseTrackFunc, <>mouseEndTrackFunc;
	
	draw { drawFunc.value(this) }
	mouseBeginTrack { arg x, y, modifiers; 
		mouseBeginTrackFunc.value(this, x, y, modifiers); 
	}
	mouseTrack { arg x, y, modifiers; 
		mouseTrackFunc.value(this, x, y, modifiers); 
	}
	mouseEndTrack { arg x, y, modifiers; 
		mouseEndTrackFunc.value(this, x, y, modifiers); 
	}
	keyDown { arg key, modifiers, unicode; 
		keyDownFunc.value(this, key, modifiers, unicode) 
	}
}

//by jt v.0.22
SCMultiSliderView : SCView { 

	var <>acceptDrag = true;
	var <>metaAction, <>mouseUpAction;
	var <>size ;
	var <gap;
	var < editable = true;
	var < elasticMode = 0;
	

	draw {}
	mouseBeginTrack { arg x, y, modifiers;}
	mouseTrack { arg x, y, modifiers; 	}
	mouseEndTrack { arg x, y, modifiers;
		mouseUpAction.value(this);
	}
	properties {
		^super.properties ++ #[\value, \thumbSize, \fillColor, \strokeColor, \xOffset, \x, \y, \showIndex, \drawLines, \drawRects, \selectionSize, \startIndex, \referenceValues, \thumbWidth, \absoluteX, \isFilled, \step, \elasticResizeMode]
	}	
	
	elasticMode_{arg mode;
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
		this.setProperty(\isHorizontal,val);	
	}
	canReceiveDrag {
		^acceptDrag.value(this);
	}
	receiveDrag {
		//this.object = currentDrag;
		if(currentDrag.at(0).isKindOf(Array), { 
			this.value_(currentDrag.at(0));
			this.reference_(currentDrag.at(1));
		},{
			this.value_(currentDrag);
		});
		this.doAction;
		currentDrag = nil;
	}
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.defaultDrag,{
			beginDragAction.value(this);
		});
	}
	
	defaultDrag {
			var setsize, vals, rvals, outval;
			rvals = this.reference;
			vals = this.value;
			if(this.selectionSize > 1, {
				vals = vals.copyRange(this.index, this.selectionSize + this.index);});
			if(rvals.isNil, { 
				^vals 
			},{
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
	}
	
	doMetaAction{ 
		metaAction.value(this)
	} //when ctrl click
}

SCEnvelopeView : SCMultiSliderView {
	var connection, <>allConnections, doOnce;
	var <>items;

	value_ { arg val;
		if(val.at(1).size != val.at(0).size,{
			// otherwise its a fatal crash
			Error("SCEnvelopeView got mismatched times/levels arrays").throw;
		});
		this.size = val.at(0).size;
		this.setProperty(\value, val)
	}
	string_ { arg astring;
		items =items.add(astring);
		this.setProperty(\string, astring)
	}
	prString_ { arg astring;
		this.setProperty(\string, astring)
	}
	strings_ { arg astrings;
		astrings.do({arg str,i;
		this.select(i);
		this.prString_(str);
		});
		this.select(-1);
	}
	value {
		var ax, ay, axy;
		ax = Array.newClear(this.size);
		ay = Array.newClear(this.size);
		axy = Array.with(ax, ay);
		^this.getProperty(\value, axy)
	}
	thumbHeight_ { arg index, height;
		this.select(index);
		this.setProperty(\thumbHeight, height);
		this.select(-1);
	}
	thumbWidth_ { arg index, width;
		this.select(index);
		this.setProperty(\thumbWidth, width);
		this.select(-1);
	}
	connect { arg aconnections; //draw a connection between the selected index 
		this.setProperty(\connect, aconnections.asFloat);
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
	setStatic { arg index, abool;
		this.select(index);
		this.setProperty(\isStatic, abool);
		this.select(-1); //unselect
	}
	selectionColor_ { arg acolor;
		this.setProperty(\selectionColor, acolor)
	}
	receiveDrag {
		if(currentDrag.isKindOf(String), {
			this.addValue;
			items = items.insert(this.lastIndex + 1, currentDrag);
			this.strings_(items);		
		},{		
			this.value_(currentDrag);
		});
	}
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			this.value, 
		{
			beginDragAction.value(this);
		});
	}
	addValue{arg xval, yval;
		var arr, arrx, arry, aindx;
		aindx = this.lastIndex;
		aindx.postln;
		if(xval.isNil && yval.isNil, {
			arr = this.value;
			arrx = arr@0;
			arry = arr@1;
			xval = arrx.at(aindx) + 0.05;
			yval = arry.at(aindx);
		});
		if(aindx < (arrx.size - 1), {
			arrx = arrx.insert(aindx + 1 , xval);
			arry = arry.insert(aindx + 1, yval);
		},{
			arrx = arrx.add( xval );
			arry = arry.add( yval);
		});		
		this.value_([arrx,arry]);
	}	
}

Gradient {
	var color1, color2, direction, steps;
	*new { arg color1, color2, direction=\h, steps=64;
		^super.newCopyArgs(color1, color2, direction, steps)
	}
}

HiliteGradient {
	var color1, color2, direction, steps, frac;
	*new { arg color1, color2, direction=\v, steps=64, frac = 0.33;
		^super.newCopyArgs(color1, color2, direction, steps, frac)
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
