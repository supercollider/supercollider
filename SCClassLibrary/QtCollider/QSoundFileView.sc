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

  startFrame { ^0 }

  numFrames { ^this.getProperty( \frames ); }

  viewFrames { ^this.getProperty( \viewFrames ); }

  readProgress { ^this.getProperty( \readProgress ); }

  scrollPos { ^this.getProperty( \scrollPos ); }

  zoom { arg factor; this.invokeMethod( \zoomBy, factor ); }

  zoomToFrac { arg fraction; this.invokeMethod( \zoomTo, fraction ); }

  zoomAllOut { this.invokeMethod( \zoomAllOut ); }

  scrollTo { arg frame;
    if( frame < 0 ) { frame = 0 };
    this.setProperty( \scrollPos, frame );
  }

  scroll { arg amount; this.scrollTo( this.scrollPos + amount ); }

  scrollToStart { this.invokeMethod( \scrollToStart ); }

  scrollToEnd { this.invokeMethod( \scrollToEnd ); }

  currentSelection { ^this.getProperty( \currentSelection ); }

  currentSelection_ { arg index; this.setProperty( \currentSelection, index ); }

  selection { arg index; ^this.invokeMethod( \selectionAt, 0, true ); }

  setSelection { arg index, selection;
    this.invokeMethod( \setSelectionAt, [index, selection] );
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

  selectionStartTime { arg index; ^this.nonimpl("selectionStartTime"); }

  selectionDuration { arg index; ^this.nonimpl("selectionDuration"); }

  selectAll { arg index; ^this.nonimpl("selectAll"); }

  selectNone { arg index; this.setSelectionSize( index, 0 ); }

  setSelectionColor { arg index, color; ^this.nonimpl("setSelectionColor"); }

  setEditableSelectionStart { arg index, editable; ^this.nonimpl("setEditableSelectionStart"); }

  setEditableSelectionSize { arg index, editable; ^this.nonimpl("setEditableSelectionSize"); }

  readSelection { arg block, closeFile; ^this.nonimpl("readSelection"); }

  readSelectionWithTask { ^this.nonimpl("readSelectionWithTask"); }
}
