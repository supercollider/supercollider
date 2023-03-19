TestEventPatternProxy : UnitTest{

    test_playOnInstanceClock {
        var actualClock;
        var clock = TempoClock(2);
        var cond = Condition();
        var proxy = EventPatternProxy(
            Pbind(\dummy, Pfuncn{ actualClock = thisThread.clock; cond.unhang }, \dur, 0)
        );
        proxy.clock_(clock).play;
        cond.hang;
        this.assertEquals(actualClock, clock, "EventPatternProxy.play() should use instance's clock if no argClock is given");
    }

	test_EventPatternProxy_xfade_cleanupNotPremature {

		var stream2CleanupCalled = false, p = EventPatternProxy.new.fadeTime_(2), r;
		p.source = Pbind(\dur, 1.1, \degree, 3, \amp, 0.1);
		r = p.asStream;
		// pulling one event is necessary to prime the old stream
		this.assertEquals(r.next(())[\degree], 3,
			"EventPatternProxy xfade 1st source 1st pull shold be executed transparently.");
		p.source = Pfset({},
			Pbind(\dur, 1.3, \freq, 440, \amp, 0.4),
			{ stream2CleanupCalled = true });
		// Pull enough events to exhaust the xfade.
		// There should NOT be a cleanup of the 2nd stream executed,
		// since it's an infinite stream (of the same event).
		// Three more events should be enough to pull here to
		// end the cross-fade, but "just in case" we pull 6.
		6 do: { r.next(()) };
		this.assert(stream2CleanupCalled.not,
			"EventPatternProxy xfade 2nd source cleanup should be not called prematurely.");
		// todo: might also wanna check that the cleanup was sent downstream (addToCleanup seen)
	}

	// todo: TestPatternProxy.test_transparency_withEvents could be moved here
	// without any changes to that method. test_update_withEvents
	// from (the same) TestPatternProxy would need more refactoring though
	// to separate global JITlib lookups (Pdef, Pdefn) from source updating.
}
