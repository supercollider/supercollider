QWindow : QAbstractScroll {
  classvar <allWindows, <>initAction;

  var scroll, resizable, <drawHook, <background;

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
          .initQWindow(name, scroll, resizable);
  }

  initQWindow { arg argName, argScroll, argResizeable;
    name = argName;
    scroll = argScroll;
    resizable = argResizeable == true;
    QWindow.addWindow( this );
    this.connectFunction( 'destroyed()', { |me| QWindow.removeWindow(me); }, false );
    QWindow.initAction.value( this );
  }

  // compatibility with SCWindow
  asView {
    ^this;
  }

  // compatibility with SCWindow
  view {
    ^this;
  }

  // compatibility with SCTopView
  findWindow {
    ^this;
  }

  bounds_ { arg aRect;
    var r = QWindow.flipY( aRect.asRect );
    super.bounds_(r);
    if( resizable.not ) {
      this.setProperty( \minimumSize, r.asSize );
      this.setProperty( \maximumSize, r.asSize );
    }
  }

  bounds {
    ^QWindow.flipY( super.bounds );
  }

  innerBounds {
    if( scroll )
      { ^this.getProperty( \innerBounds, Rect.new ) }
      { ^super.bounds.moveTo(0,0) };
  }

  visibleOrigin {
    if( scroll )
      { ^this.getProperty( \visibleOrigin, Point.new ) }
      { ^Point() };
  }

  visibleOrigin_ { arg point;
    if( scroll ) { this.setProperty( \visibleOrigin, point ) };
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  drawHook_ { arg aFunction;
    if( drawHook.isNil ) { this.setProperty( \paint, true ) };
    drawHook = aFunction;
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

  doDrawFunc { drawHook.value(this) }
}
