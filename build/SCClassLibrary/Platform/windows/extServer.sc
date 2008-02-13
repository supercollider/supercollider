+ Server { 

	// no internal server on windows
	*initClass {
		Class.initClassTree(ServerOptions);
		named = IdentityDictionary.new;
		set = Set.new;
		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		program = "scsynth.exe";
	}

	quitInProcess {}	// no internal server, this should be a no-op
	
	prepareForRecord { arg path;
		if (path.isNil) { 
			path = defaultRecDir ++ "/SC_" ++ Main.elapsedTime 
							++ "." ++ recHeaderFormat; 
		};
		recordBuf = Buffer.alloc(this, 65536, recChannels,
			{arg buf; buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true);},
			this.options.numBuffers + 1); // prevent buffer conflicts by using reserved bufnum
		SynthDef("server-record", { arg bufnum;
			DiskOut.ar(bufnum, In.ar(0, recChannels)) 
		}).send(this);
		// cmdPeriod support
		CmdPeriod.add(this);
	}
}
