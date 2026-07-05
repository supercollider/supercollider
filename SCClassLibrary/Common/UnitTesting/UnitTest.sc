UnitTestKnownIssue {
	// required
	var <urlOfIssue;
	var <description; 
	// nil if all platforms
	// if false, failure is optional, if true, passing is considered a unit test failure.
	var <failureExpected;

	// A predicate to decide when the known issue is valid.
	var prWhen;

	*new { |urlOfIssue, description, when({true}), failureExpected(false)|
		urlOfIssue ??  { Error("Must provide urls for all known issues").throw };
		^super.newCopyArgs(
			urlOfIssue: urlOfIssue,
			description: description,
			prWhen: when,
			failureExpected: failureExpected 
		);
	}
	when {|platform| ^prWhen.(platform) }
}

UnitTestFeatures {
	const <needsServerBoot = 1, <needsDocument = 2, <needsMIDI = 3, <needsOpenWindow = 4;

}

UnitTest {
	const <brief = 1, <full = 2;

	classvar <failures, <passes, <durations, <knownIssueResults, <numRealFailures = 0, routine, <>reportPasses = true, <>passVerbosity;
	classvar <allTestClasses;

	var currentMethod, methodFailed = false, debug = "";

	*initClass { passVerbosity = full }

	// Returns an array or single instance of UnitTestFeatures (or nil).
	*features { ^nil }

	*needsServerBoot { ^this.features.asArray.includes(UnitTestFeatures.needsServerBoot) }
	*needsDocument { ^this.features.asArray.includes(UnitTestFeatures.needsDocument)  }
	*needsMIDI { ^this.features.asArray.includes(UnitTestFeatures.needsMIDI)  }
	*needsOpenWindow { ^this.features.asArray.includes(UnitTestFeatures.needsOpenWindow)  }

	// A set of symbols (method names) to UnitTestKnownIssues
	*knownIssues { ^nil }

	*findTestClasses {
		allTestClasses = UnitTest.allSubclasses.collectAs({ |class|
			var classkey = class.asString[4..]; // drop Meta_
			var methtests = class.findTestMethods.collectAs({ | method |
				method.name.asString -> {
					class.prRunWithinSetUpClass {
						class.new.runTestMethod(method);
					}
				}
			}, Dictionary);
			methtests.add(" run all in this class" -> { class.run });
			classkey -> methtests;
		}, Dictionary);
		// err there may be some empty classes hanging around
		allTestClasses = allTestClasses.reject { | class | class.size == 1 };
		allTestClasses.add("...All..." -> Dictionary["Run all" -> { UnitTest.runAll }]);

	}

	// run a single test in the name format "TestPolyPlayerPool:test_prepareChildrenToBundle"
	*runTest { | methodName |
		var class, method, unitTest;
		# class, method = methodName.split($:);
		class = class.asSymbol.asClass;
		method = class.findMethod(method.asSymbol);
		if(method.isNil) {
			Error("Test method not found " + methodName).throw
		};
		class.prRunWithinSetUpClass {
			class.new.runTestMethod(method);
		}
	}

	// called before running tests in a unit test class
	*setUpClass {}

	// called after running tests in a unit test class
	*tearDownClass {}

	// called before each test
	setUp {}

	// called after each test
	tearDown {}

	// run all tests of this UnitTest
	*run { | reset = true, report = true, onComplete |
		if(reset) { this.reset };
		this.prRunAllTestMethods(report, onComplete)
	}

	*runAll { |onComplete| ^this.runList(this.allSubclasses, onComplete) }

	*runFiltered { |filterFunc, onComplete|
		^this.runList(this.allSubclasses.select(filterFunc), onComplete);
	}

	*runList { |list, onComplete({|...a| UnitTestFeatures.printOnto(Post, *a)})|
		if (this !== UnitTest) {
			^this.shouldNotImplement(thisMethod)
		};
		^{
			this.reset;
			// Don't pass the onComplete here as we want to run it after all tests are done.
			// Disable reset, we want all the results.
			list.do { |testClass| testClass.run(false, true) };
			this.report;
			onComplete.(durations, passes, failures, knownIssueResults, numRealFailures === 0);
		}.fork(AppClock)
	}


	prRunTestMethodImpl { |method, report|
		// This is quite awkward, but the idea is that nothing that throws while these methods are executing can cause sclang to exit.
		// But anything that is thrown outside of these methods (setUp, performMethod, tearDown) should quit sclang if asked todo so.
		// Ideally, routines would inherit their exceptionHandler and then test authors can wrap everything they think throws in a try block.
		currentMethod = method;
		try { this.setUp } { |err|
			this.failed(method, err.errorString, report, "Error occured during setUp");
			^nil // don't perform tests if this failed.
		};
		try { this.perform(method.name) } { |err|
			this.failed(method,  err.errorString, report,  "an unexpected exception was thrown.");
		};
		try { this.tearDown } { |err|
			this.failed(method,  err.errorString, report, "Error occured during tearDown");
		}
	}

	// run a single test method of this class
	// this assumes that setUpClass has been called
	// and that tearDownClass is called afterwards
	runTestMethod { | method, report = true |
		var start = Date.localtime;

		this.class.forkIfNeeded {
			var cond = CondVar();
			var done = false;

			{
				this.prRunTestMethodImpl(method, report);
				done = true;
				cond.signalAll;
			}.fork(AppClock); // always fork tests on the app clock

			if(cond.waitFor(18, { done }).not){
				this.failed(method, "Timeout: max of 18 seconds allowed.", report);
			};


			this.prRunTestMethodHandleKnowIssue(method, report);
			if(report) { this.class.report };

			durations ?? { durations = () };
			durations[this.class] ?? { durations[this.class] = () };
			durations[this.class][method.name] = Date.localtime.rawSeconds - start.rawSeconds;
		}
	}

	prRunTestMethodHandleKnowIssue { |method, report|
		var knownIssue = this.class.knownIssues;
		if(knownIssue.notNil and: {knownIssue.isKindOf(UnitTestKnownIssue).not}) {
			// accepts either a UnitTestKnownIssue, or a dictionary of method names to issues
			knownIssue = knownIssue[method.name];
		};
		
		if(knownIssue.notNil and: {knownIssue.when(thisProcess.platform) }){
			failures[this.class].do { |tests|
				tests.do { |r| 
					if(r.testMethod.name == method.name){
						r.ignore = true;
						numRealFailures = numRealFailures - 1;
						knownIssueResults[this.class] = knownIssueResults[this.class].add(r);
					}
				}
			};
			passes[this.class].do { |tests|
				tests.do { |r| 
					if(r.testMethod.name == method.name){
						r.ignore = true;
						knownIssueResults[this.class] = knownIssueResults[this.class].add(r);
					}
				}
			};

			if (knownIssue.failureExpected){
				if (methodFailed.not) {
					this.failed(method, "This test was expected to fail, you might have fixed a bug! " ++ knownIssue.description ++ " url:" ++ knownIssue.urlOfIssue, report);
				}
			} 
		};
	}



	*prRunAllTestMethods { |report = true, onComplete|
		"RUNNING UNIT TEST '%'".format(this.name).inform;
		this.forkIfNeeded {
			this.prRunWithinSetUpClass {
				this.findTestMethods.do { |method|
					this.new.runTestMethod(method, report)
				};
				if(report) { this.report };
				onComplete.(durations, passes, failures);
			}
		}
	}

	// call a function in the context of this test class
	*prRunWithinSetUpClass { |func|
		this.forkIfNeeded {
			this.setUpClass;
			func.value(this);
			this.tearDownClass;
		}
	}

	*gui {
		this.findTestClasses;
		^UnitTestGUI.new(this.allTestClasses)
	}

	///////////////////////////////////////////////////////////////////////
	// call these in your test_ methods to check conditions and pass or fail

	assert { | boolean, message, report = true, onFailure, details |
		if(boolean.not) {
			this.failed(currentMethod, message, report, details);
			if(onFailure.notNil) {
				{ onFailure.value }.defer;
				Error("UnitTest halted with onFailure handler.").throw;
			};
		} {
			this.passed(currentMethod, message, report, details)
		};
		^boolean
	}

	assertEquals { |a, b, message = "", report = true, onFailure |
		var details = "Is:\t % \nShould be:\t %".format(a, b);
		this.assert(a == b, message, report, onFailure, details);
	}

	assertNotEqual { |a, b, message = "", report = true, onFailure |
		var details = "Is:\t % \nShould not be:\t %".format(a, b);
		this.assert(a != b, message, report, onFailure, details);
	}

	assertFloatEquals { |a, b, message = "", within = 0.0001, report = true, onFailure|
		var details = ("Is:\t % \nShould equal (within range %):\t %").format(a, within, b);
		this.assert((a - b).abs <= within, message, report, onFailure, details);
	}

	assertArrayFloatEquals { |a, b, message = "", within = 0.0001, report = true, onFailure|
		var results, startFrom, someHaveFailed;
		a = a.asArray;

		// Check whether all in array meet the condition.
		results = (a - b).abs <= within;
		someHaveFailed = results.includes(false);

		if(someHaveFailed) {
			startFrom = results.indexOf(false);
			// Add failure details:
			message = message ++
			"\n% of % items in array failed to match."
			" Displaying arrays from index of first failure"
			" (%) onwards:\n%\n! = \n%\n"
			.format(
				results.count(_ == false),
				results.size,
				startFrom,
				a[startFrom..],
				if(b.isArray) { b[startFrom..] } { b }
			);
			this.failed(currentMethod, message, report);

			if(onFailure.notNil) {
				{ onFailure.value }.defer;
				Error("UnitTest halted with onFailure handler.").throw;
			};
		} {
			this.passed(currentMethod, message, report)
		}
		^someHaveFailed.not
	}

	assertException { | func, errorClass, message, report = true, onFailure, details |
		var moreDetails = nil;
		var passed = false;
		errorClass = errorClass.asClass;

		func.try { |error|
			if(error.isKindOf(errorClass)) {
				// Add extra info in case the class was an unexpected child type.
				moreDetails = "Received exception of class '%', with message: '%'".format(
					error.class.name,
					error.errorString
				);
				passed = true;
			} {
				moreDetails = "Received exception of class '%', with message: '%'\nExpected class '%'".format(
					error.class.name,
					error.errorString,
					errorClass.name
				);
			}
		};

		moreDetails = moreDetails ?? { "Function did not throw an exception" };
		if(details.isNil) { details = moreDetails } { details = details ++ "\n" ++ moreDetails };
		^this.assert(passed, message, report, onFailure, details);
	}

	assertNoException { | func, message, report = true, onFailure, details |
		var moreDetails;
		var passed = true;

		func.try { |error|
			moreDetails = "Function threw an exception of class '%', with message: '%'".format(
				error.class.name,
				if (error.isException) { error.errorString } { error.asString }
			);
			if(details.isNil) { details = moreDetails } { details = details ++ "\n" ++ moreDetails };
			passed = false;
		};
		^this.assert(passed, message, report, onFailure, details)
	}


	// make a further assertion only if it passed, or only if it failed
	ifAsserts { | boolean, message, ifPassedFunc, ifFailedFunc, report = true|
		if(boolean.not) {
			this.failed(currentMethod, message, report);
			ifFailedFunc.value;
		} {
			this.passed(currentMethod,message, report);
			ifPassedFunc.value;
		};
		^boolean
	}

	// this method should be avoided if at all possible
	// it's better to use CondVar directly in tests instead
	wait { |predicate, failureMessage = "", maxTime = 10.0|
		var condvar = CondVar();
		var waitDur = 0.1;
		var limit = max(1.0, maxTime / waitDur);

		while {
			(limit >= 0) and: { condvar.waitFor(waitDur, predicate).not }
		} {
			limit = limit - 1;
		};

		// consider test failed if limit is surpassed
		if(limit < 0) {
			this.failed(currentMethod, failureMessage)
		}
	}

	// wait is better
	asynchAssert { |waitConditionBlock, testBlock, timeoutMessage = "", timeout = 10|
		var limit = timeout / 0.1;

		while {
			waitConditionBlock.value.not and:
			{ (limit = limit - 1) > 0 }
		} {
			0.1.wait;
		};

		if(limit == 0) {
			this.failed(currentMethod,"Timeout:" + timeoutMessage)
		} {
			testBlock.value
		};
	}

	// if already booted, then freeAll and create new allocators
	// if this is called inside a routine, the routine waits until server is booted

	bootServer { | server |
		server = server ? Server.default;
		if(server.serverRunning.not) {
			server.bootSync
		} {
			server.freeAll;
		};
		server.newAllocators; // new nodes, busses regardless
	}

	debug { |text|
		debug = debug ++ text;
	}

	// call failure directly
	failed { | method, message, report = true, details |
		var r = UnitTestResult(this.class, method, message, details, debug, false);
		failures[this.class] = failures[this.class].add(r);
		numRealFailures = numRealFailures + 1;

		methodFailed = true;

		if(report) {
			Post << Char.nl << "FAIL: ";
			r.report;
			Post << Char.nl;
		};
	}

	// call pass directly
	passed { | method, message, report = true, details |
		var r = UnitTestResult(this.class, method, message, details, debug, true);
		passes[this.class] = passes[this.class].add(r);

		if(report and: { reportPasses }) {
			Post << "PASS: ";
			r.report(passVerbosity == brief);
		};
	}

	// PRIVATE IMPLEMENTATION
	// these are mostly private
	// don't use this directly,
	// use TestClass.run

	*runTestClassForClass { | class, reset = true, report = true |
		var testClass;
		if(class.isNil) {
			"No class supplied for testing".die;
		};
		testClass = ("Test" ++ class.name.asString).asSymbol.asClass;
		if(testClass.isNil) {
			("No test class found for " + class).inform;
			^this
		};
		if(testClass.respondsTo(\run).not) {
			("Attempting to run UnitTests on class that is not a subclass of UnitTest"
				+ testClass).error;
			^this
		};
		testClass.run(reset,report)
	}

	*findTestClass { | forClass |
		^("Test" ++ forClass.name.asString).asSymbol.asClass
	}

	*report {
		Post.nl;
		if (this !== UnitTest) {
			"UNIT TESTS FOR '%' COMPLETED".format(this.name).inform;
			if(failures[this].size > 0) {
				"There were failures:".inform;
				failures[this].do { arg results;
					results.report(true);
				};
			} {
				"There were no failures".inform;
			}
		} {
			failures = failures.reject { |c| c.size == 0 };
			if(failures.size > 0) {
				"There were failures:".inform;
				failures.do { |failingClass|
					failingClass.do { |results| results.report(true)}
				}
			} {
				"There were no failures".inform;
			}
		}
	}

	// private - use TestYourClass.run

	*forkIfNeeded { |function|
		function.forkIfNeeded(AppClock)
	}

	// returns the methods named test_
	findTestMethods {
		^this.class.findTestMethods
	}

	*findTestMethods {
		^methods.select { |m|
			m.name.asString.beginsWith("test_")
		}
	}

	*classesWithTests { | package = 'Common'|
		^Quarks.classesInPackage(package).select { |c|
			UnitTest.findTestClass(c).notNil
		}
	}

	*classesWithoutTests { |package = 'Common'|
		^Quarks.classesInPackage(package).difference(UnitTest.classesWithTests(package))
	}

	// whom I am testing
	// removing "Test" by copyToEnd(4)
	*findTestedClass {
		^this.name.asString.copyToEnd(4).asSymbol.asClass
	}

	// methods in the tested class that do not have test_ methods written
	*untestedMethods {
		var testedClass,testMethods,testedMethods,untestedMethods;
		testedClass = this.findTestedClass;
		// what methods in the target class do not have tests written for them ?
		testMethods = this.findTestMethods;
		testedMethods = testMethods.collect { |meth|
			testedClass.findMethod(meth.name.asString.copyToEnd(5).asSymbol)
		}.reject(_.isNil);

		if(testedMethods.isNil or: { testedMethods.isEmpty }) {
			untestedMethods = testedClass.methods
		} {
			untestedMethods = testedClass.methods.select { |meth|
				testedMethods.includes(meth).not
			}
		};

		// reject getters,setters, empty methods
		untestedMethods = untestedMethods.reject { |meth| meth.code.isNil };
		^untestedMethods
	}

	*listUntestedMethods { | forClass |
		this.findTestClass(forClass).untestedMethods.do {|m| m.name.postln }
	}

	// private
	*reset {
		failures = ();
		passes = ();
		durations = ();
		knownIssueResults = ();
		numRealFailures = 0;
		routine.stop;
	}

	s {
		^Server.default; // for convenient translation to/from example code
	}

}


