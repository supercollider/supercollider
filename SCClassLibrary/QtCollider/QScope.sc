QScope : QView {
  var <bufnum, <style=0, <xZoom=1.0, <yZoom=1.0, <x=0.0, <y=0.0;
  var <waveColors;

  *qtClass { ^'QcScope' }

  bufnum_ { arg anInt;
    bufnum = anInt;
    this.setProperty( \bufferNumber, anInt );
  }

  style_ { arg anInt;
    style = anInt;
    this.setProperty( \style, anInt );
  }

  xZoom_ { arg aFloat;
    xZoom = aFloat;
    this.setProperty( \xZoom, aFloat );
  }

  yZoom_ { arg aFloat;
    yZoom = aFloat;
    this.setProperty( \yZoom, aFloat );
  }

  x_ { arg aFloat;
    x = aFloat;
    this.setProperty( \xOffset, aFloat );
  }

  y_ { arg aFloat;
    y = aFloat;
    this.setProperty( \yOffset, aFloat );
  }

  waveColors_ { arg aColorArray;
    waveColors = aColorArray;
    this.setProperty( \waveColors, aColorArray );
  }
}
