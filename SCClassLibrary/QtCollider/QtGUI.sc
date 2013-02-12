QtGUI {

  classvar <style;

  *initClass {
    Class.initClassTree( GUI );
    GUI.add( this );
    this.style = "Plastique";
    this.palette = QPalette.light;
  }

  *id { ^\qt }

  *stop { }

  *debugLevel {
    _QtGUI_DebugLevel
    ^this.primitiveFailed
  }

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
  *scrollView { ^QScrollView; }

  *staticText { ^QStaticText }
  *button { ^QButton; }
  *checkBox { ^QCheckBox; }
  *textField { ^QTextField }
  *numberBox { ^QNumberBox }
  *slider { ^QSlider }
  *multiSliderView { ^QMultiSliderView }
  *rangeSlider { ^QRangeSlider }
  *slider2D { ^QSlider2D }
  *tabletSlider2D { ^this.notImplemented( "TabletSlider2D"); }
  *knob { ^QKnob; }
  *listView { ^QListView }
  *treeView { ^QTreeView }
  *popUpMenu { ^QPopUpMenu }
  *textView { ^QTextView; }

  *freqScope     { ^PlusFreqScopeWindow }
  *freqScopeView { ^PlusFreqScope }
  *scopeView { ^QScope }
  *stethoscope1 { ^QStethoscope }
  *stethoscope { ^QStethoscope2 }
  *soundFileView { ^QSoundFileView }
  *envelopeView { ^QEnvelopeView }
  *tabletView { ^this.notImplemented( "TabletView"); }
  *movieView { ^this.notImplemented( "MovieView"); }
  *levelIndicator { ^QLevelIndicator }
  *webView { ^QWebView }

  *userView { ^QUserView }

  *dragSource { ^QDragSource; }
  *dragSink { ^QDragSink; }
  *dragBoth { ^QDragBoth; }

  *dialog { ^QDialog }

  *ezScroller { ^EZScroller }
  *ezSlider { ^EZSlider }
  *ezListView { ^EZListView }
  *ezPopUpMenu { ^EZPopUpMenu}
  *ezNumber { ^EZNumber}
  *ezRanger { ^EZRanger }

  *hLayout { ^QHLayout }
  *vLayout { ^QVLayout }
  *gridLayout { ^QGridLayout }
  *stackLayout { ^QStackLayout }

  *pen { ^QPen }

  *font { ^QFont }
  *image { ^this.notImplemented( "Image" ) }

  *speech { ^'Speech'.asClass }

  *notImplemented { arg class;
    //("QtGUI: " ++ class.asString ++ " is not implemented yet").postln;
    ^nil;
  }

  //////////////////////////////////////////////////////////////////////

  *cursorPosition { _Qt_CursorPosition }

  *availableStyles {
    _Qt_AvailableStyles
    ^this.primitiveFailed;
  }

  *style_ { arg styleName;
    protect { this.prSetStyle(styleName) } { |err| if(err.isNil){style = styleName} };
  }

  *stringBounds { arg aString, aFont;
    var bounds = this.prStringBounds( aString, aFont );
    bounds.left = 0;
    bounds.top = 0;
    ^bounds
  }

  *palette {
    _Qt_GlobalPalette
    ^this.primitiveFailed;
  }

  *palette_ { arg p;
    _Qt_SetGlobalPalette
    ^this.primitiveFailed;
  }

  *focusView {
    _Qt_FocusWidget
    ^this.primitiveFailed;
  }

  *selectedText {
    var view = this.focusView;
    if( view.notNil ) {
      if( view.respondsTo(\selectedText) ) { ^view.selectedText };
      if( view.respondsTo(\selectedString) ) { ^view.selectedString };
    };
    ^"";
  }

  // private ///////////////////////////////////////////////////////////

  *prStringBounds { arg aString, aFont;
    _Qt_StringBounds
    ^this.primitiveFailed
  }

  *prSetStyle { arg name;
    _Qt_SetStyle
    ^this.primitiveFailed;
  }
}
