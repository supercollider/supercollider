QFont {
  classvar defaultSansFace, defaultSerifFace, defaultMonoFace;
  var <>name, <size, <>bold, <>italic, <hasPointSize=false;
  /* ----------------- class -------------------*/

  *initClass {

  }

  *new { arg name, size, bold = false, italic = false, usePointSize = false;
    ^super.newCopyArgs( name, size, bold, italic, usePointSize );
  }

  *availableFonts {
    _Qt_AvailableFonts
  }

  *antiAliasing_ { arg flag;
  }

  *smoothing_ { arg flag;
  }

  *defaultSansFace {
    ^"sans-serif";
  }

  *defaultSerifFace {
    ^"serif";
  }

  *defaultMonoFace {
    ^"monospace";
  }

  *default {
    ^QFont();
  }

  *monospace {|size, bold = false, italic = false, usePointSize = false|
     ^this.new(this.defaultMonoFace, size, bold, italic, usePointSize)
  }

  *serif {|size, bold = false, italic = false, usePointSize = false|
     ^this.new(this.defaultSerifFace, size, bold, italic, usePointSize)
  }

  *sansSerif {|size, bold = false, italic = false, usePointSize = false|
     ^this.new(this.defaultSansFace, size, bold, italic, usePointSize)
  }

  /* ------------------instance------------------*/

  setDefault {
  }

  boldVariant {
    ^this.class.new( name, size, true, italic );
  }

  size_ { arg pixelSize; this.pixelSize_( pixelSize ); }

  pixelSize_ { arg pixelSize;
    size = pixelSize;
    hasPointSize = false;
  }

  pointSize_ { arg pointSize;
    size = pointSize;
    hasPointSize = true;
  }

  pixelSize { ^( if(hasPointSize){nil}{size} ) }

  pointSize { ^( if(hasPointSize){size}{nil} ) }

  /* ------------------ private -----------------*/

}
