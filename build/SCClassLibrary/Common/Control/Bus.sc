
Bus {

	var <rate,<index,<numChannels,<server;
	var mapSymbol;

	*control { arg server,numChannels=1;
		var alloc;
		server = server ? Server.default;
		alloc = server.controlBusAllocator.alloc(numChannels);
		if(alloc.isNil,{
			error("Meta_Bus:control: failed to get a control bus allocated."
				+ "numChannels:" + numChannels + "server:" + server.name);
			^nil
		});
		^this.new(\control,alloc,numChannels,server)
	}
	*audio { arg server,numChannels=1;
		var alloc;
		server = server ? Server.default;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil,{
			error("Meta_Bus:audio: failed to get an audio bus allocated."
			+ "numChannels:" + numChannels + "server:" + server.name);
			^nil
		});
		^this.new(\audio,alloc,numChannels,server)
	}
	*alloc { arg rate,server,numChannels=1;
		^this.perform(rate ? \audio,server,numChannels);
	}

	*new { arg rate=\audio,index=0,numChannels=2,server;
		^super.newCopyArgs(rate,index,numChannels,server ? Server.default)
	}

	set { arg ... values; // shouldn't be larger than this.numChannels
		server.sendBundle(nil,(["/c_set"]
			++ values.collect({ arg v,i; [index + i ,v] }).flat));
	}
	setMsg { arg ... values;
		^["/c_set"]
			++ values.collect({ arg v,i; [index + i ,v] }).flat
	}

	setn { arg values;
		// could throw an error if values.size > numChannels
		server.sendBundle(nil,
			["/c_setn",index,values.size] ++ values);
	}
	setnMsg { arg values;
		^["/c_setn",index,values.size] ++ values;
	}
	setAt { |offset ... values|  
		// shouldn't be larger than this.numChannels - offset
		server.sendBundle(nil,(["/c_set"]
			++ values.collect({ arg v,i; [index + offset + i ,v] }).flat));
	}
	setnAt { |offset, values| 
		// could throw an error if values.size > numChannels
		server.sendBundle(nil,
			["/c_setn",index + offset, values.size] ++ values);
	}
	setPairs { | ... pairs| 
		server.sendBundle(nil,(["/c_set"]
			++ pairs.clump(2).collect({ arg pair; [pair[0] + index, pair[1]] }).flat));
	}

	get { arg action; 
		action = action ? { |vals| "Bus % index: % values: %.\n".postf(rate, index, vals); };
		OSCpathResponder(server.addr,['/c_set',index], { arg time, r, msg;
			action.value(msg.at(2)); r.remove }).add;
		server.listSendMsg(["/c_get",index]);
	}
	getn { arg count, action;
		action = action ? { |vals| "Bus % index: % values: %.\n".postf(rate, index, vals); };
		OSCpathResponder(server.addr,['/c_setn',index],{arg time, r, msg;
			action.value(msg.copyToEnd(3)); r.remove } ).add;
		server.listSendMsg(this.getnMsg(count));
	}
	getMsg {
		^["/c_get",index];
	}
	getnMsg { arg count;
		^["/c_getn",index, count ? numChannels];
	}

	fill { arg value,numChans;
		// could throw an error if numChans > numChannels
		server.sendBundle(nil,
			["/c_fill",index,numChans,value]);
	}

	fillMsg { arg value;
		^["/c_fill",index,numChannels,value];
	}


	free {
		if(index.isNil,{ (this.asString + " has already been freed").warn; ^this });
		if(rate == \audio,{
			server.audioBusAllocator.free(index);
		},{
			server.controlBusAllocator.free(index);
		});
		index = nil;
		numChannels = nil;
		mapSymbol = nil;
	}

	// allow reallocation

	alloc {
		if(rate === 'audio', {
			index = server.audioBusAllocator.alloc(numChannels);
		}, {
			index = server.controlBusAllocator.alloc(numChannels);
		});
		mapSymbol = nil;
	}

	realloc {
		var r, n;
		if(index.notNil, {
			r = rate; n = numChannels;
			this.free;
			rate = r; numChannels = n;
			this.alloc;
		})
	}

	// alternate syntaxes
	setAll { arg value;
		this.fill(value,numChannels);
	}

	value_ { arg value;
		this.fill(value,numChannels);
	}

	printOn { arg stream;		stream << this.class.name << "(" <<*			[rate, index, numChannels, server]  <<")"	}

	storeOn { arg stream;
		stream << this.class.name << "(" <<*
			[rate.asCompileString, index, numChannels, server.asCompileString]  <<")"
	}

	== { arg aBus;
		if(aBus === this,{ ^true });
		^aBus respondsTo: #[\index, \numChannels, \rate, \server]
		and: { aBus.index == index
		and: { aBus.numChannels == numChannels
		and: { aBus.rate == rate
		and: { aBus.server === server }}}}
	}
	hash { ^index.hash bitXor: numChannels.hash bitXor: rate.hash bitXor: server.hash }

	isAudioOut { // audio interface
		^(rate === \audio and: {index < server.options.firstPrivateBus})
	}

	ar { |numChannels=(this.numChannels), offset=0|
		if(rate == \audio,{
			^In.ar(index + offset, numChannels)
		},{
			//"Bus converting control to audio rate".inform;
			^K2A.ar( In.kr(index + offset, numChannels) )
		})
	}
	kr { |numChannels=(this.numChannels), offset=0|
		if(rate == \audio,{
			^A2K.kr( In.ar(index + offset, numChannels) )
		},{
			^In.kr(index + offset, numChannels)
		})
	}

	play { arg target=0, outbus, fadeTime, addAction=\addToTail;
		if(this.isAudioOut.not,{ // returns a Synth
			^{ this.ar }.play(target, outbus, fadeTime, addAction);
		});
	}

	asUGenInput { ^this.index }
	asControlInput { ^this.index }

	asMap {
		^mapSymbol ?? {
			if(index.isNil) { MethodError("bus not allocated.", this).throw };
			mapSymbol = if(rate == \control) { "c" } { "a" };
			mapSymbol = (mapSymbol ++ index).asSymbol;
		}
	}

	// added by nescivi; gets a subbus from another bus

	*newFrom { |bus, offset, numChannels=1|
		if ( offset > bus.numChannels or: {numChannels + offset >
			bus.numChannels} )
		{
			Error( "Bus:newFrom tried to reach outside the channel range of
%".format( bus )).throw
		};
		^Bus.new( bus.rate, bus.index + offset, numChannels);
	}

	subBus{|offset, numChannels=1|
		^Bus.newFrom( this, offset, numChannels );
	}

}

