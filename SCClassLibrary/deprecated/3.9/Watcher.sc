Watcher : SkipJack {
    *new { arg name = "anon", updateFunc, dt=0.2, stopTest = false;
        this.deprecated(thisMethod, SkipJack.class.findMethod(\new));
        ^super.newCopyArgs(updateFunc, dt, stopTest, name).init.start;
    }
}
