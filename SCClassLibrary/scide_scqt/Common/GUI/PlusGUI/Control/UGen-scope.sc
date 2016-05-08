+ UGen {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom = 1.0;
		var server = Server.default;
		^SynthDef.wrap({ var bus, numChannels, rate, scope;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, server, numChannels);
			switch(rate,
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);

			{
				scope = Stethoscope.new( server, numChannels, bus.index, bufsize, zoom, rate );
				scope.window.name_(name.asString).bounds_( Stethoscope.tileBounds);
				Stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free;
					Stethoscope.ugenScopes.remove(scope) };
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}

+ Array {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom = 1.0;
		var server = Server.default;
		^SynthDef.wrap({ var bus, numChannels, rate, scope;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, server, numChannels);
			switch(rate,
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);

			{
				scope = Stethoscope.new( server, numChannels, bus.index, bufsize, zoom, rate );
				scope.window.name_(name.asString).bounds_( Stethoscope.tileBounds);
				Stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free;
					Stethoscope.ugenScopes.remove(scope) };
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}
