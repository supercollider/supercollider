+ Color {
	setStroke {
		_Color_SetStroke
		^this.primitiveFailed
	}
	setFill {
		_Color_SetFill
		^this.primitiveFailed
	}
	set {
		this.setStroke.setFill;
	}
}
