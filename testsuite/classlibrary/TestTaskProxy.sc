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

}
