
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
//		- equalSize option
//

SCKnob : SCUserView {
	classvar <>defaultMode, <>equalSize=false, <>compactRatio=0.87;
	var size, widthDiv2, center, hit;
	var <>color, <value, prevValue, <>step, <>keystep, <>mode, isCentered = false;
	var <skin;

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
	
		if ( equalSize ) {
			argBounds = argBounds.asRect.height_( argBounds.asRect.width );
		}{
			argBounds = argBounds.asRect.height_( 
				(argBounds.asRect.width * compactRatio).ceil
			);
		};

		super.init(argParent, argBounds);
		
		value = 0.0;
		mode = defaultMode;
		keystep = 0.01;
		step = 0.01;
		
		skin = GUI.skins.default.knob.default;

		this.oldMethodsCompat(skin);
		
		size = this.bounds.width;
		widthDiv2 = size * 0.5;
		center = Point(widthDiv2, widthDiv2);
		
		if (relativeOrigin.not) {
			center = center + (this.bounds.left @ this.bounds.top);
		};
		
		this.receiveDragHandler = { this.valueAction_(SCView.currentDrag); };
		this.beginDragAction = { value.asFloat; };
		this.canReceiveDragHandler = { SCView.currentDrag.isNumber };
	}

	draw {
		var startAngle, arcAngle, aw;

		color[2].set;
		SCPen.addAnnularWedge(
			center,
			widthDiv2 - (0.08 * size), 
			widthDiv2, 	
			0.25pi, 
			-1.5pi
		);
		SCPen.perform(\fill);

		if (isCentered.not, {
			startAngle = 0.75pi; 
			arcAngle = 1.5pi * value;
		}, {
			startAngle = -0.5pi; 
			arcAngle = 1.5pi * (value - 0.5);
		});

		color[1].set;
		SCPen.addAnnularWedge(
			center, 
			widthDiv2 - (0.12 * size), 
			widthDiv2, 	
			startAngle, 
			arcAngle
		);
		SCPen.perform(\fill);

		color[0].set;
		aw = widthDiv2 - (0.14 * size);
		SCPen.addWedge(center, aw, 0, 2pi);
		SCPen.perform(\fill);

		color[3].set;
		SCPen.width = (0.08 * size);
		SCPen.moveTo(center);
		SCPen.lineTo(Polar.new(aw, 0.75pi + (1.5pi * value)).asPoint + center);
		SCPen.stroke;
	}

	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;
		
		if (relativeOrigin.not) {
			hit = x @ y;
		}{
			hit = this.mousePosition;
		};
		
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount);
		
		this.mouseMove(x, y, modifiers);

	}
		
	mouseMove { arg x, y, modifiers;
		var mp, pt, angle, inc = 0;
		
		if (relativeOrigin) {
			mp = this.mousePosition;
			x = mp.x; y = mp.y;
		};
		
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
		
//	mouseOver { arg x, y;
//		mouseOverAction.value(this, x, y);
//	}

	defaultKeyDownAction { arg char, modifiers, unicode,keycode;
		// standard keydown
		if (char == $r, { this.valueAction = 1.0.rand; });
		if (char == $n, { this.valueAction = 0.0; });
		if (char == $x, { this.valueAction = 1.0; });
		if (char == $c, { this.valueAction = 0.5; });
		if (char == $[, { this.decrement; this});
		if (char == $], { this.increment; this });
		if (unicode == 16rF700, { this.increment; this });
		if (unicode == 16rF703, { this.increment; this });
		if (unicode == 16rF701, { this.decrement; this });
		if (unicode == 16rF702, { this.decrement; this });
		^nil		// bubble if it's an invalid key
	}

	increment { ^this.valueAction = (this.value + keystep).min(1) }

	decrement { ^this.valueAction = (this.value - keystep).max(0) }

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
		isCentered = bool;
		this.refresh;
	}
	
	centered {
		^isCentered
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
}
