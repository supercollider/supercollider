ScopeBuffer {
	var <index, <numChannels, <server;

	*alloc { arg server, numChannels=1;
		var alloc;
		server = server ? Server.default;
		alloc = server.scopeBufferAllocator.alloc(numChannels);
		if (alloc.isNil) {
			error("Meta_ScopeBuffer:alloc: failed to get a scope buffer allocated."
				+ "numChannels:" + numChannels + "server:" + server.name);
			^nil
		};
		^this.new(alloc, numChannels, server)
	}

	*new { arg index=0, numChannels=1, server;
		^super.newCopyArgs(index, numChannels, server ? Server.default)
	}

	// compatibility method
	bufnum {
		^index;
	}

	free {
		if (index.isNil) {
			(this.asString + " has already been freed").warn;
			^this
		};

		server.scopeBufferAllocator.free(index);
		index = nil;
		numChannels = nil;
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<*
			[index, numChannels, server]  <<")"
	}

	storeOn { arg stream;
		stream << this.class.name << "(" <<*
			[index, numChannels, server.asCompileString]  <<")"
	}

	== { arg that;
		^this.compareObject(that, #[\index, \numChannels, \server])
	}

	hash {
		^this.instVarHash(#[\index, \numChannels, \server])
	}

}
