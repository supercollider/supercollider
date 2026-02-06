TestPdup : UnitTest {

    test_swappedArguments_doesNotCrash {
        var err;

        err = {
            Pbind(
                \dur, Pdup(1/8, 8)
            ).asStream.next;
        }.try { |e| e };

        // If it crashed, err will be an Error
        this.assert(err.isKindOf(Error).not, "Pdup should not crash if the first argument is < 1");
    }

}
