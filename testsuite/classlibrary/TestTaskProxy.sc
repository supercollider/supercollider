TestTaskProxy : UnitTest{

    test_TaskProxy_playOnInstanceClock {
        var actualClock;
        var clock = TempoClock(2);
        var cond = Condition();
        var proxy = TaskProxy{ actualClock = thisThread.clock; cond.unhang };
        proxy.play; // needs to play once before setting clock_, otherwise playOnce sets the player's clock for us
        cond.hang;
        proxy.clock_(clock).play;
        cond.hang;
        this.assertEquals(actualClock, clock, "TaskProxy.play() should use instance's clock if no argClock is given");
    }

	test_TaskProxy_playNewSource_afterInternalError {
		var newSourceCalledAfterError = false, t = TaskProxy(''), c = CondVar();
		t.play(quant:0);
		t.source = { newSourceCalledAfterError = true; c.signalAll; nil };
		c.waitFor(1) { newSourceCalledAfterError };
		this.assert(newSourceCalledAfterError,
			"TaskProxy should resume playing after an error, when a new source is provided.");
	}

	test_Tdef_playNewSource_afterInternalError {
		var newSourceCalledAfterError = false, c = CondVar();
		Tdef(\test, '').play(quant:0);
		Tdef(\test).source = { newSourceCalledAfterError = true; c.signalAll; nil };
		c.waitFor(1) { newSourceCalledAfterError };
		Tdef(\test).clear;
		this.assert(newSourceCalledAfterError,
			"Tdef should resume playing after an error, when a new source is provided.");
	}

}
