//# VERSION 3.16
TestPdup : UnitTest {

    test_swappedArguments_doesNotCrash {
        var func = {
            Pbind(
                \dur, Pdup(1/8, 8)
            ).asStream.next;
        };

        this.assertNoException(
            func,
            "Pdup should not crash if the first argument is < 1"
        );
    }

}
