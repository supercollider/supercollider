QSoundFileView : QView {

  var <>soundfile;

  *qtClass { ^"QcWaveform" }

  load { arg filename; this.invokeMethod( \load, filename ); }

  readFile { arg aSoundFile;
    var path = soundfile.path;
    if( path.isString && path != "" ) {
      this.load( path );
    }
  }

  read {
    if( soundfile.notNil ) { this.readFile( soundfile ) }
  }

  readFileWithTask { arg aSoundFile; this.readFile( aSoundFile ); }

  readWithTask { this.read }

  drawsWaveform { ^this.getProperty( \drawsWaveform ); }

  drawsWaveform_ { arg boolean; this.setProperty( \drawsWaveform, boolean ); }

  //// Info

  startFrame { ^0 }

  numFrames { ^this.getProperty( \frames ); }

  viewFrames { ^this.getProperty( \viewFrames ); }

  readProgress { ^this.getProperty( \readProgress ); }

  scrollPos { ^this.getProperty( \scrollPos ); }


  //// Navigation

  zoom { arg factor; this.invokeMethod( \zoomBy, factor ); }

  zoomToFrac { arg fraction; this.invokeMethod( \zoomTo, fraction ); }

  zoomAllOut { this.invokeMethod( \zoomAllOut ); }

  zoomSelection { arg selection;
    if( selection.isNil ) { selection = this.currentSelection };
    this.invokeMethod( \zoomSelection, selection );
  }

  scrollTo { arg pos; // absolute. from 0 to 1
    var frame = pos * (this.numFrames - this.viewFrames);
    this.setProperty( \scrollPos, frame );
  }

  scroll { arg amount; this.scrollTo( this.scrollPos + amount ); }

  scrollToStart { this.invokeMethod( \scrollToStart ); }

  scrollToEnd { this.invokeMethod( \scrollToEnd ); }

  xZoom { ^this.getProperty( \xZoom ); }

  xZoom_ { arg seconds; this.setProperty( \xZoom, seconds ); }

  yZoom { ^this.getProperty( \yZoom ); }

  yZoom_ { arg factor; this.setProperty( \yZoom, factor.asFloat ); }

  //// Selections

  currentSelection { ^this.getProperty( \currentSelection ); }

  currentSelection_ { arg index; this.setProperty( \currentSelection, index ); }

  selection { arg index; ^this.invokeMethod( \selection, 0, true ); }

  setSelection { arg index, selection;
    this.invokeMethod( \setSelection, [index, selection] );
  }

  selectionStart { arg index;
    var sel = this.selection( index );
    ^sel.at(0);
  }

  setSelectionStart { arg index, frame;
    var sel = this.selection( index );
    sel.put( 0, frame );
    this.setSelection( index, sel );
  }

  selectionSize { arg index;
    var sel = this.selection( index );
    ^sel.at(1);
  }

  setSelectionSize { arg index, frames;
    var sel = this.selection( index );
    sel.put( 1, frames );
    this.setSelection( index, sel );
  }

  selectAll { arg index; this.setSelection( this.currentSelection, [0, this.numFrames] ); }

  selectNone { arg index; this.setSelection( this.currentSelection, [0, 0] );  }


  setEditableSelectionStart { arg index, editable; ^this.nonimpl("setEditableSelectionStart"); }

  setEditableSelectionSize { arg index, editable; ^this.nonimpl("setEditableSelectionSize"); }

  setSelectionColor { arg index, color; this.invokeMethod( \setSelectionColor, [index,color] ); }


  selectionStartTime { arg index; ^this.nonimpl("selectionStartTime"); }

  selectionDuration { arg index; ^this.nonimpl("selectionDuration"); }


  readSelection { arg block, closeFile; ^this.nonimpl("readSelection"); }

  readSelectionWithTask { ^this.nonimpl("readSelectionWithTask"); }

  // cursor

  timeCursorOn_ { arg flag; this.setProperty( \cursorVisible, flag ) }
  timeCursorEditable_ { arg flag; this.setProperty( \cursorEditable, flag ) }
  timeCursorPosition_ { arg frame; this.setProperty( \cursorPosition, frame ) }
  timeCursorColor_ { arg color; this.setProperty( \cursorColor, color ) }

  // grid

  gridOn_ { arg flag; this.setProperty( \gridVisible, flag ) }
  gridResolution_ { arg seconds; this.setProperty( \gridResolution, seconds ) }
  gridOffset_ { arg seconds; this.setProperty( \gridOffset, seconds ) }
  gridColor_ { arg color; this.setProperty( \gridColor, color ) }
}
