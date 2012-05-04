QTopScrollWidget : QScrollCanvas {
  var <>win;
  doDrawFunc { win.drawFunc.value(win); }
}

QScrollTopView : QScrollView {
  var >window;

  *qtClass {^'QcScrollWindow'}

  *new { arg win, name, bounds, resizable, border;
    ^super.newCustom([name, bounds, resizable, border])
          .initQScrollTopView(win);
  }

  initQScrollTopView { arg win;
    var cnv;
    window = win;
    // NOTE: The canvas widget must not be a QView, so that asking its
    // children for parent will skip it and hit this view instead.
    cnv = QTopScrollWidget.new;
    cnv.win = win;
    this.canvas = cnv;
  }

  bounds {
    var r;
    r = this.getProperty( \geometry );
    ^r.moveTo(0,0);
  }

  bounds_ { arg rect;
    var rNew = rect.asRect;
    var rOld = this.getProperty( \geometry );
    this.setProperty( \geometry, rOld.resizeTo( rNew.width, rNew.height ) );
  }

  drawingEnabled_ { arg bool; canvas.setProperty( \drawingEnabled, bool ); }

  findWindow { ^window; }
}

QTopView : QView {
  var >window;

  *qtClass {^'QcWindow'}

  *new { arg win, name, bounds, resizable, border;
    ^super.newCustom([name, bounds, resizable, border])
          .initQTopView(win);
  }

  initQTopView { arg win; window = win; }

  bounds {
    var r;
    r = this.getProperty( \geometry );
    ^r.moveTo(0,0);
  }

  bounds_ { arg rect;
    var rNew = rect.asRect;
    var rOld = this.getProperty( \geometry );
    this.setProperty( \geometry, rOld.resizeTo( rNew.width, rNew.height ) );
  }

  drawingEnabled_ { arg bool; this.setProperty( \drawingEnabled, bool ); }

  findWindow { ^window; }

  doDrawFunc { window.drawFunc.value(window) }
}

QWindow
{
  classvar <allWindows, <>initAction;

  var resizable, <drawFunc;
  var <view;

  //TODO
  var <>acceptsClickThrough=false;
  var <currentSheet;

  *implementsClass {^'Window'}

  *screenBounds {
    _QWindow_ScreenBounds
  }

  *availableBounds {
    _QWindow_AvailableGeometry
  }

  *closeAll {
    allWindows.copy.do { |win| win.close };
  }

  /* NOTE:
    - 'server' is only for compatibility with SwingOSC
    - all args have to be of correct type for QWidget constructor to match!
  */
  *new { arg name="",
         bounds,
         resizable = true,
         border = true,
         server,
         scroll = false;

    if( bounds.isNil ) {
      bounds = Rect(0,0,400,400).center_( QWindow.availableBounds.center );
    }{
      bounds = QWindow.flipY( bounds.asRect );
    };
    ^super.new.initQWindow( name, bounds, resizable, border, scroll );
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
    if( resizable.not ) { view.fixedSize = r.size }
  }

  bounds {
    ^QWindow.flipY( view.getProperty( \geometry ) );
  }

  setInnerExtent { arg w, h;
    // bypass this.bounds, to avoid QWindow flipping the y coordinate
    var r = view.getProperty(\geometry );
    view.setProperty(\geometry, r.resizeTo( w, h ); )
  }

  background { ^view.background; }

  background_ { arg aColor; view.background = aColor; }

  drawFunc_ { arg aFunction;
    view.drawingEnabled = aFunction.notNil;
    drawFunc = aFunction;
  }

  setTopLeftBounds{ arg rect, menuSpacer=45;
    view.setProperty( \geometry, rect.moveBy( 0, menuSpacer ) );
  }

  onClose { ^view.onClose }
  onClose_ { arg func; view.onClose_(func) }

  // TODO
  addToOnClose{ arg function; }
  removeFromOnClose{ arg function; }

  //------------------- simply redirected to QView ---------------------//

  sizeHint { ^view.sizeHint }
  minSizeHint { ^view.minSizeHint }
  alpha_ { arg value; view.alpha_(value); }
  addFlowLayout { arg margin, gap; ^view.addFlowLayout( margin, gap ); }
  close { view.close; }
  isClosed { ^view.isClosed; }
  visible { ^view.visible; }
  visible_ { arg boolean; view.visible_(boolean); }
  front { view.front; }
  fullScreen { view.fullScreen; }
  endFullScreen { view.endFullScreen; }
  minimize { view.minimize; }
  unminimize { view.unminimize; }
  name { ^view.name; }
  name_ { arg string; view.name_(string); }
  refresh { view.refresh; }
  userCanClose { ^view.userCanClose; }
  userCanClose_ { arg boolean; view.userCanClose_( boolean ); }
  alwaysOnTop { ^view.alwaysOnTop; }
  alwaysOnTop_ { arg boolean; view.alwaysOnTop_(boolean); }
  layout { ^view.layout; }
  layout_ { arg layout; view.layout_(layout); }
  toFrontAction_ { arg action; view.toFrontAction_(action) }
  toFrontAction { ^view.toFrontAction }
  endFrontAction_ { arg action; view.endFrontAction_(action) }
  endFrontAction { ^view.endFrontAction }
  acceptsMouseOver { ^view.acceptsMouseOver }
  acceptsMouseOver_ { arg flag; view.acceptsMouseOver_(flag) }

  // ---------------------- private ------------------------------------

  *flipY { arg aRect;
    var flippedTop = QWindow.screenBounds.height - aRect.top - aRect.height;
    ^Rect( aRect.left, flippedTop, aRect.width, aRect.height );
  }

  *addWindow { arg window;
    allWindows = allWindows.add( window );
  }

  *removeWindow { arg window;
    allWindows.remove( window );
  }
}
