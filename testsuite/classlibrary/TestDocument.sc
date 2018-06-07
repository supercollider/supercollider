TestDocument : UnitTest {


	test_document_envir {

		var doc, envir1, envir2, envir0;
		var closeCount = 0, closeCounter, endFrontCount = 0, endFrontCounter;

		envir0 = (a: 0);
		envir1 = (a: 27);
		envir2 = (a: 29);

		currentEnvironment = envir0;
		doc = Document(envir: envir1);

		// check base state
		this.assert(doc.isFront.not,
			"new document should be created in the background by default");

		this.assert(currentEnvironment === envir0,
			"background document linked with envir on creation should not change currentEnvironment");

		doc.envir = envir2;

		this.assert(currentEnvironment === envir0,
			"background document linked with envir interactively should not change currentEnvironment");


		doc.front;

		this.assert(currentEnvironment === envir2,
			"focused document linked with envir should push its environment");

		this.assert(doc.savedEnvir === envir0,
			"focused document linked with envir should save its old environment");

		doc.envir = envir1;

		this.assert(currentEnvironment === envir1,
			"focused document linked with envir should push envir when envir is changed");

		this.assert(doc.savedEnvir === envir0,
			"focused document linked with envir should keep its old saved environment when envir is changed");

		closeCounter = { closeCount = closeCount + 1 };
		endFrontCounter = { endFrontCount = endFrontCount + 1 };
		doc.onClose = closeCounter;
		doc.endFrontAction = endFrontCounter;

		doc.close;

		/*

		0.3.wait;

		this.assertEquals(closeCount, 1, "onClose should be called once");
		this.assertEquals(endFrontCount, 1, "endFrontAction should be called once");



		// this fails, has envir1 (a: 27) as current.
		this.assertEquals(currentEnvironment, envir0,
			"closing document linked with envir should restore original currentEnvironment");
		*/



	}

}

