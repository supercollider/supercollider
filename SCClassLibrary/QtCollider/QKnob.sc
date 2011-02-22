
// blackrain at realizedsound dot net - 05/2006
//  fix key modidiers bug by Stephan Wittwer 08/2006 - thanks!
//  Knob updates only on value changes - 10/2006
//  GUI.cocoa changes - 04/2007
//
//  03.10.2008 - new implementation:
//    - Knob now is a subclass of SCViewHolder
//    - Relative origin
//
//  01.20.2009 - SCKnob
//    - a subclass of SCUserView again.
//    - isSquare option
//
//  08.03.2010 - QKnob = SCKnob adjusted for GUI.qt scheme (by Jakob Leben)

QKnob : QUserView {
  classvar <>defaultMode, <>isSquare=false, <>compactRatio=0.87;
  var size, widthDiv2, center, aw8, aw12, aw14, hit;
  var <>color, <value, prevValue, <>step, <>keystep, <>mode, <centered = false;
  var <skin;
  var <>shift_scale = 100.0, <>ctrl_scale = 10.0, <>alt_scale = 0.1;

  *initClass {
    var version;

    defaultMode='round';

    StartUp.add({

      GUI.skins.default.put('knob', (
        default: (
          scale:  Color.black.alpha_(0.3),
          center: Color.blue(0.7, 0.5),
          level:  Color.green(0.8, 0.8),
          dial: Color.black.alpha_(0.7),
          defaultMode: 'round'
        )
      ));

    });
  }

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQKnob( parent );
  }

  initQKnob { arg argParent;

    var argBounds = this.calcConsts( this.bounds );

    this.bounds_( argBounds );

    value = 0.0;
    keystep = 0.01;
    step = 0.01;
    mode = defaultMode;

    skin = GUI.skins.default.knob.default;

    this.oldMethodsCompat(skin);

    /*
    this.receiveDragHandler = { this.valueAction_(SCView.currentDrag); };
    this.beginDragAction = { value.asFloat; };
    this.canReceiveDragHandler = { SCView.currentDrag.isNumber };*/

    this.drawFunc_( { this.drawKnob } );
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

  *sizeHint {
    ^Point(60,60);
  }

  bounds_ { arg rect;
    rect = this.calcConsts(rect);
    super.bounds_(rect);
  }

  drawKnob {
    var startAngle, arcAngle;

    QPen.color = color[2];
    QPen.addAnnularWedge(
      center,
      aw8,
      widthDiv2,
      0.25pi,
      -1.5pi
    );
    QPen.perform(\fill);

    if (centered.not, {
      startAngle = 0.75pi;
      arcAngle = 1.5pi * value;
    }, {
      startAngle = -0.5pi;
      arcAngle = 1.5pi * (value - 0.5);
    });

    QPen.color = color[1];
    QPen.addAnnularWedge(
      center,
      aw12,
      widthDiv2,
      startAngle,
      arcAngle
    );
    QPen.perform(\fill);

    QPen.color = color[0];
    QPen.addWedge(center, aw14, 0, 2pi);
    QPen.perform(\fill);

    QPen.color = color[3];
    QPen.width = (0.08 * size);
    QPen.moveTo(center);
    QPen.lineTo(Polar.new(aw14, 0.75pi + (1.5pi * value)).asPoint + center);
    QPen.stroke;
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
      { modifiers & 16r2000000 > 0 } { shift_scale }
      { modifiers & 16r4000000 > 0 } { ctrl_scale }
      { modifiers & 16r8000000 > 0 } { alt_scale }
      { 1 };
  }

  defaultKeyDownAction { arg char, modifiers, unicode, keycode;
    var zoom = this.getScale(modifiers);

    // standard keydown
    switch( char,
      $r, { this.valueAction = 1.0.rand },
      $n, { this.valueAction = 0.0 },
      $x, { this.valueAction = 1.0 },
      $c, { this.valueAction = 0.5 },

      {
        switch( keycode,
          16r5b, { this.decrement(zoom) },
          16r5d, { this.increment(zoom) },
          16r1000013, { this.increment(zoom) },
          16r1000014, { this.increment(zoom) },
          16r1000015, { this.decrement(zoom) },
          16r1000012, { this.decrement(zoom) },
          {^this} // bubble if it's an invalid key
        )
      }
    );
    ^true;
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
