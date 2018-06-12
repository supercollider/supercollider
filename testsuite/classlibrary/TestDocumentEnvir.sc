TestDocumentEnvironment : UnitTest {

	var doc, envir1, envir2, envir0;

	setUp {
		envir0 = (a: 0);
		envir1 = (a: 27);
		envir2 = (a: 29);

		currentEnvironment = envir0;
		doc = Document(envir: envir1);
	}

	tearDown {
		doc.close;
		currentEnvironment = topEnvironment;
	}

	test_new_document_in_background {
		this.assert(doc.isFront.not,
			"new document should be created in the background by default");
	}

	test_new_document_in_background_neutral {
		this.assertEquals(currentEnvironment, envir0,
			"background document linked with envir on creation should not change currentEnvironment");
	}

	test_set_envir_in_background {

		doc.envir = envir2;

		this.assertEquals(currentEnvironment, envir0,
			"background document linked with envir interactively should not change currentEnvironment");

	}

	test_document_to_front {

		doc.envir = envir2;

		doc.front;

		this.assertEquals(currentEnvironment, envir2,
			"focused document linked with envir should push its environment");
	}

	test_document_to_front_save_old_envir {

		doc.envir = envir2;

		doc.front;

		this.assertEquals(doc.savedEnvir, envir0,
			"focused document linked with envir should save its old environment");
	}

	test_set_envir_in_foreground {

		doc.envir = envir2;

		doc.front;

		doc.envir = envir1;

		this.assertEquals(currentEnvironment, envir1,
			"focused document linked with envir should push envir when envir is changed");

	}

	test_set_envir_in_foreground_keep_old_envir {

		doc.envir = envir2;

		doc.front;

		doc.envir = envir1;

		this.assertEquals(doc.savedEnvir, envir0,
			"focused document linked with envir should keep its old saved environment when envir is changed");

	}

	test_onClose {
		var test = { this.assertEquals(count, 1, "onClose should be called not more than once") };
		var count = 0;
		var counter = { count = count + 1; test.value; };
		doc.onClose = counter;
		doc.close;

	}

	test_endFront_on_close {

		var test = { this.assertEquals(count, 1, "endFrontAction should be called not more than once") };
		var count = 0;
		var counter = { count = count + 1; test.value; };
		doc.endFrontAction = counter;
		doc.close;
	}

	test_close_restore_old_envir {

		var test = {
			this.assertEquals(currentEnvironment, envir0,
				"closing document linked with envir should restore original currentEnvironment"
			)
		};

		doc.envir = envir2;

		doc.front;

		doc.onClose = test;

		doc.close;


	}




}


TestEnvironmentDocument : UnitTest {
	var doc;

	setUp {
		currentEnvironment = topEnvironment;
		doc = Document();
	}

	tearDown {
		doc.close;
	}

	test_linkDoc {
		var envir = (a: 9);
		doc.front;
		envir.linkDoc(doc);
		this.assertEquals(currentEnvironment, envir, "a linked environment should be current when document has focus");
	}

	test_unlinkDoc {
		var envir = (a: 9);
		var oldCurrent = currentEnvironment;
		doc.front;

		envir.linkDoc(doc);
		envir.unlinkDoc(doc);
		this.assertEquals(currentEnvironment, oldCurrent, "a linked environment restore original envir when unlinked");

	}

}


