// blackrain at realizedsound dot net - 05/2006
//	fix key modidiers bug by Stephan Wittwer 08/2006 - thanks!
//	Knob updates only on value changes - 10/2006
//	GUI.cocoa changes - 04/2007
//
//	03.10.2008 - new implementation:
//		- Knob now is a subclass of SCViewHolder
//		- Relative origin
//
//	01.20.2009 - SCKnob
//		- a subclass of SCUserView again.
//		- isSquare option
//

SCKnob : SCUserView {
	classvar <>defaultMode, <>isSquare=false, <>compactRatio=0.87;
	var size, widthDiv2, center, aw8, aw12, aw14, hit;
	var <>color, <value, prevValue, <>step, <>keystep, <>mode, <centered = false;
	var <skin;
	var <>shift_scale = 100.0, <>ctrl_scale = 10.0, <>alt_scale = 0.1;

	*viewClass { ^SCUserView }

	*initClass {
		var version;

		defaultMode='round';

		StartUp.add({

			GUI.skins.default.put('knob', (
				default: (
					scale:	Color.black.alpha_(0.3),
					center:	Color.blue(0.7, 0.5),
					level:	Color.green(0.8, 0.8),
					dial:	Color.black.alpha_(0.7),
					defaultMode: 'round'
				)
			));

		});
	}

	init { arg argParent, argBounds;

		argBounds = this.calcConsts(argBounds);

		super.init(argParent, argBounds);

		value = 0.0;
		keystep = 0.01;
		step = 0.01;
		mode = defaultMode;

		skin = GUI.skins.default.knob.default;

		this.oldMethodsCompat(skin);

		this.receiveDragHandler = { this.valueAction_(SCView.currentDrag); };
		this.beginDragAction = { value.asFloat; };
		this.canReceiveDragHandler = { SCView.currentDrag.isNumber };
	}

	calcConsts { arg rect;
		if ( isSquare ) {
			rect = rect.asRect.height_( rect.asRect.width );
		}{
			rect = rect.asRect.height_( (rect.asRect.width * compactRatio).ceil );
		};
		size = rect.width;
		widthDiv2 = size * 0.5;
		aw8  = widthDiv2 - (0.08 * size);
		aw12 = widthDiv2 - (0.12 * size);
		aw14 = widthDiv2 - (0.14 * size);
		center = Point(widthDiv2, widthDiv2);

		^rect
	}

	bounds_ { arg rect;
		rect = this.calcConsts(rect);
		super.bounds_(rect);
	}

	draw {
		var startAngle, arcAngle;

		SCPen.color = color[2];
		SCPen.addAnnularWedge(
			center,
			aw8,
			widthDiv2,
			0.25pi,
			-1.5pi
		);
		SCPen.perform(\fill);

		if (centered.not, {
			startAngle = 0.75pi;
			arcAngle = 1.5pi * value;
		}, {
			startAngle = -0.5pi;
			arcAngle = 1.5pi * (value - 0.5);
		});

		SCPen.color = color[1];
		SCPen.addAnnularWedge(
			center,
			aw12,
			widthDiv2,
			startAngle,
			arcAngle
		);
		SCPen.perform(\fill);

		SCPen.color = color[0];
		SCPen.addWedge(center, aw14, 0, 2pi);
		SCPen.perform(\fill);

		SCPen.color = color[3];
		SCPen.width = (0.08 * size);
		SCPen.moveTo(center);
		SCPen.lineTo(Polar.new(aw14, 0.75pi + (1.5pi * value)).asPoint + center);
		SCPen.stroke;
	}

	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;

		hit =  x @ y;

		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount);

		this.mouseMove(x, y, modifiers);

	}

	mouseMove { arg x, y, modifiers;
		var mp, pt, angle, inc = 0;


		if (modifiers & 1048576 != 1048576) { // we are not dragging out - apple key
			case
				{ (mode == \vert) || (modifiers & 262144 == 262144) } { // Control
					if ( hit.y > y, {
						inc = step;
					}, {
						if ( hit.y < y, {
							inc = step.neg;
						});
					});
					value = (value + inc).clip(0.0, 1.0);
					hit = Point(x,y);
					if (prevValue != value) {
						action.value(this, x, y, modifiers);
						prevValue = value;
						this.refresh;
					}
				}
				{ (mode == \horiz) || (modifiers & 524288 == 524288) } { // Option
					if ( hit.x > x, {
						inc = step.neg;
					}, {
						if ( hit.x < x, {
							inc = step;
						});
					});
					value = (value + inc).clip(0.0, 1.0);
					hit = Point(x,y);
					if (prevValue != value) {
						action.value(this, x, y, modifiers);
						prevValue = value;
						this.refresh;
					}
				}
				{ mode == \round } {
					pt = center - Point(x,y);
					angle = Point(pt.y, pt.x.neg).theta;
					if ((angle >= -0.80pi) and: { angle <= 0.80pi} , {
						value = [-0.75pi, 0.75pi].asSpec.unmap(angle);
						if (prevValue != value) {
							action.value(this, x, y, modifiers);
							prevValue = value;
							this.refresh;
						}
					});

				}
		};

		mouseMoveAction.value(this, x, y, modifiers);
	}

	getScale { |modifiers|
		^case
			{ modifiers & 131072 == 131072 } { shift_scale }
			{ modifiers & 262144 == 262144 } { ctrl_scale }
			{ modifiers & 524288 == 524288 } { alt_scale }
			{ 1 };
	}

	defaultKeyDownAction { arg char, modifiers, unicode,keycode;
		var zoom = this.getScale(modifiers);

		// standard keydown
		if (char == $r, { this.valueAction = 1.0.rand; ^this });
		if (char == $n, { this.valueAction = 0.0; ^this });
		if (char == $x, { this.valueAction = 1.0; ^this });
		if (char == $c, { this.valueAction = 0.5; ^this });

		if (char == $[, { this.decrement(zoom); ^this });
		if (char == $], { this.increment(zoom); ^this });
		if (unicode == 16rF700, { this.increment(zoom); ^this });
		if (unicode == 16rF703, { this.increment(zoom); ^this });
		if (unicode == 16rF701, { this.decrement(zoom); ^this });
		if (unicode == 16rF702, { this.decrement(zoom); ^this });
		^nil		// bubble if it's an invalid key
	}

	increment { |zoom=1| ^this.valueAction = (this.value + (keystep * zoom)).min(1) }

	decrement { |zoom=1| ^this.valueAction = (this.value - (keystep * zoom)).max(0) }

	value_ { arg val;
		value = val.clip(0.0, 1.0);
		this.refresh;
	}

	valueAction_ { arg val;
		value = val.clip(0.0, 1.0);
		action.value(this);
		this.refresh;
	}

	centered_ { arg bool;
		centered = bool;
		this.refresh;
	}

	skin_ { arg newskin;
		if ( newskin.notNil ) {
			skin = newskin;
			newskin.proto_( GUI.skins.default.knob.default );
			this.oldMethodsCompat;
			this.refresh;
		}{
			format("%: skin not found.", this.class).inform;
		};
	}
	oldMethodsCompat {
		color = [
			skin.center,
			skin.level,
			skin.scale,
			skin.dial
		];
		defaultMode = skin.defaultMode;
	}

	*paletteExample{arg parent, bounds;
		^this.new(parent, bounds.asRect.height@bounds.asRect.height);
	}

}
