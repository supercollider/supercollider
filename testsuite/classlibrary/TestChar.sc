TestChar : UnitTest {

    test_asCompileString_roundTrips {
        [$a, $ , $$, $', $", $\\, $\n, $\r, $\t, $\f, $\v, 0.asAscii].do { |c|
            this.assertEquals(c.asCompileString.interpret, c, "% should interpret back to itself".format(c.asCompileString));
        };
    }

    test_asCompileString_backslash {
        this.assertEquals($\\.asCompileString, "$\\\\", "A backslash should be escaped");
    }
}
