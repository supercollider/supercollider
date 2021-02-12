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
				// failureMessage is deliberately nil
				// we will assert below, we don't need `wait` to call `failed` itself
				this.wait({ success }, failureMessage: nil, maxTime: 0.2);
				doc.close;
			} {
				Document.initAction = save;
			};
			this.assert(success, "Document.new should fire Document.initAction function");
		} {
			// TODO: skip
		};
	}

	test_open_autorun_document_runs_code {
		var path = Platform.defaultTempDir +/+ "autoRunTest.scd",
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
			// TODO: skip
		};
	}

	test_document_getText_retrievesText {
		var doc, str;
		this.assert(Platform.ideName == "scqt", "Document tests are valid only when run in the IDE");
		doc = Document(string: "abc");
		str = doc.getText;
		doc.close;
		this.assertEquals(str, "abc", "getText contents retrieved from document should match input contents");
	}

	test_document_getTextAsync_retrievesText {
		var doc, str,
		cond = Condition.new;
		this.assert(Platform.ideName == "scqt", "Document tests are valid only when run in the IDE");
		doc = Document(string: "abc");
		doc.getTextAsync({ |text|
			str = text;
			cond.unhang;
		}, 0, -1);
		cond.hang;
		doc.close;
		this.assertEquals(str, "abc", "getTextAsync contents retrieved from document should match input contents");
	}
}
