+ UGen {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom;
		^SynthDef.wrap({ var bus, numChannels, rate, scope, screenBounds, x, y, right;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, Server.internal, numChannels);
			switch(rate, 
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);
			
			{
				screenBounds = SCWindow.screenBounds;
				x = 544 + (Stethoscope.ugenScopes.size * 222);
				right = x + 212;
				y = floor(right / screenBounds.width) * 242 + 10;
				if(right > screenBounds.right, {x = floor(right%screenBounds.width / 222) 
					* 222; }); 
				scope = Stethoscope.new(Server.internal, numChannels, bus.index, bufsize, zoom, 
					rate);
				scope.window.name_(name.asString).bounds_(Rect(x, y, 212, 212));
				Stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free; 
					Stethoscope.ugenScopes.remove(scope)};
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}

+ Array {
	scope { arg name = "UGen Scope", bufsize = 4096, zoom;
		^SynthDef.wrap({ var bus, numChannels, rate, scope, screenBounds, x, y, right;
			numChannels = this.numChannels;
			rate = this.rate;
			bus = Bus.perform(rate, Server.internal, numChannels);
			switch(rate, 
				\audio, {Out.ar(bus.index, this)},
				\control, {Out.kr(bus.index, this)}
			);
			
			{
				screenBounds = SCWindow.screenBounds;
				x = 544 + (Stethoscope.ugenScopes.size * 222);
				right = x + 212;
				y = floor(right / screenBounds.width) * 242 + 10;
				if(right > screenBounds.right, {x = floor(right%screenBounds.width / 222) 
					* 222; }); 
				scope = Stethoscope.new(Server.internal, numChannels, bus.index, bufsize, zoom, 
					rate);
				scope.window.name_(name.asString).bounds_(Rect(x, y, 212, 212));
				Stethoscope.ugenScopes.add(scope);
				scope.window.onClose = { scope.free; bus.free; 					Stethoscope.ugenScopes.remove(scope)};
				CmdPeriod.doOnce({ {scope.window.close}.defer });
			}.defer(0.001);
			this;
		})
	}

}