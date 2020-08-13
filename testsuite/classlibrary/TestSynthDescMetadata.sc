TestSynthDescMetadata : UnitTest {
	var dir, path, mdPath, metadata, original, oldMdPlugin, error;
	var defname = \test_md;

	setUp {
		dir = PathName.tmp;
		path = dir +/+ defname ++ ".scsyndef";
		mdPath = dir +/+ defname ++ ".txarcmeta";
		metadata = (a: 1);
		original = SynthDef(defname, {
			Out.ar(0, DC.ar(0))
		}).metadata_(metadata);

		oldMdPlugin = SynthDesc.mdPlugin;
		SynthDesc.mdPlugin = TextArchiveMDPlugin;

		try {
			original.writeDefFile(dir);
		} { |err|
			error = err
		};
		SynthDescLib.global.removeAt(defname);
	}

	tearDown {
		File.delete(path);
		File.delete(mdPath);
		SynthDesc.mdPlugin = oldMdPlugin;
	}

	test_SynthDesc_read_restores_metadata_in_desc_and_def {
		var synthdesc;

		// currently we can't report this in setUp
		// because UnitTest 'currentMethod' is set AFTER setUp
		if(error.notNil) {
			^this.failed(thisMethod, "Error writing SynthDef: '%'".format(error.errorString));
		};

		synthdesc = SynthDesc.read(path, keepDefs: true)[\test_md];
		if(synthdesc.isNil) {
			^this.failed(thisMethod, "SynthDesc was not read from disk");
		};

		this.checkMetadata(synthdesc, metadata, SynthDesc);
	}

	test_SynthDescLib_read_restores_metadata_in_desc_and_def {
		var synthdesc;

		if(error.notNil) {
			^this.failed(thisMethod, "Error writing SynthDef: '%'".format(error.errorString));
		};

		synthdesc = SynthDescLib.read(dir +/+ "*.scsyndef").global;
		synthdesc = synthdesc[\test_md];
		if(synthdesc.isNil) {
			^this.failed(thisMethod, "SynthDesc was not read from disk");
		};

		this.checkMetadata(synthdesc, metadata, SynthDescLib);
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
}
