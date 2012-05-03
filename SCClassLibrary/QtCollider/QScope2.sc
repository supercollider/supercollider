QScope2 : QView {
  var <bufnum;
  var <xZoom=1.0, <yZoom=1.0, <x=0.0, <y=0.0;
  var <waveColors;

  *qtClass { ^'QcScopeShm' }

  bufnum_ { arg anInt;
    bufnum = anInt;
    this.setProperty( \bufferNumber, anInt );
  }

  server_ { arg server;
    var port = if(server.inProcess){ thisProcess.pid }{ if(server.isLocal){server.addr.port} };
    if(port.isNil) {
      "QScope2: Can not scope on the desired server.".warn
    } {
      this.setProperty(\serverPort, port);
    };
  }

  start { this.invokeMethod(\start); }
  stop { this.invokeMethod(\stop); }

  style { ^this.getProperty(\style) }

  style_ { arg value; this.setProperty(\style, value) }

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
