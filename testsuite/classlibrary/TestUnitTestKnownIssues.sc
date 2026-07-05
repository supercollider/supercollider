TestUnitTestKnownIssues : UnitTest {
	*knownIssues {
		^(
			\test_alwaysFails : UnitTestKnownIssue("/some/fake/url", "This test should always fail", failureExpected: true),
			\test_sometimesFails : UnitTestKnownIssue("/some/fake/url", "This test should sometimes fail"),
		)
	}

	test_alwaysFails {
		this.assert(false, "This should always fail");
	}

	test_sometimesFails {
		this.assert(0.5.coin, "This test should sometimes fail"); 
	}
}
