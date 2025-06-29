+ Server {
	meter { |numIns, numOuts, position|
		if(serverMeter.isNil or: { serverMeter.isClosed }) {
			serverMeter = ServerMeter(this, numIns, numOuts)
		} {
			serverMeter.window.front
		};
		position !? { serverMeter.position_(position) };
		^serverMeter
	}
}
