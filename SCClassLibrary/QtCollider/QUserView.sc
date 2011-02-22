QUserView : QView {
  var <drawFunc, <background;

  *qtClass { ^"QcCustomPainted" }

  *new { arg parent, bounds;
    var me = super.new(parent, bounds ?? {this.sizeHint} );
    me.canFocus = true;
    ^me;
  }

  *sizeHint {
    ^Point(150,150);
  }

  drawingEnabled { ^this.getProperty( \drawingEnabled ); }
  drawingEnabled_ { arg boolean; this.setProperty( \drawingEnabled, boolean ); }

  clearOnRefresh { ^this.getProperty( \clearOnRefresh ); }
  clearOnRefresh_ { arg boolean; this.setProperty( \clearOnRefresh, boolean ); }

  clearDrawing { this.invokeMethod( \clear ); }

  drawFunc_ { arg aFunction;
    if( drawFunc.isNil ) { this.drawingEnabled = true };
    if( aFunction.isNil ) { this.drawingEnabled = false };
    drawFunc = aFunction;
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  // reimplement mouse and key response to do nothing if enabled = false;

  keyDownEvent { arg char, modifiers, unicode, keycode;
    if( this.enabled ) { ^this.keyDown( char, modifiers, unicode, keycode ) };
  }

  keyUpEvent { arg char, modifiers, unicode, keycode;
    if( this.enabled ) { ^this.keyUp( char, modifiers, unicode, keycode ) };
  }

  mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
    if( this.enabled ) { ^this.mouseDown( x, y, modifiers, buttonNumber, clickCount ) };
  }

  mouseUpEvent { arg x, y, modifiers, buttonNumber;
    if( this.enabled ) { ^this.mouseUp( x, y, modifiers, buttonNumber ) };
  }

  mouseMoveEvent { arg x, y, modifiers;
    if( this.enabled ) { ^this.mouseMove( x, y, modifiers ) };
  }

  mouseOverEvent { arg x, y;
    if( this.enabled ) { ^this.mouseOver( x, y ) };
  }

  doDrawFunc { drawFunc.value(this) }
}
