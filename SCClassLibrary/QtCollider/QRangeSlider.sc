QRangeSlider : QAbstractStepValue {
  *qtClass { ^'QcRangeSlider' }

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQRangeSlider( bounds );
  }

  initQRangeSlider { arg bounds;
    var r;
    if( bounds.notNil ) {
      r = bounds.asRect;
      if( r.width > r.height ) {
        this.orientation_( \horizontal );
      } {
        this.orientation_( \vertical );
      }
    }
  }

  pixelStep {
    // FIXME for now we are using step instead
    ^this.step;
  }

  orientation_ { arg aSymbol;
    this.setProperty( \orientation, QOrientation(aSymbol) );
  }

  lo {
    ^this.getProperty( \loValue );
  }

  lo_ { arg aFloat;
    this.setProperty( \loValue, aFloat );
  }

  activeLo_ { arg aFloat;
    this.lo_(aFloat);
    this.doAction;
  }

  hi {
    ^this.getProperty( \hiValue );
  }

  hi_ { arg aFloat;
    this.setProperty( \hiValue, aFloat );
  }

  activeHi_ { arg aFloat;
    this.hi_(aFloat);
    this.doAction;
  }

  range {
    ^(this.hi - this.lo);
  }

  range_ { arg aFloat;
    this.hi_( this.lo + aFloat; )
  }

  activeRange_ { arg aFloat;
    this.range_(aFloat);
    this.doAction;
  }

  setSpan { arg lo, hi;
    this.lo_(lo);
    this.hi_(hi);
  }

  setSpanActive { arg lo, hi;
    this.setSpan(lo,hi);
    this.doAction;
  }

  setDeviation { arg deviation, average;
    this.lo_( average - deviation );
    this.hi_( average + deviation );
  }

  knobColor { ^this.getProperty(\knobColor) }
  knobColor_ { arg color; this.setProperty(\knobColor, color) }

  background { ^this.getProperty(\grooveColor) }
  background_ { arg color; this.setProperty(\grooveColor, color) }

  defaultGetDrag { ^Point(this.lo,this.hi); }
  defaultCanReceiveDrag { ^(QView.currentDrag.class === Point); }
  defaultReceiveDrag {
    var pt = QView.currentDrag;
    this.setSpanActive( pt.x, pt.y );
  }
}
