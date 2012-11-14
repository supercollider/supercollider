QFont {
  classvar <defaultSansFace, <defaultSerifFace, <defaultMonoFace;

  var <>name, <size, <>bold, <>italic, <hasPointSize=false;
  /* ----------------- class -------------------*/

  *implementsClass {^'Font'}

  *initClass {
    defaultSansFace = this.prDefaultFamilyForStyle(0);
    defaultSerifFace = this.prDefaultFamilyForStyle(1);
    defaultMonoFace = this.prDefaultFamilyForStyle(2);
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

  *default {
    ^QFont();
  }

  *default_ { arg font;
    this.setDefault(font);
  }

  *setDefault { arg font, class;
    this.prSetDefault(font, class !? {class.qtClass});
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

  setDefault { arg class;
    QFont.setDefault(this, class);
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

  // support Qt GUI in combination with CocoaDocument
  asSCFont {
    var cocoa, scfont;
    cocoa = GUI.get(\cocoa);
    if(cocoa.notNil) {
      scfont = cocoa.font.new(name, size);
      if(bold){scfont = scfont.boldVariant};
      ^scfont;
    }{
      ^nil
    }
  }

  /* ------------------ private -----------------*/

  *prSetDefault { arg font, className;
    _QFont_SetDefaultFont
    ^this.primitiveFailed;
  }

  *prDefaultFamilyForStyle { arg style = -1;
    _QFont_DefaultFamilyForStyle
    ^this.primitiveFailed;
  }
}
