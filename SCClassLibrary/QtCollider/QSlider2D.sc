QSlider2D : QAbstractStepValue {
  *qtClass { ^'QcSlider2D' }

  *new { arg parent, bounds;
    var me = super.new( parent, bounds );
    me.connectMethod( 'randomize()', \randomize );
    ^me;
  }

  pixelStepX {
    // FIXME for now we are using step instead
    ^this.step;
  }

  pixelStepY {
    // FIXME for now we are using step instead
    ^this.step;
  }

  x {
    ^this.getProperty( \xValue );
  }

  x_ { arg aFloat;
    this.setProperty( \xValue, aFloat );
  }

  activex_ { arg aFloat;
    this.x_(aFloat);
    this.doAction;
  }

  y {
    ^this.getProperty( \yValue );
  }

  y_ { arg aFloat;
    this.setProperty( \yValue, aFloat );
  }

  activey_ { arg aFloat;
    this.y_(aFloat);
    this.doAction;
  }

  setXY { arg x, y;
    this.x_(x);
    this.y_(y);
  }

  setXYActive { arg x, y;
    this.setXY(x,y);
    this.doAction;
  }

  incrementX { arg factor=1.0; this.invokeMethod( \incrementX, factor.asFloat ); }
  decrementX { arg factor=1.0; this.invokeMethod( \decrementX, factor.asFloat ); }
  incrementY { arg factor=1.0; this.invokeMethod( \incrementY, factor.asFloat ); }
  decrementY { arg factor=1.0; this.invokeMethod( \decrementY, factor.asFloat ); }

  randomize {
    this.setXYActive( 1.0.rand, 1.0.rand );
  }

  knobColor { ^this.getProperty(\knobColor) }
  knobColor_ { arg color; this.setProperty(\knobColor, color) }

  background { ^this.getProperty(\grooveColor) }
  background_ { arg color; this.setProperty(\grooveColor, color) }

  defaultGetDrag { ^Point(this.x,this.y); }
  defaultCanReceiveDrag { ^(QView.currentDrag.class === Point); }
  defaultReceiveDrag {
    var pt = QView.currentDrag;
    this.setXYActive( pt.x, pt.y );
  }
}
