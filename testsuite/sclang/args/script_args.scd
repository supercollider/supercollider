// run with: sclang script_args.scd --foo=1 -b bar
if(thisProcess.argv != ["--foo=1", "-b", "bar"]) {
	"unexpected argv: %".format(thisProcess.argv).postln;
	1.exit;
};

0.exit;
