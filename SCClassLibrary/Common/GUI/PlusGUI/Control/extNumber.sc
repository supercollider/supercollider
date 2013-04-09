+ SimpleNumber {

	guessDecimalPlaces{
		var dec = 0;
		while( { ( ( this.round( pow(10,-1*dec) ) - this ) < 0 ) },{ dec = dec + 1; } );
		^dec
	}

}