Bus {

	var <rate, <index, <numChannels, <server;
	var mapSymbol;

	*control { |server, numChannels = 1|
		var alloc;
		server = server ? Server.default;
		alloc = server.controlBusAllocator.alloc(numChannels);
		if(alloc.isNil) {
			error("Meta_Bus:control: failed to get a control bus allocated."
					+ "numChannels:" + numChannels + "server:" + server.name);
			^nil
		};
		^this.new(\control, alloc, numChannels, server)
	}

	*audio { |server, numChannels = 1|
		var alloc;
		server = server ? Server.default;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil) {
			error("Meta_Bus:audio: failed to get an audio bus allocated."
					+ "numChannels:" + numChannels + "server:" + server.name);
			^nil
		};
		^this.new(\audio, alloc, numChannels, server)
	}

	*alloc { |rate, server, numChannels = 1|
		^this.perform(rate ? \audio, server, numChannels)
	}

	*new { |rate = \audio, index = 0, numChannels = 2, server|
		^super.newCopyArgs(rate, index, numChannels, server ? Server.default)
	}

	*newFrom { |bus, offset, numChannels = 1|
		if(offset > bus.numChannels or: { numChannels + offset > bus.numChannels }) {
			Error("Bus:newFrom tried to reach outside the channel range of %".format(bus)).throw
		};
		^this.new(bus.rate, bus.index + offset, numChannels, bus.server)
	}

	isSettable {
		^rate != \audio
	}

	set { |... values| // shouldn't be larger than this.numChannels
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable) {
			server.sendBundle(nil, (["/c_set"]
					++ values.collect { |v, i| [index + i, v] }.flat))
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setMsg { |... values|
		if(index.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable) {
			^["/c_set"] ++ values.collect { |v, i| [index + i, v] }.flat
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setn { |values|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		// could throw an error if values.size > numChannels
		if(this.isSettable) {
			server.sendBundle(nil,
				["/c_setn", index, values.size] ++ values)
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setnMsg { |values|
		if(index.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable) {
			^["/c_setn", index, values.size] ++ values
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setAt { |offset ... values|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		// shouldn't be larger than this.numChannels - offset
		if(this.isSettable) {
			server.sendBundle(nil, (["/c_set"]
					++ values.collect { |v, i| [index + offset + i, v] }.flat))
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setnAt { |offset, values|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		// could throw an error if values.size > numChannels
		if(this.isSettable) {
			server.sendBundle(nil,
				["/c_setn", index + offset, values.size] ++ values)
		} {
			error("Cannot set an audio rate bus")
		}
	}

	setPairs { |... pairs|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable) {
			server.sendBundle(nil, (["/c_set"]
					++ pairs.clump(2).collect { |pair| [pair[0] + index, pair[1]] }.flat))
		} {
			error("Cannot set an audio rate bus")
		}
	}

	get { |action|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(numChannels == 1) {
			action = action ? { |vals| "Bus % index: % value: %.\n".postf(rate, index, vals) };
			OSCFunc({ |message|
				// The response is of the form [/c_set, index, value].
				// We want "value," which is at index 2.
				action.value(message[2])
			}, \c_set, server.addr, argTemplate: [index]).oneShot;
			server.listSendMsg([\c_get, index])
		} {
			this.getn(numChannels, action)
		}
	}

	getn { |count, action|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		action = action ? { |vals| "Bus % index: % values: %.\n".postf(rate, index, vals) };
		OSCFunc({ |message|
			// The response is of the form [/c_set, index, count, ...values].
			// We want the values, which are at indexes 3 and above.
			action.value(message[3..])
		}, \c_setn, server.addr, argTemplate: [index]).oneShot;
		server.listSendMsg([\c_getn, index, count ? numChannels])
	}

	getMsg {
		if(index.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\c_get, index]
	}

	getnMsg { |count|
		if(index.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^[\c_getn, index, count ? numChannels]
	}

	getSynchronous {
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable.not) {
			Error("Bus-getSynchronous only works for control-rate busses").throw
		} {
			^server.getControlBusValue(index)
		}
	}

	getnSynchronous { |count|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable.not) {
			Error("Bus-getnSynchronous only works for control-rate busses").throw
		} {
			^server.getControlBusValues(index, count ? numChannels)
		}
	}

	setSynchronous { |... values|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable.not) {
			Error("Bus-setSynchronous only works for control-rate busses").throw
		} {
			if(values.size == 1) {
				server.setControlBusValue(index, values[0])
			} {
				server.setControlBusValues(index, values)
			}
		}
	}

	setnSynchronous { |values|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isSettable.not) {
			Error("Bus-setnSynchronous only works for control-rate busses").throw
		} {
			server.setControlBusValues(index, values)
		}
	}

	fill { |value, numChans|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		// could throw an error if numChans > numChannels
		server.sendBundle(nil, ["/c_fill", index, numChans, value])
	}

	fillMsg { |value|
		if(index.isNil) { Error("Cannot construct a % for a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		^["/c_fill", index, numChannels, value]
	}

	free { |clear = false|
		if(index.isNil) { (this.class.name ++ " has already been freed").warn; ^this };
		if(rate == \audio) {
			server.audioBusAllocator.free(index)
		} {
			server.controlBusAllocator.free(index);
			if(clear) { this.setAll(0) }
		};
		index = nil;
		numChannels = nil;
		mapSymbol = nil
	}

	// allow reallocation

	alloc {
		if(rate === 'audio') {
			index = server.audioBusAllocator.alloc(numChannels)
		} {
			index = server.controlBusAllocator.alloc(numChannels)
		};
		mapSymbol = nil
	}

	realloc {
		var r, n;
		if(index.notNil) {
			r = rate;
			n = numChannels;
			this.free;
			rate = r;
			numChannels = n;
			this.alloc
		} {
			Error("Cannot % a % that has been freed".format(thisMethod.name, this.class.name)).throw
		}
	}

	// alternate syntaxes
	setAll { |value|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		this.fill(value, numChannels)
	}

	value_ { |value|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		this.fill(value, numChannels)
	}

	printOn { |stream|
		stream << this.class.name << "(" <<*
			[rate, index, numChannels, server] << ")"
	}

	storeOn { |stream|
		stream << this.class.name << "(" <<*
			[rate.asCompileString, index, numChannels, server.asCompileString] << ")"
	}

	== { |aBus|
		^this.compareObject(aBus, #[\index, \numChannels, \rate, \server])
	}

	hash {
		^this.instVarHash(#[\index, \numChannels, \rate, \server])
	}

	isAudioOut { // audio interface
		^(rate === \audio and: { index < server.options.firstPrivateBus })
	}

	ar { |numChannels = (this.numChannels), offset = 0|
		if(rate == \audio) {
			^In.ar(index + offset, numChannels)
		} {
			^K2A.ar(In.kr(index + offset, numChannels))
		}
	}

	kr { |numChannels = (this.numChannels), offset = 0|
		if(rate == \audio) {
			^A2K.kr(In.ar(index + offset, numChannels))
		} {
			^In.kr(index + offset, numChannels)
		}
	}

	play { |target = 0, outbus, fadeTime, addAction = \addToTail|
		if(index.isNil) { Error("Cannot call % on a % that has been freed".format(thisMethod.name, this.class.name)).throw };
		if(this.isAudioOut.not) {
			// returns a Synth
			^{ this.ar }.play(target, outbus, fadeTime, addAction)
		}
	}

	asUGenInput { ^this.index }
	asControlInput { ^this.index }

	asMap {
		^mapSymbol ?? {
			if(index.isNil) { MethodError("bus not allocated.", this).throw };
			mapSymbol = if(rate == \control) { "c" } { "a" };
			mapSymbol = (mapSymbol ++ index).asSymbol
		}
	}

	subBus { |offset, numChannels = 1|
		^this.class.newFrom(this, offset, numChannels)
	}

}
