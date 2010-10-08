QtGUI {

  *initClass {
    GUI.add( this );
  }

  *id { ^\qt }

  *start{
    prStart( this );
    GUI.fromID(\qt);
  }

  *prStart{ arg me;
    _QtGUI_Start
    ^this.primitiveFailed;
  }

  *stop { }

  ///////////////////// redirection of GUI classes /////////////////////////////

  *view { ^QView }
  *compositeView { ^QView }
  *hLayoutView { ^QHLayout }
  *vLayoutView { ^QVLayout }
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
  *envelopeView { ^this.notImplemented( "EnvelopeView"); }
  *tabletView { ^this.notImplemented( "TabletView"); }
  *soundFileView { ^this.notImplemented( "SoundFileView"); }
  *movieView { ^this.notImplemented( "MovieView"); }

  *userView { ^QUserView }

  *pen { ^QPen; }
  *font { ^QFont }

  *dragSource { ^this.notImplemented( "DragSource"); }
  *dragSink { ^this.notImplemented( "DragSink"); }
  *dragBoth { ^this.notImplemented( "DragBoth"); }

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

  *prStringBounds { arg aString, aFont, aRect;
    _Qt_StringBounds
    ^this.primitiveFailed
  }
}
