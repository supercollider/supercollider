
SCView {  // abstract class
	classvar <>currentDrag;
	
	var dataptr, parent, <>action, <background;
	
	*new { arg parent, bounds;
		^super.new.init(parent, bounds);
	}
	
	init { arg argParent, argBounds;
		parent = argParent.asView;
		this.prInit(parent, argBounds ?? {Rect(20,20,20,20)});
		if (parent.notNil, { parent.add(this); });
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
	
	keyDown { arg key, modifiers; }
	
	doAction {
		action.value(this);
	}
	
	properties {
		^#[\bounds, \visible, \enabled, \canFocus, \resize, \background]
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
	prInit { arg argParent, argBounds;
		_SCView_New
		^this.primitiveFailed
	}
	prClose { dataptr = nil; }
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
}

SCContainerView : SCView { // abstract class
	var children, <>decorator;
			
	add { arg child;
		children = children.add(child);
		if (decorator.notNil, { decorator.place(child); });
	}
	// private
	prClose {
		super.prClose;
		children.do({ arg item; item.prClose });
	}
}

SCCompositeView : SCContainerView {
}

SCTopView : SCCompositeView {
	// created by SCWindow
}

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
		^super.properties ++ [\knobColor, \step]
	}

}

SCSlider : SCSliderBase
{
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setPropertyWithAction(\value, val);
	}	
	
	keyDown { arg key, modifiers;
		if (key == $r, { this.value = 1.0.rand; ^this });
		if (key == $n, { this.value = 0.0; ^this });
		if (key == $x, { this.value = 1.0; ^this });
		if (key == $c, { this.value = 0.5; ^this });
		if (key == $], { 
			this.value = this.value + (1/this.bounds.width); 
			^this 
		});
		if (key == $[, { 
			this.value = this.value - (1/this.bounds.width); 
			^this 
		});
	}
	beginDrag { 
		currentDrag = this.value; 
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		currentDrag = nil;
	}
}

SCKnob : SCSlider
{
}

SCRangeSlider : SCSliderBase {
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
		^super.properties ++ [\lo, \hi]
	}
	keyDown { arg key, modifiers;
		var a, b;
		if (key == $r, { 
			a = 1.0.rand; 
			b = 1.0.rand; 
			this.lo = min(a, b);
			this.hi = max(a, b);
			^this 
		});
		if (key == $n, { this.lo = 0.0; this.hi = 0.0; ^this });
		if (key == $x, { this.lo = 1.0; this.hi = 1.0; ^this });
		if (key == $c, { this.lo = 0.5; this.hi = 0.5; ^this });
		if (key == $a, { this.lo = 0.0; this.hi = 1.0; ^this });
	}
	beginDrag { 
		currentDrag = Point(this.lo, this.hi); 
	}
	canReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	receiveDrag {
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
		^super.properties ++ [\x, \y]
	}
	keyDown { arg key, modifiers;
		if (key == $r, { this.x = 1.0.rand; this.y = 1.0.rand; ^this });
		if (key == $n, { this.x = 0.0; this.y = 0.0; ^this });
		if (key == $x, { this.x = 1.0; this.y = 1.0; ^this });
		if (key == $c, { this.x = 0.5; this.y = 0.5; ^this });
	}
	beginDrag { 
		currentDrag = Point(this.x, this.y); 
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


SCButton : SCControlView {
	var <font, <states;
	
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setPropertyWithAction(\value, val);
	}	

	keyDown { arg key, modifiers;
		if (key == $ , { this.value = this.value + 1; ^this });
		if (key == $\r, { this.value = this.value + 1; ^this });
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
		^super.properties ++ [\value, \font, \states]
	}
}


SCStaticText : SCView {
	var <string, <font, <object;
	
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
		this.string = object.asString(80);
	}
	
	properties {
		^super.properties ++ [\string, \font, \stringColor]
	}
}

SCNumberBox : SCStaticText {
	var keyString;
	
	keyDown { arg key, modifiers; 
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			this.value = keyString.asFloat;
			^this
		});
		if (key == 127.asAscii, { // delete key
			keyString = nil;
			this.string = object.asString;
			this.stringColor = Color.black;
			^this
		});
		if (key.isDecDigit || "+-.eE".includes(key), {
			if (keyString.isNil, { 
				keyString = String.new;
				this.stringColor = Color.red;
			});
			keyString = keyString.add(key);
			this.string = keyString;
		});
	}
	value { ^object }
	value_ { arg val;
		keyString = nil;
		this.stringColor = Color.black;
		object = val;
		this.string = val.asString;
		this.doAction;
	}
	properties {
		^super.properties ++ [\boxColor]
	}
	beginDrag { 
		currentDrag = object.asFloat; 
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		currentDrag = nil;
	}
}


SCDragView : SCStaticText {
}

SCDragSource : SCDragView {
	var <>beginDragAction;
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			object, 
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
	var <>beginDragAction;
	
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
	mouseTrack { arg x, y, modifiers; }
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
	keyDown { arg key, modifiers; 
		keyDownFunc.value(this, key, modifiers) 
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


