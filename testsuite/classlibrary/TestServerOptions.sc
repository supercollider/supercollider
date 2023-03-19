TestServerOptions : UnitTest {

	test_bindAddress_nonDefaultValue {
		var serverOptions, optionsString;
		serverOptions = ServerOptions();
		serverOptions.bindAddress = "0.0.0.0";
		optionsString = serverOptions.asOptionsString.split($ );
		this.assert(
			optionsString.find(["-B", serverOptions.bindAddress]).notNil,
			"Setting bindAddress to 0.0.0.0 creates a -B option"
		);
	}

	test_bindAddress_defaultValue {
		var serverOptions, optionsString;
		serverOptions = ServerOptions();
		serverOptions.bindAddress = "127.0.0.1";
		optionsString = serverOptions.asOptionsString.split($ );
		this.assert(
			optionsString.find(["-B", serverOptions.bindAddress]).isNil,
			"Setting bindAddress to default value of 127.0.0.1 does not add -B to options string"
		);
	}

}
