QWindow : QAbstractScroll {
  classvar <allWindows, <>initAction;

  var scroll, <drawHook, <background;

  //dummy for compatibility
  var <currentSheet;

  *initClass {
    ShutDown.add( { QWindow.closeAll } );
  }

  *screenBounds {
    ^this.prScreenBounds( Rect.new );
  }

  *closeAll {
    allWindows.copy.do { |win| win.close };
  }

  *qtClass {^"QcWindow"}

  *new { arg name,
         bounds = Rect( 128, 64, 400, 400 ),
         resizable = true,
         border = true,
         server,
         scroll = false;

    var b = QWindow.flipY( bounds.asRect );

    //NOTE we omit server, which is only for compatibility with SwingOSC
    ^super.newCustom( [name, b, resizable, border, scroll] )
          .initQWindow(name, scroll);
  }

  initQWindow { arg argName, argScroll;
    name = argName;
    scroll = argScroll;
  }

  asView {
    ^this;
  }

  view {
    ^this;
  }

  bounds_ { arg aRect;
    super.bounds_( QWindow.flipY( aRect.asRect ) );
  }

  bounds {
    ^QWindow.flipY( super.bounds );
  }

  innerBounds {
    if( scroll )
      { ^this.getProperty( \innerBounds, Rect.new ) }
      { ^super.bounds.moveTo(0,0) };
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  drawHook_ { arg aFunction;
    if( drawHook.notNil ) { this.disconnectFunction( 'painting()', drawHook ) };
    this.connectToFunction( 'painting()', aFunction, true );
    if( drawHook.isNil ) { this.setProperty( \paint, true ) };
    drawHook = aFunction;
  }

  refresh {
    if( drawHook.notNil ) {
      this.invokeMethod( \repaint );
    };
  }
  // ---------------------- private ------------------------------------

  *flipY { arg aRect;
    var flippedTop = QWindow.screenBounds.height - aRect.top - aRect.height;
    ^Rect( aRect.left, flippedTop, aRect.width, aRect.height );
  }

  *prScreenBounds { arg return;
    _QWindow_ScreenBounds
    ^this.primitiveFailed
  }

  *addWindow { arg window;
    allWindows = allWindows.add( window );
  }

  *removeWindow { arg window;
    allWindows.remove( window );
  }
}
