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

}
