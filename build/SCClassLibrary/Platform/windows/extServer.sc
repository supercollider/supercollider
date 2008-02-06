+ Server { 

	// no internal server on windows
	*initClass {
		Class.initClassTree(ServerOptions);
		named = IdentityDictionary.new;
		set = Set.new;
		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		program = "scsynth.exe";
	}

}

