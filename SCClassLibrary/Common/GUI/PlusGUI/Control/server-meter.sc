+ Server {
	meter { |numIns, numOuts, position|
		if(numIns.notNil or: numOuts.notNil and: { meter.notNil }) { meter.close };
		if(meter.isNil or: { meter.isClosed }) {
			meter = ServerMeter(this, numIns, numOuts)
		} {
			meter.window.front
		};
		position !? { meter.position_(position) };
		^meter
	}
}