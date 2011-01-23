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
    super.refresh;
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }
}
