
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
		var	lib = SynthDescLib.all[libname] ?? {
				Error("library" + libname  + "not found").throw
			},
			desc = this.asSynthDesc(libname, keepDef);
		lib.servers.do({ |server|
			server.value.sendBundle(nil, ["/d_recv", this.asBytes] ++ completionMsg)
		});
	}
	
	asSynthDesc { |libname, keepDef = true|
		var	lib = SynthDescLib.all[libname] ?? {
				Error("library" + libname  + "not found").throw
			},
			stream = CollStream(this.asBytes), desc;
		SynthDesc.readFile(stream, keepDef, lib.synthDescs);
		desc = lib[name.asSymbol];
		if(keepDef) {
			desc.def = this;
		};
		if(metadata.notNil) { desc.metadata = metadata };
		^desc
	}
}
