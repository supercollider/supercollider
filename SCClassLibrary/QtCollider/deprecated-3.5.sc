+ QWindow {
	
  drawHook {
    this.deprecated(thisMethod, this.class.findMethod(\drawFunc));
    ^drawFunc
  }
  drawHook_ { |aFunction|
    this.deprecated(thisMethod, this.class.findMethod(\drawFunc_));
    this.drawFunc_(aFunction)
  }

}

+ QKnob {

  *isSquare { this.deprecated(thisMethod); ^false }
  *isSquare_ { this.deprecated(thisMethod) }

  *compactRatio { this.deprecated(thisMethod); ^0.87 }
  *compactRatio_ { this.deprecated(thisMethod) }

  skin { this.deprecated( thisMethod ); ^nil }

  color_ { arg colors;
    var p;
    this.deprecated( thisMethod );
    p = this.palette;
    p.button = colors[0];
    p.windowText = colors[1];
    p.window = colors[2];
    p.buttonText = colors[3];
    this.palette = p;
  }

  color {
    var p;
    this.deprecated( thisMethod );
    p = this.palette;
    ^[p.button, p.windowText, p.window, p.buttonText];
  }

}
