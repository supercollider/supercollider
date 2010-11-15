ServerMeterView{

	classvar serverMeterViews, 	updateFreq = 10, dBLow = -80;

	var <view;
	var inresp, outresp, insynth, outsynth, synthFunc, responderFunc, server, numIns, numOuts, inmeters, outmeters;

	*new{ |aserver,parent,leftUp,numIns,numOuts|
		^super.new.init(aserver,parent,leftUp,numIns,numOuts)
	}

	init { arg aserver, parent, leftUp, anumIns,anumOuts;
		var innerView;

		var viewWidth, meterWidth = 15, gapWidth = 4;

		leftUp = leftUp ? (0@0);

		server = aserver;

		numIns = anumIns ?? { server.options.numInputBusChannels };
		numOuts = anumOuts ?? { server.options.numOutputBusChannels };
		viewWidth = (numIns + numOuts + 2) * (meterWidth + gapWidth);

		view = CompositeView(parent, Rect(leftUp.x,leftUp.y, viewWidth, 180) );
		view.onClose_({ this.stop });
		innerView = CompositeView(view, Rect(10,25, viewWidth, 180) );
		innerView.addFlowLayout(0@0, gapWidth@gapWidth);

		// dB scale
		UserView(innerView, Rect(0,0,meterWidth,195)).drawFunc_({
			Pen.color = Color.white;
			Pen.font = Font(Font.defaultSansFace, 10).boldVariant;
			Pen.stringCenteredIn("0", Rect(0, 0, meterWidth, 12));
			Pen.stringCenteredIn("-80", Rect(0, 170, meterWidth, 12));
		});

		(numIns > 0).if({
			// ins
			StaticText(view, Rect(10, 5, 100, 15))
				.font_(Font(Font.defaultSansFace, 10).boldVariant)
				.stringColor_(Color.white)
				.string_("Inputs");
			inmeters = Array.fill( numIns, { arg i;
				var comp;
				comp = CompositeView(innerView, Rect(0,0,meterWidth,195)).resize_(5);
				StaticText(comp, Rect(0, 180, meterWidth, 15))
					.font_(Font(Font.defaultSansFace, 9).boldVariant)
					.stringColor_(Color.white)
					.string_(i.asString);
				LevelIndicator( comp, Rect(0,0,meterWidth,180) ).warning_(0.9).critical_(1.0)
					.drawsPeak_(true)
					.numTicks_(9)
					.numMajorTicks_(3);
			});
		});

		if((numIns > 0) && (numOuts > 0)){
			// divider
			UserView(innerView, Rect(0,0,meterWidth,180)).drawFunc_({
				Pen.color = Color.white;
				Pen.line(((meterWidth + gapWidth) * 0.5)@0, ((meterWidth + gapWidth) * 0.5)@180);
				Pen.stroke;
			});
		};

		// outs
		(numOuts > 0).if({
			StaticText(view, Rect(10 + if(numIns > 0 , ((numIns + 2) * (meterWidth + gapWidth)), 0), 5, 100, 15))
				.font_(Font(Font.defaultSansFace, 10).boldVariant)
				.stringColor_(Color.white)
				.string_("Outputs");
			outmeters = Array.fill( numOuts, { arg i;
				var comp;
				comp = CompositeView(innerView, Rect(0,0,meterWidth,195));
				StaticText(comp, Rect(0, 180, meterWidth, 15))
					.font_(Font(Font.defaultSansFace, 9).boldVariant)
					.stringColor_(Color.white)
					.string_(i.asString);
				LevelIndicator( comp, Rect(0,0,meterWidth,180) ).warning_(0.9).critical_(1.0)
					.drawsPeak_(true)
					.numTicks_(9)
					.numMajorTicks_(3);
			});
		});

		this.setSynthFunc(inmeters, outmeters);
		this.start;

	}

	setSynthFunc{
		var numRMSSamps, numRMSSampsRecip;

		synthFunc = {
			//responders and synths are started only once per server
			var numIns = server.options.numInputBusChannels;
			var numOuts = server.options.numOutputBusChannels;
			numRMSSamps = server.sampleRate / updateFreq;
			numRMSSampsRecip = 1 / numRMSSamps;

			server.bind({
				(numIns > 0).if({
					insynth = SynthDef(server.name ++ "InputLevels", {
						var in, imp;
						in = In.ar(NumOutputBuses.ir, numIns);
						imp = Impulse.ar(updateFreq);
						SendReply.ar(imp, "/" ++ server.name ++ "InLevels",
							// do the mean and sqrt clientside to save CPU
							[
								RunningSum.ar(in.squared, numRMSSamps),
								Peak.ar(in, Delay1.ar(imp)).lag(0, 3)]
							.flop.flat
						);
					}).play(RootNode(server), nil, \addToHead);
				});
				(numOuts > 0).if({
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
						);
					}).play(RootNode(server), nil, \addToTail);
				});
			});
		};
	}

	startResponders{
		var numRMSSamps, numRMSSampsRecip;

		//responders and synths are started only once per server
		numRMSSamps = server.sampleRate / updateFreq;
		numRMSSampsRecip = 1 / numRMSSamps;
		(numIns > 0).if({
			inresp = OSCresponderNode(server.addr, "/" ++ server.name ++ "InLevels", { |t, r, msg|
				{try {
				msg.copyToEnd(3).pairsDo({|val, peak, i|
					var meter;
					i = i * 0.5;
					meter = inmeters[i];
					if(meter.isClosed.not){
						meter.value = (val.max(0.0) * numRMSSampsRecip).sqrt.ampdb.linlin(dBLow, 0, 0, 1);
						meter.peakLevel = peak.ampdb.linlin(dBLow, 0, 0, 1);
					}
				}) }}.defer;
			}).add;
		});
		(numOuts > 0).if({
			outresp = OSCresponderNode(server.addr, "/" ++ server.name ++ "OutLevels", { |t, r, msg|
				{try {
				msg.copyToEnd(3).pairsDo({|val, peak, i|
					var meter;
					i = i * 0.5;
					meter = outmeters[i];
					if(meter.isClosed.not){
						meter.value = (val.max(0.0) * numRMSSampsRecip).sqrt.ampdb.linlin(dBLow, 0, 0, 1);
						meter.peakLevel = peak.ampdb.linlin(dBLow, 0, 0, 1);
					}
				}) }}.defer;
			}).add;
		});
	}

	start{
		if(serverMeterViews.isNil){
			serverMeterViews = IdentityDictionary.new;
		};
		if(serverMeterViews[server].isNil){
			serverMeterViews.put(server,List[this]);
			ServerTree.add(synthFunc);
			if(server.serverRunning, synthFunc); // otherwise starts when booted
			server.doWhenBooted({this.startResponders});
		}{
			if(serverMeterViews[server].size == 0){
				ServerTree.add(synthFunc);
				if(server.serverRunning, synthFunc); // otherwise starts when booted


			};
			serverMeterViews[server].add(this);
			server.doWhenBooted({this.startResponders});
		}

	}

	stop{
		serverMeterViews[server].remove(this);

		if(serverMeterViews[server].size == 0){
			(numIns > 0).if({ insynth.free; });
			(numOuts > 0).if({outsynth.free; });
			ServerTree.remove(synthFunc);
		};

		(numIns > 0).if({ inresp.remove; });
		(numOuts > 0).if({ outresp.remove; });
	}

	remove{
	 	view.remove
	}
}

ServerMeter{

	var <window, <meterView;

	*new{ |server, numIns, numOuts|

		var window, meterView, viewWidth,meterWidth = 15, gapWidth = 4;

		numIns = numIns ?? { server.options.numInputBusChannels };
		numOuts = numOuts ?? { server.options.numOutputBusChannels };

		viewWidth = (numIns + numOuts + 2) * (meterWidth + gapWidth);

		window = Window.new(server.name ++ " levels (dBFS)", Rect(5, 305, viewWidth + 20, 230));
		window.view.background = Color.grey(0.4);

		meterView = ServerMeterView(server,window,0@0,numIns,numOuts);

		window.front;

		^super.newCopyArgs(window,meterView)

	}
}