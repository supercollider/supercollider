+ UGen {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom;
		var gui;
		gui = GUI.current;
		^SynthDef.wrap({ var bus, numChannels, rate, scope;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, Server.internal, numChannels);
			switch(rate,
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);

			{
				scope = gui.stethoscope.new( gui.stethoscope.defaultServer, numChannels, bus.index, bufsize, zoom,
					rate);
				scope.window.name_(name.asString).bounds_( gui.stethoscope.tileBounds);
				gui.stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free;
					gui.stethoscope.ugenScopes.remove(scope)};
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}

+ Array {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom;
		var gui;
		gui = GUI.current;
		^SynthDef.wrap({ var bus, numChannels, rate, scope;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, Server.internal, numChannels);
			switch(rate,
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);

			{
				scope = gui.stethoscope.new( gui.stethoscope.defaultServer, numChannels, bus.index, bufsize, zoom,
					rate);
				scope.window.name_(name.asString).bounds_( gui.stethoscope.tileBounds);
				gui.stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free; 					gui.stethoscope.ugenScopes.remove(scope)};
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}