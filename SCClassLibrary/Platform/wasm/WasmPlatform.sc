WasmPlatform : UnixPlatform {
	*initClass {
		Class.initClassTree(Server);
		// SC 3.15 bumped default mem size to 256 mb which is too heavy for wasm
		// reduce it to 8192 kb
		Server.default.options.memSize = 8192;
	}

	name { ^"wasm".asSymbol; }
	version { ^"wasm" }

	startup {
		helpDir = this.systemAppSupportDir++"/Help";

		// Server setup. first looks for scsynth in the dir containing the sclang executable;
		// if nothing is found, falls back to PATH
		Server.program = "PATH=$(dirname $(readlink /proc/$PPID/exe)):$PATH; exec scsynth";

		// Score setup
		Score.program = Server.program;

		// load user startup file
		this.loadStartupFiles;
	}

	initPlatform {
		super.initPlatform;
	}

	killProcessByID { |pid, force = true, subprocesses = true|
		"killProcessById is not implemented".warn;
	}
}

JS {
	*runCode {|code|
		^this.prRunCode(code);
	}

	*prRunCode {|code|
		_Wasm_runCode
		^this.primitiveFailed;
	}

	*ideSend {|payload|
		_Wasm_ideSend
		^this.primitiveFailed;
	}
}
