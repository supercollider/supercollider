QSoundFileView : QView {

  var <>soundfile;

  *qtClass { ^"QcWaveform" }

  load { arg filename;
    this.invokeMethod( \load, filename );
  }

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

  zoom { arg factor;
    this.invokeMethod( \zoomBy, factor );
  }

  zoomToFrac { arg fraction;
    this.invokeMethod( \zoomTo, fraction );
  }

  zoomAllOut {
    this.invokeMethod( \zoomAllOut );
  }

  scrollTo { arg frame;
    if( frame < 0 ) { frame = 0 };
    this.setProperty( \scrollPos, frame );
  }

  scroll { arg amount;
    this.scrollTo( this.scrollPos + amount );
  }

  scrollToStart {
    this.invokeMethod( \scrollToStart );
  }

  scrollToEnd {
    this.invokeMethod( \scrollToEnd );
  }


}