UnitTestResult {
	var <testClass, <testMethod, <message, <details, <debug, <passed, <>ignore = false;

	*new { |testClass, testMethod, message(""), details, debug, passed|
		^super.newCopyArgs(
			testClass: testClass ? this,
			testMethod: testMethod ? thisMethod,
			message: message,
			details: details,
			debug: debug,
			passed: passed
		)
	}

	report { |brief=false|
		var name = if(testMethod.notNil) { testMethod.name } { "unit test result" };
		Post << testClass.asString << ": " << name;
		if (message.size > 0) {
			Post << " - " << message;
		};
		if (brief.not) {
			if (debug.size > 0) {
				Post << Char.nl << debug;
			};
			if (details.notNil) {
				Post << Char.nl << details;
			};
		};
		Post << Char.nl;
	}
}

UnitTestResultsPrinter {
	const sectionSize = 150;
	classvar useColours;

	*initClass {
		useColours = \Document.asClass.isNil;
	}

	*new { |...a, k| ^this.performArgs(\printOnto, [Post] ++ a, k) }

	*printSimpleResultsOnto { |stream, passes, failures|
		var all = passes.asArray.flat ++ failures.asArray.flat;
		var allFiles = all.collectAs({ |r| 
			r.testClass -> File.readAllString(r.testClass.filenameSymbol.asString)
		}, Event);

		all.do { |result|
			var pos, lineNumber, startOfLine = 0;
			if (result.ignore.not) {
				pos = result.testMethod.charPos;
				lineNumber = allFiles[result.testClass][0..pos].count { |c, i| 
					if (c == $\n) {
						startOfLine = i;
						true
					} { 
						false 
					}
				} + 1;

				stream 
					<< result.passed.if({"PASSED"}, {"FAILED"}) << $: 
					<< result.testClass.asString << $- << result.testMethod.name.asString << $: 
					<< result.testMethod.filenameSymbol.asString << $: 
					<< lineNumber << $:
					<< (pos - startOfLine) << $:
					<< ((pos - startOfLine) + result.testMethod.name.asString.size) << $:
					<< (result.message.select { |c| c != $\n })
					<< " "
					<< if (result.details.notNil) { result.details.select { |c| c != $\n }} { "" } << $\n;
			}
		};
		^stream
	}

	*printOnto { |stream, durations, passes, failures, knownIssueResults, success, options(())|
		var flatDurations, failedMethods, knownIssueByMethod;

		var unitTestsWithMethods = UnitTest.allSubclasses.select { |u| u.findTestMethods.size != 0 };
		var numSkipped = unitTestsWithMethods.count { |c| durations[c].isNil };

		// puts failures at the end
		var positiveSubClassList = unitTestsWithMethods.select{ |c| failures[c].isNil or: {failures[c].every(_.ignore) } and: {durations[c].notNil} };
		var skippedSubClassList = unitTestsWithMethods.select{ |c| durations[c].isNil };
		var negativeSubClassList = unitTestsWithMethods.select{ |c| failures[c].notNil and: {failures[c].any(_.ignore.not)} }; 
		var negativeWithoutKnownIssuesSubClass = negativeSubClassList.select {|c| c.knownIssues.isNil };

		var failedCount = failures.inject(0, {|p, d| p + d.count(_.ignore.not) });
		var failedButIgnoredCount = failures.inject(0, { |p, d| p + d.count(_.ignore)});
		var passedCount = passes.inject(0, {|p, d| p + d.size});


		if (options[\printSimple] ?? {false}) {
			UnitTestResultsPrinter.printSimpleResultsOnto(stream, passes, failures);
		};

		if((options[\supressPassedResults] ?? {false}).not) {
			this.prStartSection(stream, "Passesed Test Results");
			positiveSubClassList.do {|class|
				this.prPrintOnTo_ClassResult(stream, class, durations[class], passes[class], failures[class]);
				stream << "\n";
			}
		};

		if((options[\supressDurationStats] ?? {false}).not) {
			this.prStartSection(stream, "Duration Statistics");
			flatDurations = [];
			durations.keysValuesDo{ |class, methodDurs| 
				methodDurs.keysValuesDo{ |method, dur|
					flatDurations = flatDurations.add((\class: class, \method: method, \duration: dur));
				}
			};

			this.prDurationsGraph(stream, flatDurations);

			flatDurations
				.sort{|l,r| l[\duration] > r[\duration] }
				.keep(10)
				.do { |dict|
					stream <<  "\t" <<
					("%:% ".format(dict[\class], dict[\method]).padRight(100) ++ this.prRoundDuration(dict[\duration]) ++ "s\n")
				};
		};

		if(knownIssueResults.size != 0 and: {(options[\supressKnownIssues] ?? {false}).not}) {
			this.prStartSection(stream, "Classes With Known Issues");
			stream << "\tFor an overview of these issues see: " << this.prMkUnderlined("https://github.com/supercollider/supercollider/issues/5360") << "\n\n";

			knownIssueResults.keysValuesDo { |class, resultsArray|
				var p  = resultsArray.inject(0, { |acc, r| acc + r.passed.if(1, 0)} );
				var total = p + resultsArray.inject(0, { |acc, r| acc + r.passed.if(0, 1)} );
				stream << "\t%\t%\n".format(
					this.prFormatCount( p, total, p == total),
					class.asString
				);
				class.findTestMethods.do { |m|
					var issue = class.knownIssues;
					if (issue.isKindOf(UnitTestKnownIssue).not) {
						issue = issue[m.name];
					};
					knownIssueByMethod = resultsArray.select { |results| results.testMethod.name === m.name };
					if (knownIssueByMethod.size !== 0) {
						stream <<  "\t\t" << "%\t% % url: %\n".format(
							if(knownIssueByMethod.any { |r| r.passed.not }) { 
								this.prFormatCount(knownIssueByMethod.inject(0, {|acc, r| acc + r.passed.if(1, 0)}), knownIssueByMethod.size, false) 
							} { 
								this.prFormatCount(knownIssueByMethod.size, knownIssueByMethod.size, true) 
							}, 
							m.name,
							issue.description,
							this.prMkUnderlined(issue.urlOfIssue)
						);
						knownIssueByMethod.do { |result| this.prPrintOnTo_result(stream, result) }
					}
				}
			};
		};

		if(failedCount != 0 and: {(options[\supressFailedResults] ?? {false}).not}) {
			this.prStartSection(stream, "Failed Test Results");
			negativeSubClassList.do {|class|
				var k;
				if (k = knownIssueResults[class]; k.isKindOf(UnitTestKnownIssue).not) {
					if (k.isNil) {
						// this means we have no known issue and need to print everything.
						this.prPrintOnTo_ClassResult(stream, class, durations[class], passes[class], failures[class]);
					} {
						this.prPrintOnTo_ClassResult(stream, class, durations[class], passes[class], failures[class], k.collectAs({|u| u.testMethod }, Set));
					};
					stream << "\n";
				};
			};
		};

		if((options[\supressSkippedList] ?? {false}).not) {
			this.prStartSection(stream, "Skipped");
			if (options[\skipReason].notNil) {
				stream << "\t" << this.prMkBold(options[\skipReason].asString) << "\n\n";
			};
			unitTestsWithMethods
				.select { |u| durations[u].isNil }
				.do { |c| stream << "\t" << this.prMkYellow("[-]") << "\t" << c.asString << "\n" };
		};

		if(failedCount != 0 and: {(options[\supressFailedSummary] ?? {false}).not}) {
			this.prStartSection(stream, "Failed");

			failures.keysValuesDo { |class, failed|
				failedMethods = Set();
				failed.do {|result|
					if (result.ignore.not) {
						failedMethods.add(result.testMethod)
					}
				};
				failedMethods.do{ |m|
					stream << this.prMkRed("\t[✘]\t%:%\n".format(class.asString, m.name));
				}
			};
		};

		if((options[\supressFinalCount] ?? {false}).not) {
			this.prStartSection(stream, "Final Count");


			stream << "\t" << this.prMkBold(if(success) { "TEST STEP PASSED"} {"TETS STEP FAILED"}) << "\n\n";

			stream << "\t%, %, %, %, out of %\n\n\n\n".format(
				this.prMkRed("% test% failed".format(failedCount, (failedCount > 1).if({"s"},{""}))),
				this.prMkYellow("% failure% ignored".format(failedButIgnoredCount, (failedButIgnoredCount > 1).if({"s"},{""}))),
				this.prMkYellow(numSkipped.asString ++ " test classes skipped"),
				this.prMkGreen(passedCount.asString ++ " passed"),
				passedCount + failedCount + numSkipped
			)
		};

		^stream
	}

	*prDurationsGraph { |stream, durations|
		var values = durations.collect {|d| d[\duration] };
		var max = values.maxItem;
		var min = values.minItem;
		var histoSize = 120;
		var total = values.sum;
		// Taking the sqrt of the durations, this give more time resolution for the slow tests, and less resolution for the long tests.
		var histo = values.max(0).histo(histoSize).collect {|v, i| v * i.linlin(0, histoSize - 1,  min, max) / total };
		var lines = 10;
		var histoScaled = histo * lines;
		var v; // temp,

		var minText = this.prRoundDuration(min).asString ++ "s";
		var maxText = this.prRoundDuration(max).asString ++ "s";

		stream << "\n\t" << this.prMkBold("Weighted Historgram of Durations\n");

		lines.reverseDo { |l|
			stream << "\t▏";
			histoScaled.do {|raw|
				v = raw.round(0.125); // 8 steps
				case 
					{ raw == 0.0} { stream << " "}
					{((v - v.frac) - l).abs < 0.01}{ 
						case 
						{v.frac.abs < 0.01} { stream << "▁" }
						{(v.frac - 0.125).abs < 0.01} { stream << "▁"  }
						{(v.frac - 0.25).abs < 0.01} { stream << "▂" }
						{(v.frac - 0.375).abs < 0.01} { stream << "▃" }
						{(v.frac - 0.5).abs < 0.01} { stream << "▄" }
						{(v.frac - 0.625).abs < 0.01} { stream << "▅" }
						{(v.frac - 0.75).abs < 0.01} { stream << "▆" }
						{(v.frac - 0.875).abs < 0.01} { stream << "▇" }
					} 
					{ v > l} { stream << "█" }
					{ stream << " "}
			};
			stream << "▕\n";
		};
		stream <<  "\t" << this.prMkGreen(minText);
		// adding two due to the vertical lines at the ends of the graph
		(histoSize - maxText.size - minText.size + 2) .do { stream << $- };
		stream << this.prMkRed(maxText) << "\n\n";
		
		stream << "\tMean %s, Minimum %s, Maximum %s, Total %s\n\n".format(
			this.prRoundDuration(values.mean), 
			this.prRoundDuration(min), 
			this.prRoundDuration(max), 
			this.prRoundDuration(values.sum)
		);
	}

	*prStartSection { |stream, txt|
		stream << "\n\n";

		stream << "┌";
		sectionSize.do { stream << "─" };
		stream << "┐\n";

		stream << "│";
		(sectionSize - txt.size / 2).ceil.do { stream << " " };
		stream << this.prMkBold(txt);
		(sectionSize - txt.size / 2).floor.do { stream << " " };
		stream << "│\n";

		stream << "└";
		sectionSize.do { stream << "─" };
		stream << "┘\n";
	}

	// Returns number of methods skipped
	*prPrintOnTo_ClassResult { |stream, class, durs([]), passes([]), fails([]), methodsToSkip(Set())|
		var methodPasses, methodFails;
		var testMethods = class.findTestMethods;

		var passCount = passes.size;
		var failCount = fails.size;

		var methodPassCount, methodFailCount;

		stream << "\t" << ("%\t%".format(
			this.prFormatCount(passCount, passCount + failCount, failCount == 0),
			class.asString,
		).padRight(100) ++ "%\n".format(this.prMkBold(this.prRoundDuration(durs.sum).asString ++ "s")));

		testMethods.do { |method|
			if (methodsToSkip.includes(method.name).not) {
				methodPasses = passes.select { |p| p.testMethod.name === method.name };
				methodFails = fails.select { |p| p.testMethod.name === method.name };

				methodPassCount = methodPasses.inject(0, {|acc, r| acc + r.ignore.if(0, 1)});
				methodFailCount = methodFails.inject(0, {|acc, r| acc + r.ignore.if(0, 1)});

				stream <<  "\t\t" << "%\t%".format(
					this.prFormatCount(methodPassCount, methodPassCount + methodFailCount, methodFailCount == 0),
					method.name
				).padRight(150) << "%s\n".format( this.prRoundDuration(durs[method.name]));

				methodPasses.do{ |result|
					this.prPrintOnTo_result(stream, result)
				};
				methodFails.do {|result|
					this.prPrintOnTo_result(stream, result)
				}
			}
		}
	}


	*prPrintOnTo_result {|stream, result, passed|
		if(result.message.isNil or: {result.message.asString.size == 0}) { 
			if(result.passed) { ^nil };
		};
		stream << "\t\t\t% %\n".format(
			if(result.passed) { this.prMkGreen("[✓]") } {this.prMkRed("[✘]")},
			this.prClipString(result.message.asString.replace("\n", "\n\t\t\t\t"), 400)
		);
		if(result.passed) {
			^nil; // never print details if passed
		};
		if (result.details.notNil) {
			stream << "\t\t\t\t" << result.details.asString.replace("\n", "\n\t\t\t\t");
			if (result.details.last != $\n) {
				stream << "\n";
			}
		}
	}


	*prFormatCount { |top, bottom, c|
		^case 
			{c === true} { this.prMkGreen("[%/%]".format(top, bottom))  }
			{c === false} { this.prMkRed("[%/%]".format(top, bottom)) }
			{ this.prMkYellow("[%/%]".format(top, bottom)) }
	}

	*prClipString {|str, limit|
		^if (str.size > limit) { str.keep(limit - 3) ++ "..." } { str }
	}

	*prRoundDuration { |dur|
		var log = dur.log10.round;
		^dur.round(10.pow(log - 2))
	}

	*prMkRed { |str|
		^if (useColours) { "%[31m%%[0m".format(27.asAscii, str, 27.asAscii) } { str }
	}
	*prMkYellow { |str|
		^if (useColours) { "%[33m%%[0m".format(27.asAscii, str, 27.asAscii) } { str }
	}
	*prMkGreen { |str|
		^if (useColours) { "%[32m%%[0m".format(27.asAscii, str, 27.asAscii) } { str }
	}
	*prMkBold { |str|
		^if (useColours) { "%[1m%%[0m".format(27.asAscii, str, 27.asAscii) } { str }
	}
	*prMkUnderlined { |str|
		^if (useColours) { "%[4m%%[0m".format(27.asAscii, str, 27.asAscii) } { str }
	}
}

