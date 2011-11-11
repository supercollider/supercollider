QSlider : QAbstractStepValue {
  //compatibility stuff:
  var <orientation;
  var <> thumbSize;

  *qtClass { ^"QcSlider" }

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQSlider( bounds );
  }

  value {
    ^this.getProperty( \value );
  }

  value_ { arg argVal;
    this.setProperty( \value, argVal );
  }

  valueAction_ { arg val;
    this.value_(val);
    action.value(this);
  }

  knobColor {
    ^this.palette.buttonColor;
  }

  knobColor_ { arg color;
    this.setProperty( \palette, this.palette.buttonColor_(color) );
  }

  initQSlider { arg bounds;
    var r;
    if( bounds.notNil ) {
      r = bounds.asRect;
      if( r.width > r.height ) {
        this.orientation_( \horizontal );
      } {
        this.orientation_( \vertical );
      }
    }
  }

  pixelStep {
    // FIXME for now we are using step instead
    ^this.step;
  }

  orientation_ { arg aSymbol;
    orientation = aSymbol;
    this.setProperty( \orientation, QOrientation(aSymbol) );
  }

  defaultKeyDownAction {  arg char, modifiers, unicode, keycode;
    var scale = this.getScale( modifiers );
    switch( char,
      $r, { this.valueAction = 1.0.rand },
      $n, { this.valueAction = 0.0 },
      $x, { this.valueAction = 1.0 },
      $c, { this.valueAction = 0.5 },
      {
        switch( keycode,
          16r5d, { this.increment(scale) },
          16r1000013, { this.increment(scale) },
          16r1000014, { this.increment(scale) },
          16r5b, { this.decrement(scale) },
          16r1000015, { this.decrement(scale) },
          16r1000012, { this.decrement(scale) },
          { ^this; } // if unhandled, let Qt process the event
        );
        this.doAction;
      }
    );
    ^true; // accept the event and stop its processing
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^QView.currentDrag.isNumber; }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}
