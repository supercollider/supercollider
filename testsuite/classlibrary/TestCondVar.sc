// Custom test class for checking exceptions thrown in predicates
TestCondVarError : Error {}

// Custom test classes to test for conversions to int and float
TestCondVarBadIntConversion { asInteger { ^nil } }
TestCondVarBadFloatConversion { asFloat { ^nil } }
TestCondVarGoodIntConversion { asInteger { ^0 } }
TestCondVarGoodFloatConversion { asFloat { ^0.0 } }

TestCondVar : UnitTest {
	// The test cases for `wait` cover situations with multiple threads, these are assumed
	// to hold for `waitFor` as well.

	// Tiny yields until a condition is true. Normally, we would use CondVar for this, but since we are testing CondVar
	// itself, we don't want to rely on any of its functionality.
	momentarilyYieldUntil { |predicate| while { predicate.value.not } { 0.yield } }

	smallTimeSlice { ^1e-6 }

	// wait - without predicate
	test_wait_withoutPredicate_oneThread_signalOne {
		var c = CondVar();
		var waitResult, signalOneResult;
		fork {
			signalOneResult = c.signalOne;
		};

		// will timeout if signalOne does not wake us
		waitResult = c.wait;
		this.assertEquals(waitResult, c, "wait should return CondVar");
		this.assertEquals(signalOneResult, c, "signalOne should return CondVar");
	}

	test_wait_withoutPredicate_twoThreads_signalOne {
		var c = CondVar();
		var secondFired = false;

		fork {
			c.wait;
			secondFired = true;
		};

		fork { c.signalOne };
		c.wait;
		this.assert(secondFired.not, "Before second signalOne");
		c.signalOne; // schedule remaining waiting thread
		this.momentarilyYieldUntil { secondFired };
	}

	test_wait_withoutPredicate_twoThreads_signalAll {
		var c = CondVar();
		var secondFired = false;

		fork {
			c.wait;
			secondFired = true;
		};

		fork { c.signalAll };

		c.wait;
		this.assert(secondFired);
	}

	// wait - with predicate
	test_wait_withPredicate_immediateReturnOnTrue {
		var c = CondVar();
		// both of these should be supported
		// test will timeout in failing case
		this.assertEquals(c.wait(true), c);
		this.assertEquals(c.wait { true }, c);
	}

	test_wait_withPredicate_checkEvaluatedOnThisThread {
		var c = CondVar();
		var counter = 0;
		var evalThreads = [];

		fork {
			c.signalOne;
		};

		c.wait {
			evalThreads = evalThreads.add(thisThread);
			evalThreads.size == 2
		};

		this.assertEquals(evalThreads.size, 2);
		this.assertEquals(evalThreads[0], thisThread);
		this.assertEquals(evalThreads[1], thisThread);
	}

	test_wait_withPredicate_throwInPredicate {
		var c = CondVar();
		var f = { c.wait { TestCondVarError().throw } };
		this.assertException(f, TestCondVarError);
	}

	test_wait_withPredicate_threeThreads_signalAll_allStayBlockedWithOrderingPreserved {
		// Check that with predicates, the ordering on blocked threads is preserved after a call to signalAll.
		// In this test we intentionally create functions that will cause threads to block infinitely, so we have to do
		// some tricky manual cleanup.
		var c = CondVar();
		var wakeTimes = 0!3;
		var allowCleanupFlag = false;
		var orderingCheck = { |first, second, firstName, secondName, note|
			this.assert(first < second, "%: % < % (% < %)".format(note, firstName, secondName, first, second))
		};

		var orderingChecks = { |pre, post, note|
			orderingCheck.(pre, wakeTimes[0], "pre", "wakeTimes[0]", note);
			orderingCheck.(wakeTimes[0], wakeTimes[1], "wakeTimes[0]", "wakeTimes[1]", note);
			orderingCheck.(wakeTimes[1], wakeTimes[2], "wakeTimes[1]", "wakeTimes[2]", note);
			orderingCheck.(wakeTimes[2], post, "wakeTimes[2]", "post", note);
		};

		3.do { |i|
			fork {
				c.wait { wakeTimes[i] = Process.monotonicClockTime; allowCleanupFlag }
			}
		};

		// First we force the fork threads to be blocked and check that the wake times are
		// sensible as a precondition.
		block {
			var preWaitTime = Process.monotonicClockTime;
			this.momentarilyYieldUntil { wakeTimes[2] != 0 };
			orderingChecks.(preWaitTime, Process.monotonicClockTime, "initial scheduling");
		};

		// Now we signalAll and see if the wait times have kept the same ordering.
		block {
			var preWaitTime = Process.monotonicClockTime;
			wakeTimes = 0!3;
			c.signalAll;
			this.momentarilyYieldUntil { wakeTimes[2] != 0 };
			orderingChecks.(preWaitTime, Process.monotonicClockTime, "after signalAll");
		};

		// cleanup
		allowCleanupFlag = true;
		c.signalAll;
	}

	test_wait_withPredicate_oneThread_signalOne {
		// Test a thread blocking, continuing to block, and finally becoming unblocked
		var c = CondVar();
		var predFlag = false; // predicate flag
		var doneFlag = false;
		var counter = 0;

		fork {
			c.wait {
				counter = counter + 1;
				predFlag
			};
			doneFlag = true;
		};

		this.momentarilyYieldUntil { counter == 1 };
		this.assert(doneFlag.not, "doneFlag before signalOne");

		c.signalOne;
		this.momentarilyYieldUntil { counter == 2 };
		this.assert(doneFlag.not, "doneFlag before predFlag set");

		predFlag = true;
		c.signalOne;
		this.momentarilyYieldUntil { counter == 3 };
		this.assert(doneFlag, "doneFlag after predFlag set");
	}

	test_wait_withPredicate_twoThreads_signalOne {
		// Test two threads blocking, the first staying blocked, second unblocking, and first finally unblocking
		var c = CondVar();
		var predFlags = false!2;
		var doneFlags = false!2;
		var counters = 0!2;

		2.do { |i|
			fork {
				c.wait {
					counters[i] = counters[i] + 1;
					predFlags[i]
				};
				doneFlags[i] = true
			}
		};

		this.momentarilyYieldUntil { counters == [1, 1] };
		this.assertEquals(doneFlags, [false, false], "doneFlags before signalOne");

		c.signalOne;
		this.momentarilyYieldUntil { counters == [2, 1] };
		this.assertEquals(doneFlags, [false, false], "doneFlags before predFlags[1] set");

		predFlags[1] = true;
		c.signalOne;
		this.momentarilyYieldUntil { counters == [2, 2] };
		this.assertEquals(doneFlags, [false, true], "doneFlags after predFlags[1] set");

		predFlags[0] = true;
		c.signalOne;
		this.momentarilyYieldUntil { counters == [3, 2] };
		this.assertEquals(doneFlags, [true, true], "doneFlags after predFlags[0] set");
	}

	test_wait_withPredicate_twoThreads_signalAll {
		var c = CondVar();
		var counters = 0!2;
		var doneFlags = false!2;
		var flags = false!2;

		2.do { |i|
			fork {
				c.wait {
					counters[i] = counters[i] + 1;
					flags[i]
				};
				doneFlags[i] = true;
			}
		};

		this.momentarilyYieldUntil { counters == [1, 1] };
		this.assertEquals(doneFlags, [false, false], "Done flags before first signalAll");

		c.signalAll;
		this.momentarilyYieldUntil { counters == [2, 2] };
		this.assertEquals(doneFlags, [false, false], "Done flags before flags[0] set");

		flags[0] = true;
		c.signalAll;
		this.momentarilyYieldUntil { counters == [3, 3] };
		this.assertEquals(doneFlags, [true, false], "Done flags after flags[0] set");

		flags[1] = true;
		c.signalAll;
		this.momentarilyYieldUntil { counters == [3, 4] }; // thread 1 already exited
		this.assertEquals(doneFlags, [true, true], "Done flags after flags[1] set");
	}

	// waitFor - preconditions for timeoutBeats
	test_waitFor_timeoutBeats_badValuesThrowErrors {
		var c = CondVar();

		this.assertException({ c.waitFor(nil) }, Error, "nil");
		this.assertException({ c.waitFor(TestCondVarBadIntConversion()) }, Error, "Bad int conversion");
		this.assertException({ c.waitFor(TestCondVarBadFloatConversion()) }, Error, "Bad float conversion");
		this.assertException({ c.waitFor(inf) }, Error, "inf");
		this.assertException({ c.waitFor(0/0) }, Error, "NaN");
	}

	test_waitFor_timeoutBeats_immediateTimeout {
		var c = CondVar();
		// Makes use of the fact that the condition must be evaluated at least twice unless we timeout immediately
		var checkValue = { |val|
			var counter = 0;
			var result = c.waitFor(val) { counter = counter + 1; false };
			this.assertEquals(counter, 1, "waitFor(%): predicate only evaluated once".format(val));
			this.assert(result.not, "waitFor(%): result".format(val));
		};

		checkValue.(0);
		checkValue.(0.0);
		checkValue.(-1);
		checkValue.(-inf);

		// Check good conversions too, these convert to 0 and 0.0 respectively
		checkValue.(TestCondVarGoodIntConversion());
		checkValue.(TestCondVarGoodFloatConversion());
	}

	// waitFor - without predicate
	test_waitFor_withoutPredicate_signalled {
		var c = CondVar();
		fork { c.signalOne };
		// use assertEquals in case waitFor returns a non-boolean
		this.assertEquals(c.waitFor(1), true);
	}

	test_waitFor_withoutPredicate_timedOut {
		var c = CondVar();
		// use assertEquals in case waitFor returns a non-boolean
		this.assertEquals(c.waitFor(this.smallTimeSlice), false);
	}

	// waitFor - with predicate
	test_waitFor_withPredicate_immediatelyTrue {
		var c = CondVar();
		this.assertEquals(c.waitFor(this.smallTimeSlice) { true }, true);
	}

	test_waitFor_withPredicate_signaledButFalseSoTimedOut {
		var c = CondVar();
		var counter = 0;
		var waitResult;
		fork { c.signalOne };

		// This wait has to be a bit longer because we need to ensure there's enough time for the spurious wakeup before
		// timing out.
		waitResult = c.waitFor(0.1) {
			counter = counter + 1;
			false
		};
		this.assertEquals(waitResult, false, "waitResult");
		// Evaluated 3 times -- when blocked, when signalled, when timed out
		this.assertEquals(counter, 3, "counter");
	}

	test_waitFor_withPredicate_signaledAndTrue {
		var c = CondVar();
		var counter = 0;
		var waitResult;
		fork { c.signalOne };

		waitResult = c.waitFor(this.smallTimeSlice) {
			counter = counter + 1;
			counter == 2
		};
		this.assertEquals(waitResult, true, "waitResult");
		this.assertEquals(counter, 2, "counter");
	}

	test_waitFor_withPredicate_timedOutFalse {
		var c = CondVar();
		this.assertEquals(c.waitFor(this.smallTimeSlice) { false }, false);
	}

	test_waitFor_withPredicate_timedOutTrue {
		var c = CondVar();
		var counter = 0;
		var waitResult;

		waitResult = c.waitFor(this.smallTimeSlice) {
			counter = counter + 1;
			counter == 2
		};

		this.assertEquals(waitResult, true);
		this.assertEquals(counter, 2);
	}

	test_waitFor_withPredicate_throwInPredicate {
		var c = CondVar();
		var f = { c.waitFor(0) { TestCondVarError().throw } };
		this.assertException(f, TestCondVarError);
	}

	test_waitFor_withPredicate_checkEvaluatedOnThisThread {
		var c = CondVar();
		var counter = 0;
		var evalThreads = [];

		fork {
			c.signalOne;
		};

		c.waitFor(1) {
			evalThreads = evalThreads.add(thisThread);
			evalThreads.size == 2
		};

		this.assertEquals(evalThreads.size, 2, "evalThreads.size");
		this.assertEquals(evalThreads[0], thisThread, "first eval");
		this.assertEquals(evalThreads[1], thisThread, "second eval");
	}

	// signalOne
	test_signalOne_noWaits_doesNothingAndReturnsCondVar {
		var c = CondVar();
		this.assertEquals(c, c.signalOne);
	}

	// signalAll
	test_signalAll_noWaits_doesNothingAndReturnsCondVar {
		var c = CondVar();
		this.assertEquals(c, c.signalAll);
	}

	// copies disallowed
	test_noCopiesAllowed {
		this.assertException({ CondVar().copy }, ShouldNotImplementError, "copy throws");
		this.assertException({ CondVar().deepCopy }, ShouldNotImplementError, "deepCopy throws");
		this.assertException({ CondVar().shallowCopy }, ShouldNotImplementError, "shallowCopy throws");
	}
}
