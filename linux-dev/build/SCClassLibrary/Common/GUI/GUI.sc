

/*
	These are from sc3d5 for os9.
	they do not work currently in sc3 (os x)
*/
//
//SCSlider {
//	var dataptr;	
//	*new { arg pen, bounds, action;
//		
//	}
//	value {
//		_SCSlider_GetValue
//		^this.primitiveFailed
//	}
//	value_ { arg val;
//		_SCSlider_SetValue
//		^this.primitiveFailed
//	}
//}
/*
HasSubViews
{
	var <>pen, <>items, <>enabled, <>visible, <>layoutSize;
	*new { arg pen, items, enabled=true, visible=true, layoutSize;
		^super.newCopyArgs( pen.asPen, items ?? { Array.new(4) }, 
					enabled, visible, layoutSize ?? { LayoutSize.new })
	}

	add { arg item;
		items = items.add(item);
	}
	remove { arg item;
		items.remove(item);
	}
	at { arg index;
		^items.at(index)
	}
	getLayouts {
		^pen.use({
			items.collect({ arg item; item.getLayoutSize; });
		});
	}
	getTotalWeight { arg layouts;
		^layouts.sum({ arg layout; layout.weight });
	}
}

View : HasSubViews {	
	*new { arg items, enabled=true, visible=true, layoutSize;
		^super.new(items, enabled, visible, layoutSize);
	}
	draw {
		if (visible, {
			items.draw;
		});
	}
	mouseDown { arg where, event;
		if (enabled, {
			items.reverseDo({ arg item; 
				var tracker;
				tracker = item.mouseDown(where, event);
				if (tracker.notNil, { ^tracker });
			});
		});
		^nil
	}
	mouseOver { arg where, event;
		var view;
		if (enabled, {
			items.reverseDo({ arg item; 
				view = item.mouseOver(where, event);
				if (view.notNil, { ^view });
			});
		});
		^nil
	}
	layout { arg argBounds;
		pen.offset = argBounds.origin;
		//items.do({ arg item; item.layout(argBounds) });
	}
}

LView : View {
	// same as View but lays out its superimposed subviews
	layout { arg argBounds;
		pen.offset = argBounds.origin;
		items.do({ arg item; item.layout(argBounds) });
	}
}

SelectView : HasSubViews {
	var <>which=0;
	draw {
		if (visible, {
			pen.use({
				items.at(which).draw;
			});
		});
	}
	mouseDown { arg where, event;
		^items.at(which).mouseDown(where, event)
	}
	mouseOver { arg where, event;
		^items.at(which).mouseOver(where, event)
	}
	getLayoutSize {
		^items.at(which).getLayoutSize
	}
	layout { arg argBounds;
		^items.at(which).layout(argBounds)
	}
}

ControlView {
	var <>pen, <>bounds, <>control, <>margin, <>layoutSize;
	*new { arg pen, bounds, control, margin, layoutSize;
		^super.newCopyArgs(pen.asPen, bounds.asRect, control, margin ?? {Rect(2,2,2,2)},
			layoutSize ?? { LayoutSize.new })
	}
	mouseDown { arg where, event;
		if (bounds.containsPoint(where), {
			this.respond(where, event);
			^this; 
		},{ 
			^nil 
		});
	}
	mouseMoved { arg where, event;
		this.respond(where, event);
	}
	mouseUp { arg where, event;
		this.respond(where, event);
	}
	layout { arg argBounds;
		this.bounds = argBounds.insetByRect(margin);
	}
	getLayoutSize { ^layoutSize }
}

Button
{
	var <>bounds, <>states, <>margin, <>state=0, down=false;
	
	*new { arg bounds, states, margin, state=0;
		^super.newCopyArgs(bounds, states, margin ?? {Rect(2,2,2,2)}, state)
	}
	draw {
		var label, labelColor, btnColor, action;
		#label, labelColor, btnColor, action = states.at(state);
		this.prDraw(bounds, btnColor, labelColor, label, down);
	}
	mouseDown { arg where, event;
		if (bounds.containsPoint(where), { 
			down = true;
			^this; 
		},{ 
			^nil 
		});
	}
	mouseMoved { arg where, event;
		down = bounds.containsPoint(where);
	}
	mouseUp { arg where, event;
		var label, labelColor, btnColor, action;
		if (bounds.containsPoint(where), {
			#label, labelColor, btnColor, action = states.at(state);
			state = action.value(state) ?? { (state + 1) % states.size };
		});
		down = false;
	}
	layout { arg argBounds;
		this.bounds = argBounds.insetByRect(margin);
	}
	prDraw { arg rect, btnColor, labelColor, label, down;
		_Button_Draw
		^this.primitiveFailed
	}
}

NumberView : ControlView {
	var <>value;
	*new { arg pen, bounds, control, margin, layoutSize;
		^super.new(pen, bounds, control, margin, layoutSize)
	}
	layout { arg argBounds;
		this.bounds = argBounds;
	}
	draw {
		value = control.getMapped(value);
		pen.use({ value.asString.drawLeftJustIn(bounds) });
	}
	respond {}
}


Slider : ControlView
{
	classvar knobSize = 12;
	var <innerBounds, <value=0.0;
	var <background, <knob;
	*new { arg pen, bounds, control, margin, layoutSize;
		^super.new(pen, bounds, control, margin, layoutSize).init
	}
	init {
		this.bounds = bounds;
		knob = BevelRect.new(0,0,0,0, 2, false);
	}
	bounds_ { arg argBounds;
		bounds = argBounds;
		background = BevelRect.fromRect(bounds, 1, true);
		innerBounds = bounds.insetBy(1,1);
		this.calcKnob;
	}
	draw {
		this.calcKnob;
		pen.use({
			background.draw;
			knob.draw;
		});
	}
	isHorizontal {
		^bounds.width > bounds.height
	}
	calcValue { arg where;
		var halfKnob;
		halfKnob = knobSize * 0.5;
		if (this.isHorizontal, {
			value = (where.x - innerBounds.left - halfKnob)/(innerBounds.width - knobSize)
		},{
			value = 1.0 - ((where.y - innerBounds.top - halfKnob)/(innerBounds.height - knobSize))
		});
		value = value.clip(0.0, 1.0);
	}
	
	calcKnob {
		var k;
		value = control.get(value);
		if (this.isHorizontal, {
			k = value * (innerBounds.width - knobSize) + innerBounds.left;
			knob.set(k, innerBounds.top, k + knobSize, innerBounds.bottom);
		},{
			k = innerBounds.bottom - (value * (innerBounds.height - knobSize)) - knobSize;
			knob.set(innerBounds.left, k, innerBounds.right, k + knobSize);
		});
	}
	respond { arg where;
		this.calcValue(where);
		control.set(value);
	}
	value_ { arg val;
		value = val;
		control.set(value);
	}
}

Slider2D : ControlView
{
	classvar knobSize = 12;
	var <innerBounds, <x, <y;
	var <background, <knob;
	*new { arg pen, bounds, control, margin, layoutSize, x=0.5, y=0.5;
		^super.new(pen, bounds, control, margin, layoutSize).x_(x).y_(y).init
	}
	init {
		this.bounds = bounds;
		knob = BevelRect.new(0,0,0,0, 2, false);
	}
	bounds_ { arg argBounds;
		bounds = argBounds;
		background = BevelRect.fromRect(bounds, 1, true);
		innerBounds = bounds.insetBy(1,1);
		this.calcKnob;
	}
	draw {
		this.calcKnob;
		pen.use({
			background.draw;
			knob.draw;
		});
	}
	isHorizontal {
		^bounds.width > bounds.height
	}
	calcValue { arg where;
		var halfKnob;
		halfKnob = knobSize * 0.5;
		x = (where.x - innerBounds.left - halfKnob)/(innerBounds.width - knobSize);
		y = 1.0 - ((where.y - innerBounds.top - halfKnob)/(innerBounds.height - knobSize));
		x = x.clip(0.0, 1.0);
		y = y.clip(0.0, 1.0);
	}
	calcKnob {
		var kx, ky;
		x = control.get(x, 0);
		y = control.get(y, 1);
		kx = x * (innerBounds.width - knobSize) + innerBounds.left;
		ky = innerBounds.bottom - (y * (innerBounds.height - knobSize)) - knobSize;
		knob.set(kx, ky, kx + knobSize, ky + knobSize);
	}
	respond { arg where;
		this.calcValue(where);
		control.set(x, y);
	}
	x_ { arg val;
		x = val;
		control.set(x, y);
	}
	y_ { arg val;
		y = val;
		control.set(x, y);
	}
}

RangeSlider : ControlView
{
	var <innerBounds, <lo, <hi, <>anchor;
	var <background, <knob;
	*new { arg pen, bounds, control, margin, layoutSize, lo=0.0, hi=0.0;
		^super.new(pen, bounds, control, margin, layoutSize).lo_(lo).hi_(hi).init
	}
	init {
		knob = View(Pen(foreColor: Color.grey), [ Rect.new ]);
		this.bounds = bounds;
	}
	bounds_ { arg argBounds;
		bounds = argBounds;
		background = BevelRect.fromRect(bounds, 2, true);
		innerBounds = bounds.insetBy(2,2);
		this.calcKnob;
	}
	draw {
		pen.use({
			Pen.action = \fill;
			background.draw;
			knob.draw;
		});
	}
	isHorizontal {
		^bounds.width > bounds.height
	}
	calcValue { arg where, event;
		var val1, val2;
		if (this.isHorizontal, {
			val1 = (event.where.x - innerBounds.left)/(innerBounds.width);
			val2 = (where.x - innerBounds.left)/(innerBounds.width);
		},{
			val1 = 1.0 - ((event.where.y - innerBounds.top)/(innerBounds.height));
			val2 = 1.0 - ((where.y - innerBounds.top)/(innerBounds.height));
		});
		lo = min(val1, val2).clip(0.0, 1.0);
		hi = max(val1, val2).clip(0.0, 1.0);
		this.calcKnob;
	}
	calcKnob {
		var klo, khi;
		if (this.isHorizontal, {
			klo = lo * innerBounds.width + innerBounds.left;
			khi = hi * innerBounds.width + innerBounds.left;
			khi = min(max(klo+1, khi), innerBounds.right);
			knob.items.put(0, Rect.new(klo, innerBounds.top, khi, innerBounds.bottom) );
		},{
			klo = innerBounds.bottom - (lo * innerBounds.height);
			khi = innerBounds.bottom - (hi * innerBounds.height);
			khi = min(klo-1, khi);
			knob.items.put(0, Rect.new(innerBounds.left, khi, innerBounds.right, klo) );
		});
	}
	respond { arg where, event;
		this.calcValue(where, event);
		control.set(lo, hi);
	}
	hi_ { arg val;
		hi = val;
		control.set(lo, hi);
	}
	lo_ { arg val;
		lo = val;
		control.set(lo, hi);
	}
}

Knob : ControlView
{
	var innerBounds, radius, center, innerEdge, outerEdge;
	var <value = 0.0;
	var <background, <knob;
	*new { arg pen, bounds, control, margin, layoutSize, value=0.0;
		pen = pen ?? { Pen(action: \fill, foreColor: rgb(220,220,220)) };
		^super.new(pen, bounds, control, margin, layoutSize).init
	}
	init {
		this.bounds = bounds;
	}
	bounds_ { arg argBounds;
		bounds = argBounds.centerSquare;
		innerBounds = bounds.insetByRect(margin);
		center = bounds.center;
		radius = (bounds.width * 0.5) - 4;
		background = Oval.fromRect(innerBounds);
		this.calcKnob;
	}
	draw {
		this.calcKnob;
		pen.use({
			Pen.action = \fill;
			background.draw;
			Pen.action = \stroke;
			Pen.penSize = 1@1;
			Pen.foreColor = Color.black;
			background.draw;
			Pen.penSize = 2@2;
			Pen.moveTo(innerEdge);
			Pen.lineTo(outerEdge)
		});
	}
	calcValue { arg where;
		var angle;
		angle = atan2(where.x - center.x, center.y - where.y);
		value = ((angle + 0.75pi) / 1.5pi).clip(0.0,1.0);
	}
	calcKnob {
		var angle, halfRadius;
		value = control.get(value);
		angle = 1.5pi * value + 0.25pi;
		outerEdge = Point(
			center.x - (radius * sin(angle)),
			center.y + (radius * cos(angle))
		);	
		halfRadius = 0.5 * radius;
		innerEdge = Point(
			center.x - (halfRadius * sin(angle)),
			center.y + (halfRadius * cos(angle))
		);	
	}
	respond { arg where;
		this.calcValue(where);
		control.set(value);
	}
	layout { arg argBounds;
		this.bounds = argBounds;
	}
	value_ { arg val;
		value = val;
		control.set(value);
	}
}


Label {
	var <>pen, <>bounds, <>string, <>layoutSize;
	*new { arg pen, bounds, string, layoutSize;
		^super.newCopyArgs(pen.asPen, bounds.asRect, string.asString, layoutSize ?? { LayoutSize.new })
	}
	layout { arg argBounds;
		this.bounds = argBounds;
	}
	draw {
		pen.use({ string.drawLeftJustIn(bounds) });
	}
	mouseOver { arg where, event;
		if (bounds.containsPoint(where), {
			^this
		});
		^nil
	}
	getLayoutSize { ^layoutSize }
}

CLabel : CXLabel {
	draw {
		pen.use({ string.drawCenteredIn(bounds) });
	}
}

TextBox {
	var <>pen, <>bounds, <>string, <>justify=\default;
	*new { arg pen, bounds, string, justify=\default;
		^super.newCopyArgs(pen.asPen, bounds.asRect, string, justify)
	}
	draw {
		pen.use({
			this.prDraw;
		});
	}
	layout { arg argBounds;
		this.bounds = argBounds;
	}
	prDraw {
		_TextBox_Draw
		^this.primitiveFailed
	}
}

WindowLayer : View {
	mouseDown { arg where, event;
		if (enabled, {
			items.reverseDo({ arg item; 
				if (item.bounds.containsPoint(where), {
					items.swap(items.indexOf(item), items.size - 1);
					^item.mouseDown(where, event);
				});
			});
		});
	}
}
*/
	