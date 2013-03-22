QUserView : QView {
  var <drawFunc, <drawingEnabled=true;

  *qtClass { ^'QcCustomPainted' }

  *new { arg parent, bounds;
    var me = super.new(parent, bounds ?? {this.sizeHint} );
    me.canFocus = true;
    ^me;
  }

  *sizeHint {
    ^Point(150,150);
  }

  drawingEnabled_ { arg boolean;
    // Allow setting the property apart from the instance variable
    // to optimize when drawFunc is nil. See drawFunc_ implementation.
    drawingEnabled = boolean;
    this.setProperty( \drawingEnabled, boolean );
  }

  clearOnRefresh { ^this.getProperty( \clearOnRefresh ); }
  clearOnRefresh_ { arg boolean; this.setProperty( \clearOnRefresh, boolean ); }

  clearDrawing { this.invokeMethod( \clear ); }

  drawFunc_ { arg aFunction;
    this.setProperty( \drawingEnabled, aFunction.notNil );
    drawFunc = aFunction;
  }

  draw {
    // NOTE: it is only allowed to call this while a QPaintEvent is being
    // processed by this QWidget, or an error will be thrown.
    drawFunc.value(this);
  }

  animate_ { arg bool; this.invokeMethod( \animate, bool ); }

  frameRate_ { arg fps; this.setProperty( \frameRate, fps.asFloat ); }
  frameRate { ^this.getProperty( \frameRate ); }

  frame { ^this.getProperty( \frameCount ); }

  // override QView's action_ to not connect to 'action()' signal
  action_ { arg func;
    action = func;
  }

  doDrawFunc { drawFunc.value(this) }
}
