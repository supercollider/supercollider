/*
 * runAllUnitTests.scd
 *
 * A script to run all unit tests
 *
 * Brian Heim, 2017-09-02
 *
 * If the test suite runs without errors, this script exits
 * the interpreter with the number of reported failures.
 * If an error is caught while running unit tests, the script
 * exits with a result of 1 (indicating failure).
 */

r {
	try {
		UnitTest.runAll;
		UnitTest.failures.size.exit;
	} { |e|
		e.reportError;
		1.exit;
	}
}.play(AppClock)
