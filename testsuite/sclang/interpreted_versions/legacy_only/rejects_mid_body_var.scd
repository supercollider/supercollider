// Legacy needs every var at the top of the body, so a var after a statement doesn't compile. 3.16 accepts it, see
// TestLanguage316. Run with --interpreted-version legacy.
var got = "1; var a = 2; a".interpret;
if (got == nil) { 0.exit } { "mid body var compiled under legacy, gave %".format(got).postln; 1.exit };
