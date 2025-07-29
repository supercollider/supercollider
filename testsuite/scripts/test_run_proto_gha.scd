[
	(
		'suite': "*",
		'test': "*",
	),
	(
		'suite': "TestCondition",
		'test': "*",
		'skip': "true",
		'skipReason': "UnitTest fails when run in GHA (FIXME)",
	),
	(
		'suite': "TestCondVar",
		'test': "test_waitFor_withPredicate_signaledButFalseSoTimedOut",
		'skip': "true",
		'skipReason': "UnitTest fails when run in GHA (FIXME)",
	),
	(
		'suite': "TestDocument",
		'test': "*",
		'skip': "true",
		'skipReason': "Document tests are only valid inside the IDE",
	),
	(
		'suite': "TestDocumentEnvironment",
		'test': "*",
		'skip': "true",
		'skipReason': "UnitTest creates issues when run in GHA (FIXME)",
	),
	(
		'suite': "TestEnvironmentDocument",
		'test': "*",
		'skip': "true",
		'skipReason': "UnitTest fails when run in GHA (FIXME)",
	),
	(
		'suite': "TestLinkClock",
		'test': "*",
		'skip': "true",
		'skipReason': "UnitTest has unknown issues when run in GHA (FIXME)",
	),
	(
		'suite': "TestNodeProxyRoles",
		'test': "test_proxyroles_xfade_smoothly",
		'skipReason': "UnitTest fails in GHA (FIXME)",
		'skip': "true",
	),
	(
		'suite': "TestOSCFunc",
		'test': "test_argSizeMatching_Succeeds",
		'skip': "true",
		'skipReason': "UnitTest fails when run in GHA on Linux (FIXME)",
	),
	(
		'suite': "TestThreadReschedule",
		'test': "test_Threads_rescheduleToOtherClock",
		'skipReason': "UnitTest sometime fails when run in GHA on Linux (FIXME)",
		'skip': "true",
	),
	(
		'suite': "TestVolume",
		'test': "test_booting",
		'skip': "true",
		'skipReason': "Multiple Volume synths are created when running test inside the suite",
	),
	(
		'suite': "TestWebView",
		'test': "test_findText_found",
		'skip': "true",
		'skipReason': "UnitTest times out",
	),
	(
		'suite': "TestWebView",
		'test': "test_findText_notFound",
		'skip': "true",
		'skipReason': "UnitTest sometimes fails when run in GHA",
	),
	(
		'suite': "TestMIDI",
		'test': "test_linuxRealtimeCodesAreCorrect",
		'skip': "true",
		'skipReason': "Requires ALSA kernel module, not allowed in GHA",
	),
]
