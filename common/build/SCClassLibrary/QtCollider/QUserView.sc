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
    drawFunc = aFunction;
    this.setProperty( \paint, true );
    this.refresh;
  }

  refresh {
    if( drawFunc.notNil ) {
      this.invokeMethod( \rebuildPen );
    };
    super.refresh;
  }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  draw {
    drawFunc.value( this );
  }
}
