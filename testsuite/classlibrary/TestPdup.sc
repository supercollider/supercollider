TestPdup : UnitTest {

    test_swappedArguments_doesNotCrash {
        var err = nil;

        {
            Pbind(
                \dur, Pdup(1/8, 8)
            ).asStream.next;
        }.try { |e|
            err = e;
        };

        this.assert(
            err.isNil,
            "Pdup should not crash if the first argument is < 1"
        );
    }

}
