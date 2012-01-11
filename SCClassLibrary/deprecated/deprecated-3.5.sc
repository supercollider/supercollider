UI {
	classvar resetActions, shutdownActions;

	initClass {
		resetActions = Array.new;
		shutdownActions = Array.new; // braindead extensions may rely on these
	}

	*registerForReset {|aFunction|
		this.deprecated(thisMethod, OnError.class.findMethod(\add));
		OnError.add(aFunction)
	}
	*registerForShutdown {|aFunction|
		this.deprecated(thisMethod, ShutDown.class.findMethod(\add));
		ShutDown.add(aFunction)
	}
}

Pflow : FilterPattern {
	*new {
		Error("Pflow was replaced. please use Pstep instead").throw;
	}
}

Model {
	var <>dependants;

	*new {
		this.deprecated(thisMethod, Object.class.findMethod(\new));
		"NB Model's functionality is duplicated by Object".postln;
		^super.new;
	}
	changed { arg what ... moreArgs;
		dependants.do({ arg item;
			item.update(this, what, *moreArgs);
		});
	}
	addDependant { arg dependant;
		if (dependants.isNil, {
			dependants = IdentitySet.new(4);
		});
		dependants.add(dependant);
	}
	removeDependant { arg dependant;
		if (dependants.notNil, {
			dependants.remove(dependant);
		});
	}
	release {
		dependants = nil;
	}
}

Panner : MultiOutUGen {
	*new {|...x|
		this.deprecated(thisMethod);
		^super.new(*x);
	}
	*new1 {|...x|
		this.deprecated(thisMethod);
		^super.new(*x);
	}
}

XFade : UGen {
	*new {|...x|
		this.deprecated(thisMethod);
		^super.new(*x);
	}
	*new1 {|...x|
		this.deprecated(thisMethod);
		^super.new(*x);
	}
}

GetFileDialog {

	*new { arg function;
		// platform neutral (eventually)
		this.deprecated(thisMethod, File.class.findMethod(\openDialog));
		File.openDialog(nil,
			{ arg path; function.value(true,path) },
			{  function.value(false) } );
		//CocoaDialog.getPaths({arg paths; function.value(paths.first) },nil)
	}
	// warning:  doesn't handle doAction (no instance)

}


GetStringDialog  {

	*new { arg prompt,defaultString="",func;
		var b;
		this.deprecated(thisMethod);
		Sheet({ arg l;
			b = 	TextField(l,Rect(0,0,150,30));
			b.string = String.new ++ defaultString;
			b.action = {arg field; func.value(true,field.value); l.close; };
		},prompt);
		{ b.focus }.defer;
	}
}

+ Mix {

	*ar { arg array;
		this.deprecated(thisMethod, Mix.class.findMethod(\new));
		^this.new(array);
	}
	*arFill { arg n,func;
		this.deprecated(thisMethod, Mix.class.findMethod(\fill));
		^this.fill(n,func);
	}
}

+ SimpleNumber {
	// rgb() still will not work in variable declarations in classes
	// because rgb is no longer a literal

	// move them to init methods
	rgb { arg green,blue;
		this.deprecated(thisMethod, Color.class.findMethod(\new255));
		^Color(this / 255.0,green / 255.0, blue / 255.0)
	}

}

+ Synth {

	*stop {
		this.deprecated(thisMethod, Server.findMethod(\free));
		RootNode.new.freeAll;
	}
	*play { arg func;
		this.deprecated(thisMethod, Synth.class.findMethod(\new));
		Server.default.doWhenBooted({
			func.play;
		})
	}
	play {
		this.deprecated(thisMethod, Synth.class.findMethod(\new));
		//if not already playing, play
	}
	*sampleRate {
		this.deprecated(thisMethod, Server.findMethod(\sampleRate));
		^Server.default.sampleRate ? Server.default.options.sampleRate ? 44100.0
	}

}

+ RawArray {

	write { arg path;
		var file;
		this.deprecated(thisMethod, File.findMethod(\write));
		file = File.new(path, "wb");
		if (file.notNil, {
			file.write(this);
			if(file.length < this.size,{
				Error("Write file failed !!!! " ++ path).throw;
			});
			file.close;
		});
	}

}

+ ServerOptions {
	blockAllocClass {
		this.deprecated(thisMethod)
	}
	blockAllocClass_ {
		this.deprecated(thisMethod)
	}

	rendezvous_ {|bool|
		zeroConf = bool;
		this.deprecated(thisMethod, ServerOptions.findMethod(\zeroConf_))
	}

	rendezvous {|bool|
		this.deprecated(thisMethod, ServerOptions.findMethod(\zeroConf));
		^zeroConf;
	}

	numAudioBusChannels_{|num|
		this.numPrivateAudioBusChannels = num - (numInputBusChannels + numOutputBusChannels);
		this.deprecated(thisMethod);
	}
}

