QHelpBrowser : QView {

  classvar <instance;

  *qtClass {^"QcHelpBrowser"}

  *new { arg parent, bounds;
    ^super.new(parent,bounds).initQHelpBrowser;
  }

  *newUnique {
    var bounds;
    if( instance.isNil ) {
      bounds = Rect(0,0,800,600);
      bounds = bounds.moveToPoint( bounds.centerIn( QWindow.screenBounds ) );
      instance = this.new( nil, bounds );
      instance.onClose = { instance = nil; }
    };
    ^instance;
  }

  initQHelpBrowser {
  }

  load { arg url;
    this.invokeMethod( \load, url );
  }

  home_ { arg url;
    this.invokeMethod( \setHome, url );
  }
}
