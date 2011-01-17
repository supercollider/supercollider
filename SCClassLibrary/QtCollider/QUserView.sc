QUserView : QView {
  var <drawFunc, <background;

  *qtClass { ^"QcCustomPainted" }

  *new { arg parent, bounds;
    var me = super.new(parent,bounds);
    // If bounds argument nil, ask subclass for default size.
    // FIXME Could be more optimal if there was a way to get sizeHint from new instance before
    // QWidget constructor and therefore pass to it already adjusted bounds;
    var sizeHint = me.sizeHint;
    if( bounds.isNil ) {
      me.resizeTo( sizeHint.x, sizeHint.y );
    };
    me.canFocus = true;
    ^me;
  }

  sizeHint {
    ^Point(0,0);
  }

  drawFunc_ { arg aFunction;
    if( drawFunc.notNil ) { this.disconnectFunction( 'painting()', drawFunc ) };
    this.connectToFunction( 'painting()', aFunction, true );
    if( drawFunc.isNil ) { this.setProperty( \paint, true ) };
    drawFunc = aFunction;
  }

  refresh {
    if( drawFunc.notNil ) {
      this.invokeMethod( \repaint );
    };
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }
}
