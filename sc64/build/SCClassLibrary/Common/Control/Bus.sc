
Bus {

	var <rate,<index,<numChannels,<server;
	
	*control { arg server,numChannels=1;
		var alloc;
		server = server ? Server.default;
		alloc = server.controlBusAllocator.alloc(numChannels);
		if(alloc.isNil,{
			error("Meta_Bus-control: failed to get a control bus allocated."
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
			error("Meta_Bus-audio: failed to get an audio bus allocated."
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
	get { arg action;
		OSCpathResponder(server.addr,['/c_set',index], { arg time, r, msg; 
			action.value(msg.at(2)); r.remove }).add;
		server.listSendMsg(["/c_get",index]);
	}	
	getn { arg count, action;
		OSCpathResponder(server.addr,['/c_setn',index],{arg time, r, msg; 
			action.value(msg.copyToEnd(3)); r.remove } ).add; 
		server.listSendMsg(["/c_getn",index, count]);
	}
	getMsg {
		^["/c_get",index];
	}
	getnMsg { arg count, action;
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
	}
	
	// allow reallocation
	
	alloc {
		if(rate === 'audio', {
			index = server.audioBusAllocator.alloc(numChannels);
		}, {
			index = server.controlBusAllocator.alloc(numChannels);
		});
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
	
	printOn { arg stream; 
		stream << this.class.name << "(" <<* [server.name,rate,index,numChannels]  <<")" 
	}
	
	== { arg aBus;
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
	
	ar {
		if(rate == \audio,{
			^In.ar(index,numChannels)
		},{
			//"Bus converting control to audio rate".inform;
			^K2A.ar( In.kr(index,numChannels) )
		})
	}
	kr {
		if(rate == \audio,{
			^A2K.kr(index,numChannels)
		},{
			^In.kr(index,numChannels)
		})
	}
	play { arg target=0, outbus, fadeTime, addAction=\addToTail;
		if(this.isAudioOut.not,{ // returns a Synth
			^{ this.ar }.play(target, outbus, fadeTime, addAction);
		});
	}
	
}

