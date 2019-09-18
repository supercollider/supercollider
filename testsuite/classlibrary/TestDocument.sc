TestDocument : UnitTest {
	// autorun doesn't have access to this object's scope
	// need a public place to communicate the result
	classvar <>success;

	test_new_document_runs_initAction {
		var success = false, doc, save;
		if(Platform.ideName == "scqt") {
			save = Document.initAction;
			protect {
				Document.initAction = { success = true };
				doc = Document.new;
				0.1.wait;
				doc.close;
			} {
				Document.initAction = save;
			};
			this.assert(success, "Document.new should fire Document.initAction function");
		} {
			this.assert(false, "Cannot test Document.new behavior in a non-ScIDE session");
		};
	}

	test_open_autorun_document_runs_code {
		var path = PathName.tmp +/+ "autoRunTest%.scd".format(UniqueID.next),
		file, doc;
		if(Platform.ideName == "scqt") {
			if(File.exists(path)) {
				this.assert(false, "Document autoRun test file path should not exist: '%'".format(path));
			} {
				file = File(path, "w");
				if(file.isOpen) {
					protect {
						file << "/*RUN*/\nTestDocument.success = true\n";
						file.close;
						success = false;
						doc = Document.open(path);
						0.1.wait;
						doc.close;
						this.assert(success, "Document.open should run autoRun /*RUN*/ documents");
					} {
						file.close;
						File.delete(path);
					};
				} {
					this.assert(false, "Document autoRun test could not open '%' for writing".format(path));
				};
			}
		} {
			this.assert(false, "Cannot test Document.open behavior in a non-ScIDE session");
		};
	}
}
