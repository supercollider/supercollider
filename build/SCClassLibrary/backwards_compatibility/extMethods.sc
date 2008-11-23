

+ Mix {

	*ar { arg array;
		^this.new(array);
	}
	*arFill { arg n,func;
		^this.fill(n,func);
	}
}

+ SimpleNumber {
	// rgb() still will not work in variable declarations in classes
	// because rgb is no longer a literal
	
	// move them to init methods
	
	rgb { arg green,blue;
		^Color(this / 255.0,green / 255.0, blue / 255.0)
	}
	
}

+ Synth {
	
	*stop {
		RootNode.new.freeAll;
	}
	*play { arg func;
		Server.default.doWhenBooted({		
			func.play;
		})
	}
	play {
		//if not already playing, play
	}
	*sampleRate {
		^Server.default.sampleRate ? Server.default.options.sampleRate ? 44100.0
	}


	/** temporary testing place holder **/
	*newkv { arg defName, args, target, addAction=\addToHead;
		var synth, server, addNum, inTarget;
		inTarget = target.asTarget;
		server = inTarget.server;
		addNum = addActions[addAction];
		synth = this.basicNew(defName, server);

		if((addNum < 2), { synth.group = inTarget; }, { synth.group = inTarget.group; });
		server.sendMsg('s_new', //"s_newargs"
			defName, synth.nodeID, addNum, inTarget.nodeID,
			*(args.asOSCArgArray)
			);
		^synth
	}

}

+Node {	
	setkv { arg ... args;
		server.sendMsg(15, nodeID, *(args.asOSCArgArray));  //"/n_set"
	}

}


+RawArray {

	write { arg path;
		var file;
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

+Object { 
	asOSCArgSubArray { | array| ^array.add(this.asControlInput) }
	addAsControlInput { | array| ^array.add(this.asControlInput) }
}

+Nil {
	asOSCArgArray {}
}
	
+Array {
	asOSCArgArray {
		var array = Array(100);		// allocate a bunch of space
		this.do { | e | array = e.asOSCArgSubArray(array) };
		^array
	}
	
	asOSCArgSubArray { | array|
		array = array.add($[);
		this.do{ | e | array = e.addAsControlInput(array) };
		array.add($]);
		^array;
	}

	addAsControlInput { | array|
		this.do{ | e | array = e.addAsControlInput(array) };
		^array;
	}

}		
