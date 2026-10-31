// bytecode
// Legacy generators, the bytes match old sclang's. Each method starts with a statement, so isn't a shortcut.
BytecodeGeneratorsLegacy {
    var iv;
    classvar cv;
    iterate { |src| src; ^{: x * 2, x <- src } }
    indexRange { |n| n; ^{: [x, i], x i <- (1..n), x.odd } }
    qualifiers { |n| n; ^{: y, x <- (1, 3..n), var y = x * iv, :: y.postln, :while (y < 10) } }
    immediate { |src| src; ^{; x.postln, x <- src, var z = { x }, z.value > 1 } }
    rangeGenerator { |n| n; ^(: 1..n) }
    nilTest { |src| src; {; x, x <- src, x.isNil }; ^nil }
    tail { |src| src; ^{ {; x, x <- src } } }
    nilTests { |src| src; ^{: x, x <- src, x.notNil, :while x.isNil } }
    ranges { |n| n; ^{: x, x <- (..n), y <- (1..), z <- (:1, 3..) } }
    nested { |src| src; ^{: x, x <- src, y i <- {: z, z <- x } } }
    nonLocalReturn { |src| src; if (src.notNil) { ^{: if (x.notNil) { ^x } { 1 }, x <- src } }; ^2 }
    members { |src| src; ^{; iv = x, x <- src, :: cv = x; iv, var w = x + cv } }
    functions { |src| src; ^{: { x + w }, x i <- src, var w = { i } } }
    outer { |src| var k = 3; src; ^{: x + k, x <- (k..src), var k = k + 1, k.even } }
}
