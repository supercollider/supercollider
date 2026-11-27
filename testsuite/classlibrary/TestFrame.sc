TestFrame : UnitTest {
    foo { ^thisFunction }
    test_does_not_crash {
        // This returns a bunch of function which try to return from them selves (really confusing)
        var fs = 1000.collect { this.foo };
        1000.do { Array.newClear(1000) };
        // f.value attempts to find TestFrame:foo on the stack and return from it.
        fs.do { |f| this.assertException({ f.value }, OutOfContextReturnError) };
    }
}
