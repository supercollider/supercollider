QFont {
  classvar defaultSansFace, defaultSerifFace, defaultMonoFace;
  var <>name, <>size, <>bold, <>italic;
  /* ----------------- class -------------------*/

  *initClass {
    
  }
  
  *new { arg n, s, b = false, i = false;
    ^super.newCopyArgs( n, s, b, i );
  }

  *availableFonts {
  }
  
  *antiAliasing_ { arg flag;
  }
  
  *smoothing_ { arg flag;
  }

  //TODO
  *defaultSansFace {
    ^nil;
  }

  //TODO
  *defaultSerifFace {
    ^nil;
  }

  //TODO
  *defaultMonoFace {
    ^nil;
  }

  //TODO
  *default {
    ^QFont( nil, nil );
  }

  /* ------------------instance------------------*/

  setDefault {
  }
  
  boldVariant {
    ^this.class.new( name, size, true, italic );
  }

  /* ------------------ private -----------------*/

}