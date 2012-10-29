QSoundFileView : QView {

  var <>soundfile;
  var <metaAction;
  var <>elasticMode; // NOTE: no-op, only for compatibility
  var curDoneAction;

  *qtClass { ^'QcWaveform' }

  load { arg filename, startframe, frames, block, doneAction;
    if( filename.isString && filename != "" ) {
      if( curDoneAction.notNil )
        { this.disconnectFunction( 'loadingDone()', curDoneAction ) };
      if( doneAction.notNil ) {
        this.connectFunction( 'loadingDone()', doneAction );
      };

      curDoneAction = doneAction;

      if( startframe.notNil && frames.notNil ) {
        this.invokeMethod( \load, [filename, startframe.asInteger, frames.asInteger] );
      }{
        this.invokeMethod( \load, filename );
      }
    }
  }

  alloc { arg frames, channels=1, samplerate=44100;
    this.invokeMethod( \allocate, [frames.asInteger, channels.asInteger, samplerate.asInteger] );
  }

  data_ { arg data;
    this.setData(data);
  }

  setData { arg data, block, startframe=0, channels=1, samplerate=44100;
    if( data.isKindOf(DoubleArray).not and: {data.isKindOf(FloatArray).not} )
      { data = data.as(DoubleArray) };
    this.invokeMethod( \load, [data, startframe, channels, samplerate] );
  }

  set { arg offset=0, data;
    if( data.isKindOf(DoubleArray).not and: {data.isKindOf(FloatArray).not} )
      { data = data.as(DoubleArray) };
    this.invokeMethod( \write, [data, offset.asInteger] );
  }

  readFile { arg aSoundFile, startframe, frames, block, closeFile, doneAction;
    this.load( aSoundFile.path, startframe, frames, block, doneAction );
  }

  read { arg startframe, frames, block, closeFile, doneAction;
    if( soundfile.notNil ) {
      this.readFile( soundfile, startframe, frames, block, nil, doneAction );
    };
  }

  readFileWithTask { arg aSoundFile, startframe, frames, block, doneAction, showProgress;
    this.readFile( aSoundFile, startframe, frames, block, nil, doneAction );
  }

  readWithTask { arg startframe, frames, block, doneAction, showProgress;
    this.read( startframe, frames, block, nil, doneAction );
  }

  drawsWaveForm { ^this.getProperty( \drawsWaveform ); }

  drawsWaveForm_ { arg boolean; this.setProperty( \drawsWaveform, boolean ); }

  waveColors { ^this.getProperty( \waveColors ) }
  waveColors_ { arg colors; this.setProperty( \waveColors, colors ) }

  //// Info

  startFrame { ^this.getProperty( \startFrame ); }

  numFrames { ^this.getProperty( \frames ); }

  scrollPos { ^this.getProperty( \scrollPos ); } // a fraction of the full scrolling range

  viewFrames { ^this.getProperty( \viewFrames ); }

  readProgress { ^this.getProperty( \readProgress ); }

  //// Navigation

  zoom { arg factor; this.invokeMethod( \zoomBy, factor.asFloat ); }

  zoomToFrac { arg fraction; this.invokeMethod( \zoomTo, fraction.asFloat ); }

  zoomAllOut { this.invokeMethod( \zoomAllOut ); }

  zoomSelection { arg selection;
    if( selection.isNil ) { selection = this.currentSelection };
    this.invokeMethod( \zoomSelection, selection );
  }

  scrollTo { arg fraction; // a fraction of the full scrolling range
    this.setProperty( \scrollPos, fraction );
  }

  scroll { arg fraction; // a fraction of the visible range
    var frames = this.viewFrames * fraction + this.getProperty(\viewStartFrame);
    this.setProperty( \viewStartFrame, frames );
  }

  scrollToStart { this.invokeMethod( \scrollToStart ); }

  scrollToEnd { this.invokeMethod( \scrollToEnd ); }

  xZoom { ^this.getProperty( \xZoom ); }

  xZoom_ { arg seconds; this.setProperty( \xZoom, seconds ); }

  yZoom { ^this.getProperty( \yZoom ); }

  yZoom_ { arg factor; this.setProperty( \yZoom, factor.asFloat ); }

  //// Selections

  selections { ^this.getProperty( \selections ); }

  currentSelection { ^this.getProperty( \currentSelection ); }

  currentSelection_ { arg index; this.setProperty( \currentSelection, index ); }

  selection { arg index; ^this.invokeMethod( \selection, index, true ); }

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

  selectAll { arg index; this.setSelection( index, [0, this.numFrames] ); }

  selectNone { arg index; this.setSelection( index, [0, 0] ); }


  setEditableSelectionStart { arg index, editable; ^this.nonimpl("setEditableSelectionStart"); }

  setEditableSelectionSize { arg index, editable; ^this.nonimpl("setEditableSelectionSize"); }

  setSelectionColor { arg index, color; this.invokeMethod( \setSelectionColor, [index,color] ); }


  selectionStartTime { arg index; ^this.nonimpl("selectionStartTime"); }

  selectionDuration { arg index; ^this.nonimpl("selectionDuration"); }


  readSelection { arg block, closeFile; ^this.nonimpl("readSelection"); }

  readSelectionWithTask { ^this.nonimpl("readSelectionWithTask"); }

  // cursor

  timeCursorOn { ^this.getProperty( \cursorVisible ); }
  timeCursorOn_ { arg flag; this.setProperty( \cursorVisible, flag ) }

  timeCursorEditable { ^this.getProperty( \cursorEditable ); }
  timeCursorEditable_ { arg flag; this.setProperty( \cursorEditable, flag ) }

  timeCursorPosition { ^this.getProperty( \cursorPosition ); }
  timeCursorPosition_ { arg frame; this.setProperty( \cursorPosition, frame ) }

  // grid

  gridOn { ^this.getProperty( \gridVisible ); }
  gridOn_ { arg flag; this.setProperty( \gridVisible, flag ) }

  gridResolution { ^this.getProperty( \gridResolution ) }
  gridResolution_ { arg seconds; this.setProperty( \gridResolution, seconds ) }

  gridOffset { ^this.getProperty( \gridOffset ) }
  gridOffset_ { arg seconds; this.setProperty( \gridOffset, seconds ) }

  // colors

  peakColor { ^this.getProperty(\peakColor) }
  peakColor_ { arg color; this.setProperty(\peakColor, color) }

  rmsColor { ^this.getProperty(\rmsColor) }
  rmsColor_ { arg color; this.setProperty(\rmsColor, color) }

  timeCursorColor { ^this.getProperty( \cursorColor ); }
  timeCursorColor_ { arg color; this.setProperty( \cursorColor, color ) }

  gridColor { ^this.getProperty( \gridColor ) }
  gridColor_ { arg color; this.setProperty( \gridColor, color ) }

  // actions

  metaAction_ { arg action;
    this.manageFunctionConnection( metaAction, action, 'metaAction()' );
    metaAction = action
  }
}
