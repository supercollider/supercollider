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

}
