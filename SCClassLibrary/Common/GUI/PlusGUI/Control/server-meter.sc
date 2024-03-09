+ Server {
	meter { |numIns, numOuts, position = nil|
		if(position == nil) {
			if(numIns.notNil or: numOuts.notNil and: { serverMeter.notNil }) { serverMeter.close };
			if(serverMeter.isNil or: { serverMeter.isClosed }) {
				serverMeter = ServerMeter(this, numIns, numOuts)
			} {
				serverMeter.window.front
			}
		} {
			var width, win;
			if(numIns.notNil or: numOuts.notNil and: { serverMeter.notNil }) { win.close };
			numIns = if(numIns.isNil) { this.options.numInputBusChannels };
			numOuts = if(numOuts.isNil) { this.options.numOutputBusChannels };
			width = ServerMeterView.getWidth(numIns, numOuts);
			if(serverMeter.isNil or: { serverMeter.isClosed }) {
				serverMeter = ServerMeter(this, numIns, numOuts)
			};
			serverMeter.window.bounds_(Rect(position.x, position.y, width, ServerMeterView.height)).front
		}
	}
}
