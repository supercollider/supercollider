+ Server {
	meter { |numIns, numOuts|
		if(numIns.notNil or: numOuts.notNil and: { serverMeter.notNil }) { serverMeter.close };
		if(serverMeter.isNil or: { serverMeter.isClosed }) {
			serverMeter = ServerMeter(this, numIns, numOuts)
		} {
			serverMeter.window.front
		}
	}
}
