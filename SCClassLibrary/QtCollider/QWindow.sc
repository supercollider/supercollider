QScrollTopView : QAbstractScroll {
  var >window;

  *qtClass {^"QcWindow"}

  *new { arg win, name, bounds, resizable, border;
    ^super.newCustom([name, bounds, resizable, border, true /*scroll*/])
          .initQScrollTopView(win);
  }

  initQScrollTopView { arg win; window = win; }

  bounds {
    var r;
    r = this.getProperty( \geometry, Rect.new );
    ^r.moveTo(0,0);
  }

  bounds_ { arg rect;
    var rNew = rect.asRect;
    var rOld = this.getProperty( \geometry, Rect.new );
    this.setProperty( \geometry, rOld.resizeTo( rNew.width, rNew.height ) );
  }

  findWindow { ^window; }

  doDrawFunc { window.drawHook.value(window) }

  visibleOrigin {
    ^this.getProperty( \visibleOrigin, Point.new );
  }

  visibleOrigin_ { arg point;
    this.setProperty( \visibleOrigin, point );
  }

  innerBounds {
    ^this.getProperty( \innerBounds, Rect.new );
  }
}

QTopView : QView {
  var >window;

  *qtClass {^"QcWindow"}

  *new { arg win, name, bounds, resizable, border;
    ^super.newCustom([name, bounds, resizable, border, false /*scroll*/])
          .initQTopView(win);
  }

  initQTopView { arg win; window = win; }

  bounds {
    var r;
    r = this.getProperty( \geometry, Rect.new );
    ^r.moveTo(0,0);
  }

  bounds_ { arg rect;
    var rNew = rect.asRect;
    var rOld = this.getProperty( \geometry, Rect.new );
    this.setProperty( \geometry, rOld.resizeTo( rNew.width, rNew.height ) );
  }

  findWindow { ^window; }

  doDrawFunc { window.drawHook.value(window) }
}

QWindow
{
  classvar <allWindows, <>initAction;

  var resizable, <drawHook, <background, <onClose;
  var <view;

  //TODO
  var <>acceptsClickThrough=false, <>acceptsMouseOver=false,
      <>alwaysOnTop=false;
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

  *new { arg name,
         bounds = Rect( 128, 64, 400, 400 ),
         resizable = true,
         border = true,
         server,
         scroll = false;

    //NOTE server is only for compatibility with SwingOSC

    var b = QWindow.flipY( bounds.asRect );
    ^super.new.initQWindow( name, b, resizable, border, scroll );
  }

  //------------------------ QWindow specific  -----------------------//

  initQWindow { arg name, bounds, resize, border, scroll;
    if( scroll )
      { view = QScrollTopView.new(this,name,bounds,resize,border); }
      { view = QTopView.new(this,name,bounds,resize,border); };

    // set some necessary object vars
    resizable = resize == true;

    // allWindows array management
    QWindow.addWindow( this );
    view.connectFunction( 'destroyed()', { QWindow.removeWindow(this); }, false );

    // action to call whenever a window is created
    QWindow.initAction.value( this );
  }

  asView {
    ^view;
  }

  bounds_ { arg aRect;
    var r = QWindow.flipY( aRect.asRect );
    view.setProperty( \geometry, r );
    if( resizable.not ) {
      view.setProperty( \minimumSize, r.asSize );
      view.setProperty( \maximumSize, r.asSize );
    }
  }

  bounds {
    ^QWindow.flipY( view.getProperty( \geometry, Rect.new ) );
  }

  setInnerExtent { arg w, h;
    // bypass this.bounds, to avoid QWindow flipping the y coordinate
    var r = view.getProperty(\geometry, Rect.new );
    view.setProperty(\geometry, r.resizeTo( w, h ); )
  }

  background_ { arg aColor;
    background = aColor;
    view.setProperty( \background, aColor, true );
  }

  drawHook_ { arg aFunction;
    if( drawHook.isNil ) { view.setProperty( \drawingEnabled, true ) };
    if( aFunction.isNil ) { view.setProperty( \drawingEnabled, false ) };
    drawHook = aFunction;
  }

  setTopLeftBounds{ arg rect, menuSpacer=45;
    view.setProperty( \geometry, rect.moveBy( 0, menuSpacer ) );
  }

  onClose_ { arg func;
    if( onClose != func && onClose.notNil ) {
      view.disconnectFunction( 'destroyed()', onClose );
    };
    view.connectFunction( 'destroyed()', func, false );
    onClose = func;
  }

  // TODO
  addToOnClose{ arg function; }
  removeFromOnClose{ arg function; }

  //------------------- simply redirected to QView ---------------------//

  alpha_ { var value; view.alpha_(value); }
  addFlowLayout { arg margin, gap; ^view.addFlowLayout( margin, gap ); }
  close { view.close; }
  front { view.front; }
  fullScreen { view.fullScreen; }
  endFullScreen { view.endFullScreen; }
  isClosed { ^view.isClosed; }
  minimize { view.minimize; }
  name { ^view.name; }
  name_ { arg string; view.name_(string); }
  refresh { view.refresh; }
  userCanClose { ^view.userCanClose; }
  userCanClose_ { arg boolean; view.userCanClose_( boolean ); }
  layout { ^view.layout; }
  layout_ { arg layout; view.layout_(layout); }

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
