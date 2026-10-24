// lir
//# VERSION 3.16
// Generators, and packs with an environment variable target, aren't lowered yet, so their method isn't emitted.
LirUnimplemented {
    // expect: no diagnostics
    generator { |a| ^{: x, x <- a } }
    // expect: no diagnostics
    environment_target { |v| var q; ^#~p, q = v }
}
