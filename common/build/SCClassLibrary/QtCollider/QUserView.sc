QUserView : QView {
  var <drawFunc, <background;

  *viewClass { ^QUserView }

  *new { arg parent, bounds;
    var me = super.new(parent,bounds);
    me.canFocus = true;
    ^me;
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
