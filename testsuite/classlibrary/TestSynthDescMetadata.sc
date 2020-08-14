TestSynthDescMetadata : UnitTest {
	const defname = \test_md;
	var dir, path, mdPath, metadata, original, oldMdPlugin;

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

		original.writeDefFile(dir);
		SynthDescLib.global.removeAt(defname);
	}

	tearDown {
		File.delete(path);
		File.delete(mdPath);
		SynthDesc.mdPlugin = oldMdPlugin;
	}

	test_SynthDesc_read_restores_metadata_in_desc_and_def {
		var synthdesc;
		synthdesc = SynthDesc.read(path, keepDefs: true)[defname];
		this.checkMetadata(synthdesc, metadata, SynthDesc);
	}

	test_SynthDescLib_read_restores_metadata_in_desc_and_def {
		var synthdesc;
		synthdesc = SynthDescLib.read(dir +/+ "*.scsyndef").global;
		synthdesc = synthdesc[defname];
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
