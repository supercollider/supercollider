
// methods for in-memory SynthDesc building from a SynthDef
// implemented as a method of SynthDef
// because someday there might be a subclass of SynthDef that would require different code

+ SynthDesc {
	*newFrom { |synthdef|
		^synthdef.asSynthDesc
	}
}

+ SynthDescLib {
	add { |synthdesc|
		synthDescs.put(synthdesc.name.asSymbol, synthdesc);
	}
	addServer { |server|
		servers = servers.asArray.add(server);
	}
	removeServer { |server|
		servers.tryPerform(\remove, server);
	}
}

+ SynthDef {
		// dir argument is not needed because no file will be written
	memStore { arg libname=\global, completionMsg, keepDef = true;
		var	lib, desc = this.asSynthDesc(libname, keepDef);
		libname ?? { libname = \global };
		lib = SynthDescLib.all[libname] ?? {
			Error("library" + libname  + "not found").throw
		};
		lib.servers.do({ |server|
			server.value.sendBundle(nil, ["/d_recv", this.asBytes] ++ completionMsg)
		});
	}
	
	asSynthDesc { |libname=\global, keepDef = true|
		var	lib, stream = CollStream(this.asBytes), desc;
		libname ?? { libname = \global };
		lib = SynthDescLib.all[libname] ?? {
			Error("library" + libname  + "not found").throw
		};
		SynthDesc.readFile(stream, keepDef, lib.synthDescs);
		desc = lib[name.asSymbol];
		if(keepDef) {
			desc.def = this;
		};
		if(metadata.notNil) { desc.metadata = metadata };
		^desc
	}
}
