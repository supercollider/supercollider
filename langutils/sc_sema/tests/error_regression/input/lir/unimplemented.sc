// lir
//# VERSION 3.16
// Packs with an environment variable target aren't lowered yet, so their method isn't emitted.
LirUnimplemented {
    // expect: no diagnostics
    environment_target { |v| var q; ^#~p, q = v }
}
