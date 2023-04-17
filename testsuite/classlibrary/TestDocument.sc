TestDocument : UnitTest {
	// autorun doesn't have access to this object's scope
	// need a public place to communicate the result
	classvar <>success;

	test_save_method_writes_file {
		var doc, file, str, path;
		if(Platform.ideName == "scqt") {
			path = Platform.defaultTempDir +/+ "test_document_save.scd";
			if(File.exists(path)) {
				this.assert(false, "Document save test file path should not exist: '%'".format(path));
			} {
				doc = Document(string: "abcde");
				// note: AFAIK there is no signal coming from IDE to sclang
				// to tell it that the document is ready in the editor.
				// Without a signal, there is no Condition to wait on
				// so we have no choice but to use a timed wait
				0.1.wait;
				doc.save(path);
				0.1.wait;
				file = File(path, "r");
				if(file.isOpen) {
					protect {
						str = file.readAllString;
					} {
						file.close;
						File.delete(path);
						doc.close;
					};
					this.assertEquals(str, "abcde", "Document save method should write the correct string to disk");
				} {
					this.assert(false, "Document autoRun test could not open '%' for writing".format(path));
				};
			}
		} {
			// TODO: decide what to do if the test is run outside of the scide
			"test_save_method_writes_file did not complete as it was run outside the IDE".warn;
		};
	}

	test_new_document_runs_initAction {
		var condvar = CondVar();
		var success = false;
		var doc, save;

		if(Platform.ideName == "scqt") {
			save = Document.initAction;
			protect {
				Document.initAction = {
					success = true;
					condvar.signalOne;
				};
				doc = Document.new;
				condvar.waitFor(0.2);
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
		if (Platform.ideName == "scqt") {
			doc = Document(string: "abc");
			str = doc.getText;
			doc.close;
			this.assertEquals(str, "abc", "getText contents retrieved from document should match input contents");
		} {
			// TODO: skip
		}
	}

	test_document_getTextAsync_retrievesText {
		var doc, str,
		cond = Condition.new;
		if (Platform.ideName == "scqt") {
			doc = Document(string: "abc");
			doc.getTextAsync({ |text|
				str = text;
				cond.unhang;
			}, 0, -1);
			cond.hang;
			doc.close;
			this.assertEquals(str, "abc", "getTextAsync contents retrieved from document should match input contents");
		} {
			// TODO: skip
		}
	}
}
