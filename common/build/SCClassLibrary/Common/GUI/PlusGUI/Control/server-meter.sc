+ Server {
	meter {
		var clzz;
		^if( GUI.id == \swing and: {Ê\JSCPeakMeter.asClass.notNil }, {
			\JSCPeakMeter.asClass.meterServer( this );
		}, { ServerMeter(this) });
	}
}
