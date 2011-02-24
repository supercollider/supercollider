QtGUI {

  *initClass {
    GUI.add( this );
  }

  *id { ^\qt }

  *stop { }

  *debugLevel_ { arg level;
    _QtGUI_SetDebugLevel
    ^this.primitiveFailed;
  }

  ///////////////////// redirection of GUI classes /////////////////////////////

  *view { ^QView }
  *compositeView { ^QView }
  *hLayoutView { ^QHLayoutView }
  *vLayoutView { ^QVLayoutView }
  *window { ^QWindow }
  *scrollView { ^QScrollArea; }

  *staticText { ^QStaticText }
  *button { ^QButton; }
  *textField { ^QTextField }
  *numberBox { ^QNumberBox }
  *slider { ^QSlider }
  *multiSliderView { ^QMultiSliderView }
  *rangeSlider { ^QRangeSlider }
  *slider2D { ^QSlider2D }
  *tabletSlider2D { ^this.notImplemented( "TabletSlider2D"); }
  *knob { ^QKnob; }
  *listView { ^QListView }
  *popUpMenu { ^QPopUpMenu }
  *textView { ^QTextView; }
  *freqScope { ^QFreqScopeWindow }
  *freqScopeView { ^QFreqScope }
  *scopeView { ^QScopeView }
  *stethoscope { ^QStethoscope }
  *soundFileView { ^QSoundFileView }
  *envelopeView { ^QEnvelopeView }
  *tabletView { ^this.notImplemented( "TabletView"); }
  *movieView { ^this.notImplemented( "MovieView"); }
  *levelIndicator { ^QLevelIndicator }

  *userView { ^QUserView }

  *pen { ^QPen; }
  *font { ^QFont }

  *dragSource { ^this.notImplemented( "DragSource"); }
  *dragSink { ^this.notImplemented( "DragSink"); }
  *dragBoth { ^this.notImplemented( "DragBoth"); }

  *dialog { ^QDialog }

  *ezScroller { ^EZScroller }
  *ezSlider { ^EZSlider }
  *ezListView { ^EZListView }
  *ezPopUpMenu { ^EZPopUpMenu}
  *ezNumber { ^EZNumber}
  *ezRanger { ^EZRanger }

  *notImplemented { arg class;
    ("QtGUI: " ++ class.asString ++ " is not implemented yet").postln;
    ^nil;
  }

  //////////////////////////////////////////////////////////////////////

  *stringBounds { arg aString, aFont;
    var bounds = this.prStringBounds( aString, aFont, Rect.new );
    bounds.left = 0;
    bounds.top = 0;
    ^bounds
  }

  *palette {
    ^this.prPalette( QPalette.new.init );
  }

  *palette_ { arg p;
    _Qt_SetGlobalPalette
    ^this.primitiveFailed;
  }

  // private ///////////////////////////////////////////////////////////

  *prPalette { arg ret;
    _Qt_GlobalPalette
    ^this.primitiveFailed;
  }

  *prStringBounds { arg aString, aFont, aRect;
    _Qt_StringBounds
    ^this.primitiveFailed
  }
}

Size {
  var <width, <height;

  *new { arg width=0.0, height=0.0;
    ^super.new.initSize( width, height );
  }

  initSize{ arg w, h;
    width = w;
    height = h;
  }

  asRect { ^Rect(0,0,width,height); }

  asPoint { ^Point(width,height); }

  asString {
    ^( "Size(" ++ width ++ ", " ++ height ++ ")" );
  }
}

+ Rect {
  asSize { ^Size( width, height ); }
}

+ Point {
  asSize { ^Size( x, y ); }
}