// scripts may be located next to the class or one folder below
// they should have a unique name and end with "_unittest.scd"
// the scripts are listed under this test class: UnitTestScript

// UnitTestScript mimics the behavior of Method,
// in order to sneak into the anthill without getting eaten

UnitTestScript : UnitTest {

	var <>name, <>path;

	classvar <allScripts;
	classvar fileEnd = "_unittest.scd";
	classvar scriptDict;

	*new { |name, path|
		^super.new.init(name, path)
	}

	init { |argName, argPath|
		name = argName;
		path = argPath;
	}

	*initClass {
		scriptDict = ();
	}

	*runTest { | scriptName |
		var script;
		allScripts ?? { this.findTestScripts };
		script = allScripts[scriptName.asSymbol];
		if(script.isNil) { ("UnitTestScript: script not found: "+ scriptName ).warn } {
			script.runScript
		}
	}

	*findTestScripts {
		var classPaths;
		var func = { |path|
			var scriptPaths,fileNames, scriptNames;
			scriptPaths = pathMatch(path ++"/*" ++ fileEnd);
			scriptPaths = scriptPaths ++ pathMatch(path ++"/*/*" ++ fileEnd);
			scriptPaths = scriptPaths.as(Set).as(Array); // remove duplicates
			fileNames = scriptPaths.collect(_.basename);
			scriptNames = fileNames.collect { |x| x.replace(fileEnd, "").asSymbol };
			scriptNames.do { |name, i|
				var oldPath = scriptDict.at(name);
				if(oldPath.notNil and: { oldPath != scriptPaths[i] }) {
					Error(
						"duplicate script name:\n%\n%\n\npath:%\n\n"
						.format(scriptPaths[i], scriptDict[name], path)
					);
				};
				scriptDict.put(name, scriptPaths[i]);
				if(oldPath.notNil) { allScripts.add(this.new(name, scriptPaths[i])) };
			};
		};

		classPaths = Class.allClasses.collectAs({ |class| class.filenameSymbol.asString.dirname }, Set);
		allScripts = List.new;
		classPaths.do(func);

	}

	*findTestMethods {
		this.findTestScripts;
		^allScripts
	}

	runTestMethod { |testScript|

		testScript.runScript;

	}

	runScript {
		("RUNNING UNIT TEST SCRIPT" + name ++ " path:" ++ path ++ "\n\n").inform;
		this.class.forkIfNeeded {
			currentMethod = this;
			path.load.value(this);
			this.class.report;
		}
	}

	run {
		allScripts ?? { this.class.findTestScripts };
		Routine {
			allScripts.do { |testScript|
				this.runTestMethod(testScript)
			}
		}.play(AppClock);
	}

}
