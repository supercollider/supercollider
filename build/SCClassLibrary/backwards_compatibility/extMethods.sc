

+ Mix {

	*ar { arg array;
		^this.new(array);
	}
	*arFill { arg n,func;
		^this.fill(n,func);
	}
}

+ SimpleNumber {
	// rgb() still will not work in variable declarations in classes
	// because rgb is no longer a literal
	
	// move them to init methods
	
	rgb { arg green,blue;
		^Color(this / 255.0,green / 255.0, blue / 255.0)
	}
	
}