QWindow : QView {
  classvar <allWindows, <>initAction;

  var <drawHook, <background;

  //dummy for compatibility
  var <currentSheet;

  *qtClass {^"QcWindow"}

  *initClass {
    ShutDown.add( { QWindow.closeAll } );
  }

  *new { arg name,
         bounds = Rect( 128, 64, 400, 400 ),
         resizable = true,
         border = true,
         server,
         scroll = false;

    var b = QWindow.flipY( bounds.asRect );

    //NOTE we omit server, which is only for compatibility with SwingOSC
    ^super.newCustom( [name, b, resizable, border, scroll] )
          .initQWindow(name);
  }

  initQWindow { arg argName;
    name = argName;
  }

  *screenBounds {
    ^this.prScreenBounds( Rect.new );
  }

  *closeAll {
    allWindows.copy.do { |win| win.close };
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

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  drawHook_ { arg aFunction;
    drawHook = aFunction;
    this.setProperty( \paint, true );
    this.refresh;
  }

  refresh {
    if( drawHook.notNil ) {
      this.invokeMethod( \rebuildPen );
    };
    super.refresh;
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

  draw {
    drawHook.value( this );
  }
}
