TestSynthDesc : UnitTest {
	test_SynthDesc_read_restores_metadata_in_desc_and_def {
		var package = this.setupMetadataTests;
		var synthdesc;

		if(package.error.notNil) {
			this.failed(thisMethod, "Error writing SynthDef: '%'".format(package.error.errorString));
			^this.cleanupMetadataTests(package);
		};

		synthdesc = SynthDesc.read(package.path, keepDefs: true)[\test_md];
		if(synthdesc.isNil) {
			this.failed(thisMethod, "SynthDesc was not read from disk");
			^this.cleanupMetadataTests(package);
		};

		this.checkMetadata(synthdesc, package.metadata, SynthDesc);

		this.cleanupMetadataTests(package);
	}

	test_SynthDescLib_read_restores_metadata_in_desc_and_def {
		var package = this.setupMetadataTests;
		var synthdesc;

		if(package.error.notNil) {
			this.failed(thisMethod, "Error writing SynthDef: '%'".format(package.error.errorString));
			^this.cleanupMetadataTests(package);
		};

		synthdesc = SynthDescLib.read(package.dir +/+ "*.scsyndef").global;
		synthdesc = synthdesc[\test_md];
		if(synthdesc.isNil) {
			this.failed(thisMethod, "SynthDesc was not read from disk");
			^this.cleanupMetadataTests(package);
		};

		this.checkMetadata(synthdesc, package.metadata, SynthDescLib);

		this.cleanupMetadataTests(package);
	}

	setupMetadataTests {
		var package = IdentityDictionary.new.know_(true);
		package.dir = PathName.tmp;
		package.path = package.dir +/+ "test_md.scsyndef";
		package.mdPath = package.dir +/+ "test_md.txarcmeta";
		package.metadata = (a: 1);
		package.original = SynthDef(\test_md, {
			Out.ar(0, DC.ar(0))
		}).metadata_(package.metadata);

		package.oldMdPlugin = SynthDesc.mdPlugin;
		SynthDesc.mdPlugin = TextArchiveMDPlugin;

		try {
			package.original.writeDefFile(package.dir);
		} { |err|
			package.error = err
		};
		SynthDescLib.global.removeAt(\test_md);
		^package
	}

	checkMetadata { |synthdesc, metadata, class|
		var found = true;

		this.assertEquals(
			synthdesc.metadata,
			metadata,
			"% should restore metadata when reading from disk".format(class.name)
		);

		// def metadata has extra elements; check key by key
		metadata.keysValuesDo { |key, value|
			if(synthdesc.def.metadata[key] != value) { found = false };
		};
		this.assert(found,
			"% should restore metadata into its SynthDef when reading from disk".format(class.name)
		);
	}

	cleanupMetadataTests { |package|
		File.delete(package.path);
		File.delete(package.mdPath);
		SynthDesc.mdPlugin = package.oldMdPlugin;
	}
}
