TestProcessCreation : UnitTest {

	test_string_unixCmdGetStdOut {
		this.assert("echo hello world".unixCmdGetStdOut == "hello world\n",
			"calling the program echo with a certain string should return the same string."
		)
	}


	test_collection_unixCmdGetStdOut {
		this.assert(["echo", "hello", "world"].unixCmdGetStdOut == "hello world\n",
			"calling the program echo with a certain string should return the same string."
		)
	}

}