+ Server {
	// Buffer objects are cached in an Array for easy
	// auto buffer info updating
	addBuf { |buffer|
		this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\cache));
		buffer.cache;
	}

	freeBuf { |i|
		var	buf;
		this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\uncache));
		if((buf = Buffer.cachedBufferAt(this, i)).notNil) { buf.free };
	}

	// /b_info on the way
	// keeps a reference count of waiting Buffers so that only one responder is needed
	waitForBufInfo {
		this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\cache));
	}

	resetBufferAutoInfo {
		this.deprecated(thisMethod, Meta_Buffer.findRespondingMethodFor(\clearServerCaches));
		Buffer.clearServerCaches(this);
	}
}

+ Node {
	// map to Bus objects
	busMap { arg firstControl, aBus ... args;
		var values;
		this.deprecated(thisMethod, Node.findMethod(\map));
		values = List.new;
		args.pairsDo({ arg control, bus; values.addAll([control, bus.index, bus.numChannels])});
		server.sendMsg(48, nodeID, firstControl, aBus.index, aBus.numChannels, *values);
		//"/n_mapn"
	}
	busMapMsg { arg firstControl, aBus ... args;
		var values;
		this.deprecated(thisMethod, Node.findMethod(\mapMsg));
		values = List.new;
		args.pairsDo({ arg control, bus; values.addAll([control, bus.index, bus.numChannels])});
		^[48, nodeID, firstControl, aBus.index, aBus.numChannels] ++ values;
		//"/n_mapn"
	}
}

+ SynthDef {
	memStore { arg libname = \global, completionMsg, keepDef = true;
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\add));
		this.add(libname, completionMsg, keepDef);
	}
}

+ Env {
	shapeNumber { arg shapeName;
		this.deprecated(thisMethod, this.class.class.findMethod(\shapeNumber));
		^this.class.shapeNumber(shapeName)
	}
}

+ Help {
	searchGUI {
		this.deprecated(thisMethod, Meta_Help.findRespondingMethodFor(\gui));
		^this.gui
	}
}

+ Collection{
	occurencesOf { | obj |
		this.deprecated(thisMethod, Collection.findRespondingMethodFor(\occurrencesOf));
		^this.occurrencesOf(obj);
	}
}

+ Buffer {
	copy { arg buf, dstStartAt = 0, srcStartAt = 0, numSamples = -1;
		if(buf.notNil) {
			this.deprecated(thisMethod, this.class.findRespondingMethodFor(\copyData));
			this.copyData(buf, dstStartAt, srcStartAt, numSamples);
		} {
			^super.copy
		}
	}
}

+ ArrayedCollection {
	plotOld { arg name, bounds, discrete=false, numChannels, minval, maxval, parent, labels=true;
		var plotter, txt, chanArray, unlaced, val, window, thumbsize, zoom, width,
			layout, write=false, msresize, gui;
		var flattened;
		this.deprecated(thisMethod, ArrayedCollection.findRespondingMethodFor(\plot));

		gui = GUI.current;

		bounds = bounds ?? {
				if(parent.notNil) {
					if(parent.respondsTo(\view)) {
						parent.view.bounds
					} {
						parent.bounds
					}
			} {
				Rect(200 ,140, 705, 410);
			}
		};

		width = bounds.width - 8;

		name = name ? "plot";

		numChannels = numChannels ? this.first.size.max(1);
		flattened = if(numChannels > 1) { this.flat } { this };
		unlaced = flattened.unlace(numChannels);

		minval = if(minval.isArray) {
			numChannels.collect {|index| minval.wrapAt(index) ?? { unlaced[index].minItem } }
		} {
			{ minval ?? { flattened.minItem } }.dup(numChannels);
		};
		maxval = if(maxval.isArray) {
			numChannels.collect{|index| maxval.wrapAt(index) ?? { unlaced[index].maxItem } }
		} {
			{maxval ?? { flattened.maxItem }}.dup(numChannels);
		};

		chanArray = Array.newClear(numChannels);
		if(discrete) {
			zoom = 1;
			thumbsize = max(1.0, width / (flattened.size / numChannels));
			unlaced.do { |chan, j|
				chanArray[j] = chan.linlin( minval[j], maxval[j], 0.0, 1.0 );
			};
		} {
			zoom = (width / (flattened.size / numChannels));
			thumbsize = 1;
			unlaced.do { |chan, j|
				val = Array.newClear(width);
				width.do { arg i;
					var x;
					x = chan.blendAt(i / zoom);
					val[i] = x.linlin(minval[j], maxval[j], 0.0, 1.0);
				};
				chanArray[j] = val;
			}
		};
		window = parent ?? { gui.window.new( name, bounds ) };

		layout = gui.vLayoutView.new(window,
			if(parent.notNil) {
				Rect(bounds.left + 4, bounds.top + 4, bounds.width - 10, bounds.height - 10)
			} {
				Rect(4, 4, bounds.width - 10, bounds.height - 10)
			}
		).resize_(5);

		if(labels) {
			txt = gui.staticText.new(layout, Rect( 8, 0, width, 18))
					.string_("index: 0, value: " ++ flattened[0].asString);
		};

		numChannels.do { |i|
			plotter = gui.multiSliderView.new(layout,
				Rect(0, 0,
					// compensate for the text
					layout.bounds.width, layout.bounds.height - if(labels, {26}, {0})
				))
				.readOnly_(true)
				.drawLines_(discrete.not)
				.drawRects_(discrete)
				.indexThumbSize_(thumbsize)
				.valueThumbSize_(1)
				.background_(Color.white)
				.colors_(Color.black, Color.blue(1.0,1.0))
				.action_({|v|
					var curval;
					curval = v.currentvalue.linlin(0.0, 1.0, minval[i], maxval[i]);

					if(labels){
						txt.string_("index: " ++ (v.index / zoom).roundUp(0.01).asString ++
						", value: " ++ curval);
					};
					if(write) { flattened[(v.index / zoom).asInteger * numChannels + i ]  = curval };
				})
				.keyDownAction_({ |v, char|
					if(char === $l) { write = write.not; v.readOnly = write.not;  };
				})
				.value_(chanArray[i])
				.elasticMode_(1);
			if(numChannels > 1) { plotter.resize_(5) };
		};

		^window.tryPerform(\front) ?? { window }
	}

	plot2 {|name, bounds, discrete=false, numChannels, minval, maxval|
		this.deprecated(thisMethod, ArrayedCollection.findRespondingMethodFor(\plot));
		^this.plot(name, bounds, discrete, numChannels, minval, maxval)
	}
}

