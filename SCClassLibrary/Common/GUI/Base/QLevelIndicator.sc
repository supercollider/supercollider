LevelIndicator : View {

	*qtClass { ^'QcLevelIndicator' }

	value {
		^this.getProperty(\value)
	}
	value_ { |val|
		this.setProperty(\value, val)
	}
	valueAction_ { |val|
		this.setProperty(\value, val);
		this.doAction
	}

	warning_ { |val|
		this.setProperty(\warning, val)
	}

	critical_ { |val|
		this.setProperty(\critical, val)
	}

	style_ { |val|
		this.setProperty(\style, QLevelIndicatorStyle(val))
	}

	stepWidth_{ |val|
		this.setProperty(\stepWidth, val)
	}

	background { ^this.getProperty(\grooveColor) }
	background_ { |color| this.setProperty(\grooveColor, color) }

	meterColor_{ |color| this.setProperty(\meterColor, color) }
	warningColor_{ |color| this.setProperty(\warningColor, color) }
	criticalColor_{ |color| this.setProperty(\criticalColor, color) }

	numSteps_ { |val|
		var stepWidth, length = max(this.bounds.width, this.bounds.height);
		stepWidth = length / val;
		stepWidth = stepWidth - if(stepWidth < 3, 1, 2);
		this.stepWidth = stepWidth
	}

	image_ { |image|
		this.nonimpl("image")
	}

	numTicks_ { |number|
		this.setProperty(\ticks, number)
	}

	numMajorTicks_ { |number|
		this.setProperty(\majorTicks, number)
	}

	drawsPeak_ { |bool|
		this.setProperty(\drawPeak, bool)
	}

	peakLevel_ { |val|
		this.setProperty(\peak, val)
	}

	*meterServer { |server|
		var window, inmeters, outmeters, inresp, outresp, insynth, outsynth, func;
		var numIns, numOuts;
		var view, viewWidth, meterWidth = 15, gapWidth = 4;
		var updateFreq = 10, dBLow = -80;
		var numRMSSamps, numRMSSampsRecip;


		numIns = server.options.numInputBusChannels;
		numOuts = server.options.numOutputBusChannels;
		viewWidth = (numIns + numOuts + 2) * (meterWidth + gapWidth);
		window = Window.new(server.name ++ " levels (dBFS)", Rect(5, 305, viewWidth + 20, 230));
		window.view.background = Color.grey(0.4);

		view = CompositeView(window, Rect(10, 25, viewWidth, 180));
		view.addFlowLayout(0 @ 0, gapWidth @ gapWidth);

		// dB scale
		UserView(view, Rect(0, 0, meterWidth, 195)).drawFunc_({
			Pen.color = Color.white;
			Pen.font = Font("Helvetica", 10, true);
			Pen.stringCenteredIn("0", Rect(0, 0, meterWidth, 12));
			Pen.stringCenteredIn("-80", Rect(0, 170, meterWidth, 12))
		});

		if(numIns > 0) {
			// ins
			StaticText(window, Rect(10, 5, 100, 15))
			.font_(Font("Helvetica", 10, true))
			.stringColor_(Color.white)
			.string_("Inputs");
			inmeters = Array.fill(numIns, { |i|
				var comp;
				comp = CompositeView(view, Rect(0, 0, meterWidth, 195)).resize_(5);
				StaticText(comp, Rect(0, 180, meterWidth, 15))
				.font_(Font("Helvetica", 9, true))
				.stringColor_(Color.white)
				.string_(i.asString);
				LevelIndicator(comp, Rect(0, 0, meterWidth, 180)).warning_(0.9).critical_(1.0)
				.drawsPeak_(true)
				.numTicks_(9)
				.numMajorTicks_(3)
			});

			// divider
			UserView(view, Rect(0, 0, meterWidth, 180)).drawFunc_({
				Pen.color = Color.white;
				Pen.line(((meterWidth + gapWidth) * 0.5) @ 0, ((meterWidth + gapWidth) * 0.5) @ 180);
				Pen.stroke
			})
		};

		// outs
		StaticText(window, Rect(10 + if(numIns > 0, ((numIns + 2) * (meterWidth + gapWidth)), 0), 5, 100, 15))
		.font_(Font("Helvetica", 10, true))
		.stringColor_(Color.white)
		.string_("Outputs");
		outmeters = Array.fill(numOuts, { |i|
			var comp;
			comp = CompositeView(view, Rect(0, 0, meterWidth, 195));
			StaticText(comp, Rect(0, 180, meterWidth, 15))
			.font_(Font("Helvetica", 9, true))
			.stringColor_(Color.white)
			.string_(i.asString);
			LevelIndicator(comp, Rect(0, 0, meterWidth, 180)).warning_(0.9).critical_(1.0)
			.drawsPeak_(true)
			.numTicks_(9)
			.numMajorTicks_(3)
		});

		window.front;

		func = {
			numRMSSamps = server.sampleRate / updateFreq;
			numRMSSampsRecip = 1 / numRMSSamps;
			if(numIns > 0) {
				inresp = OSCFunc({ |msg, t|
					{
						try {
							msg.copyToEnd(3).pairsDo({ |val, peak, i|
								var meter;
								i = i * 0.5;
								meter = inmeters[i];
								meter.value = (val.max(0.0) * numRMSSampsRecip).sqrt.ampdb.linlin(dBLow, 0, 0, 1);
								meter.peakLevel = peak.ampdb.linlin(dBLow, 0, 0, 1)
					}) } }.defer
				}, ("/" ++ server.name ++ "InLevels").asSymbol, server.addr).fix
			};
			outresp = OSCFunc({ |msg, t|
				{
					try {
						msg.copyToEnd(3).pairsDo({ |val, peak, i|
							var meter;
							i = i * 0.5;
							meter = outmeters[i];
							meter.value = (val.max(0.0) * numRMSSampsRecip).sqrt.ampdb.linlin(dBLow, 0, 0, 1);
							meter.peakLevel = peak.ampdb.linlin(dBLow, 0, 0, 1)
				}) } }.defer
			}, ("/" ++ server.name ++ "OutLevels").asSymbol, server.addr).fix;
			server.bind({
				if(numIns > 0) {
					insynth = SynthDef(server.name ++ "InputLevels", {
						var in, imp;
						in = In.ar(NumOutputBuses.ir, numIns);
						imp = Impulse.ar(updateFreq);
						SendReply.ar(imp, "/" ++ server.name ++ "InLevels",
							// do the mean and sqrt clientside to save CPU
							[
								RunningSum.ar(in.squared, numRMSSamps),
								Peak.ar(in, Delay1.ar(imp)).lag(0, 3)
							]
							.flop.flat
						)
					}).play(RootNode(server), nil, \addToHead)
				};

				outsynth = SynthDef(server.name ++ "OutputLevels", {
					var in, imp;
					in = In.ar(0, numOuts);
					imp = Impulse.ar(updateFreq);
					SendReply.ar(imp, "/" ++ server.name ++ "OutLevels",
						// do the mean and sqrt clientside to save CPU
						[
							RunningSum.ar(in.squared, numRMSSamps),
							Peak.ar(in, Delay1.ar(imp)).lag(0, 3)
						].flop.flat
					)
				}).play(RootNode(server), nil, \addToTail)
			})
		};

		window.onClose_({
			if(server.options.numInputBusChannels > 0) { inresp.clear };
			outresp.clear;
			insynth.free;
			outsynth.free;
			ServerTree.remove(func)
		});

		ServerTree.add(func);
		if(server.serverRunning, func) // otherwise starts when booted
	}
}
