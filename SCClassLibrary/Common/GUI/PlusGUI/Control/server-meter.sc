+ Server {
	meter { |numIns, numOuts|
		^if( GUI.id == \swing and: { \JSCPeakMeter.asClass.notNil }, {
			\JSCPeakMeter.asClass.meterServer( this );
		}, { ServerMeter(this, numIns, numOuts) });
	}
}