+ Function {
	plotOld { arg duration	= 0.01, server, bounds, minval = -1.0, maxval = 1.0, parent, labels=true;
		var gui;
		this.deprecated(thisMethod, Function.findRespondingMethodFor(\plot));
		gui = GUI.current;
		this.loadToFloatArray(duration, server, { |array, buf|
			var numChan;
			numChan = buf.numChannels;
			{
				GUI.use( gui, {
					array.plotOld(bounds: bounds, numChannels: numChan, minval: minval, maxval: maxval,
						parent: parent, labels: labels)
				});
			}.defer;
		})
	}
	plot2 {|duration = 0.01, server, bounds, minval, maxval|
		this.deprecated(thisMethod, Function.findRespondingMethodFor(\plot));
		^this.plot(duration, server, bounds, minval, maxval)
	}
}

+ Wavetable {
	plotOld { arg name, bounds, minval, maxval, parent, labels=true;
		this.deprecated(thisMethod, Wavetable.findRespondingMethodFor(\plot));
		^this.asSignal.plotOld(name, bounds, minval: minval, maxval: maxval, parent: parent, labels: labels);
	}

	plot2 { |name, bounds, minval, maxval|
		this.deprecated(thisMethod, Wavetable.findRespondingMethodFor(\plot));
		^this.plot(name, bounds, minval, maxval)
	}
}

+ Buffer {
	plotOld { arg name, bounds, minval = -1.0, maxval = 1.0, parent, labels=true;
		var gui;
		gui = GUI.current;
		this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\plot));
		this.loadToFloatArray(action: { |array, buf|
			{
				GUI.use( gui, {
					array.plotOld(name, bounds, numChannels: buf.numChannels, minval: minval, maxval: maxval, parent: parent, labels: labels);
				});
			}.defer;
		});
	}
	plot2 { |name, bounds, minval, maxval|
		this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\plot));
		^this.plot(name, bounds, minval, maxval)
	}
}

+ Env {
	plot2 { |size = 400, bounds, minval, maxval|
		this.deprecated(thisMethod, Env.findRespondingMethodFor(\plot));
		^this.plot(size, bounds, minval, maxval)
	}

	plotOld { arg size = 400, bounds, minval, maxval, parent;
		this.deprecated(thisMethod, Env.findRespondingMethodFor(\plot));
		this.asSignal(size).plotOld(bounds: bounds, minval: minval, maxval: maxval, parent: parent);
	}
}

+ AbstractFunction{
	plotGraphOld { arg n=500, from = 0.0, to = 1.0, name, bounds, discrete = false,
				numChannels, minval, maxval, parent, labels = true;
		var array, res;
		this.deprecated(thisMethod, AbstractFunction.findRespondingMethodFor(\plotGraph));
		array = Array.interpolation(n, from, to);
		res = array.collect { |x| this.value(x) };
		res.plotOld(name, bounds, discrete, numChannels, minval, maxval, parent, labels)
	}

	plotGraph2 { arg n=500, from = 0.0, to = 1.0, name, bounds, discrete = false,
		numChannels, minval, maxval, parent, labels = true;
		this.deprecated(thisMethod, AbstractFunction.findRespondingMethodFor(\plotGraph));
		this.plotGraph(n, from, to, name, bounds, discrete, numChannels, minval, maxval, parent, labels)
	}
}

+ Main {
	
	recvOSCfunc { this.deprecated(thisMethod); ^recvOSCfunc; }
	
	recvOSCfunc_ { |newFunc|
		this.deprecated(thisMethod, Main.findMethod(\addOSCFunc));
		recvOSCfunc = newFunc;	
	}
	
}
