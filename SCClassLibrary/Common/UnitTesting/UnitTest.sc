UnitTest {

	var currentMethod, debug = "";
	const <brief = 1, <full = 2;
	classvar <failures, <passes, routine, <>reportPasses = true, <>passVerbosity;
	classvar <allTestClasses;

	*initClass {
		passVerbosity = full;
	}

	*findTestClasses {
		allTestClasses = UnitTest.allSubclasses.collectAs({ |c|
			var classkey = c.asString[4..]; // drop Meta_
			var methtests = c.findTestMethods.collectAs({|m|
				m.name.asString -> { c.new.runTestMethod(m) }
			}, Dictionary);
			methtests.add(" run all in this class" -> { c.run });
			classkey -> methtests;
		}, Dictionary);
		// err there may be some empty classes hanging around
		allTestClasses = allTestClasses.reject {|d| d.size == 1 };
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
		class.new.runTestMethod(method);
	}


	// called before each test
	setUp {}

	// called after each test
	tearDown {}

	*run { | reset = true, report = true |
		if(reset) { this.reset };
		this.forkIfNeeded {
			this.prRunAllTestMethods(report);
		};
	}

	// run all UnitTest subclasses
	*runAll {
		if(this === UnitTest, {
			^this.forkIfNeeded {
				this.reset;
				this.allSubclasses.do { |testClass|
					testClass.run(false, false);
					0.1.wait;
				};
				this.report
			}
		}, {
			^this.shouldNotImplement(thisMethod)
		});
	}

	// run a single test method of this class
	runTestMethod { | method, report = true |
		this.class.forkIfNeeded {
			this.setUp;
			currentMethod = method;
			this.perform(method.name);
			this.tearDown;
			if(report) { this.class.report };
		}
	}


	*prRunAllTestMethods { |report = true|
		"RUNNING UNIT TEST '%'".format(this.name).inform;
		this.forkIfNeeded {
			this.findTestMethods.do { |method|
				this.new.runTestMethod(method, false)
			};
			if(report) { this.report };
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
		var details = "Is:\n\t % \nShould be:\n\t %".format(a, b);
		this.assert(a == b, message, report, onFailure, details);
	}

	assertFloatEquals { |a, b, message = "", within = 0.0001, report = true, onFailure|
		var details = ("Is:\n\t % \nShould equal (within range " ++ within ++ "):\n\t %").format(a, b);
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
				error.errorString
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

	// waits for condition with a maxTime limit
	// if time expires, the test is a failure
	wait { |condition, failureMessage, maxTime = 10.0|
		var limit = maxTime / 0.05;

		while {
			condition.value.not and:
			{(limit = limit - 1) > 0}
		} {
			0.05.wait;
		};

		if(limit == 0 and: failureMessage.notNil) {
			this.failed(currentMethod,failureMessage)
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
		var r = UnitTestResult(this, method, message, details, debug);
		failures = failures.add(r);

		if(report) {
			Post << Char.nl << "FAIL: ";
			r.report;
			Post << Char.nl;
		};
	}

	// call pass directly
	passed { | method, message, report = true, details |
		var r = UnitTestResult(this, method, message, details, debug);
		passes = passes.add(r);

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
		"UNIT TESTS FOR '%' COMPLETED".format(this.name).inform;
		if(failures.size > 0) {
			"There were failures:".inform;
			failures.do { arg results;
				results.report(true);
			};
		} {
			"There were no failures".inform;
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
		failures = [];
		passes = [];
		routine.stop;
	}

	s {
		^Server.default; // for convenient translation to/from example code
	}

}


UnitTestResult {

	var <testClass, <testMethod, <message, <details, <debug;

	*new { |testClass, testMethod, message = "", details, debug|
		^super.newCopyArgs(testClass ? this, testMethod ? thisMethod, message, details, debug)
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
