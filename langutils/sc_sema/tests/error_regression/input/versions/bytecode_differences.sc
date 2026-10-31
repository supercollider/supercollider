// bytecode
// Checked against old sclang's bytes by sc_backend/tools/differential_test.py, whose allowlist names the methods that
// deliberately differ. Each method starts with `cv;` so the old compiler compiles it, rather than replacing a one line
// method with a shortcut.
BytecodeDifferences {
    classvar cv;

    // Differs: a return of a sequence ending in `nil` is ReturnNil, the old compiler only used it for a plain `^nil`.
    returnSequenceNil { |a| cv; ^(a; nil) }

    // The same as old sclang.
    seriesDo { |a, f| cv; ^(a..10).do(f) }
    superSeriesDo { |a, f| cv; ^super.series(a, 10).do(f) }
    nestedNonLocalReturn { |a| cv; a.do { |x| if (x) { ^1 } }; ^2 }
}
