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
	checkNInputs { arg n;
		this.deprecated(UGen.class.findMethod(\checkNInputs));
		^super.checkNInputs(n)
	}
	checkInputs { this.deprecated; ^this.checkNInputs(1) }
}

XFade : UGen {
	checkNInputs { arg n;
		this.deprecated(UGen.class.findMethod(\checkNInputs));
		^super.checkNInputs(n)
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
		this.deprecated(thisMethod, EZText.class.findMethod(\new));
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