// bytecode
//# VERSION 3.16
// Generators, see lir/generators. A range source is iterated with forSeries even when its value is folded, and a
// `var` qualifier's function is pushed before its value.
BytecodeGenerators {
    folded { ^{: x, x <- (1..4) } }
    unbounded { |n| ^{: x, x <- (n..), y <- (1..) } }
    declare { |a| ^{: f, x <- a, var f = { x } } }
    nested { |a| ^{: y, x <- a, y i <- {: z, z <- x } } }
    whileNil { |a| ^{; x, x <- a, :while x.isNil } }
    tail { |a| a; {; x, x <- a } }
    nonLocalReturn { |a| if (a.notNil) { ^{: if (x) { ^1 } { 2 }, x <- a } }; ^2 }
}
