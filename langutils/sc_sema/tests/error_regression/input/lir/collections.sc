// lir
//# VERSION 3.16
// Collections that aren't constant are built as the old compiler did: `[a, b]` and `Class[a, b]` are
// `Class.new(2).add(a).add(b)`, `(k: v)` is `Event.new(1, nil, nil, true).put(\k, v)`.
LirCollections {
    // expect: no diagnostics
    array { |a| ^[a, 2] }
    // expect: no diagnostics
    set { |a| ^Set[a] }
    // expect: no diagnostics
    event { |v| ^(a: v, b: 2) }
    // A pack keeps its value, each target is stored at or copyToEnd of its index.
    // expect: no diagnostics
    pack { |v| var #p, q ...r = v; ^p }
    // expect: no diagnostics
    pack_assign { |v| var p, q; ^#p, q = v }
}
