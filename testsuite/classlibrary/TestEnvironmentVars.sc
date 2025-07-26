TestEnvironmentVars : UnitTest {
    test_set_and_get {
		this.assertNoException(
			{"TEST".setenv("100")},
			"Should be able to set env vars"
		);
        this.assertEquals(
            "TEST".getenv,
            "100",
            "Should be able to get the value of environment variables."
        );
    }
}
