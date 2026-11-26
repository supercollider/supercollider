//# VERSION 3.16
// Inside `#[...]` a name is a symbol, but only inside: names after it are names again.
ImmutableArrayScope {
    const c = 1;
    var inside = #[a, [b]];
    var after_is_constant = [#[a], c];
    var after_is_not_constant = [#[a], d];
    var nested_after = [[#[a]], c];
    var event_after = (x: #[a], y: c);

    // expect: no diagnostics
    method_after { ^[#[a], c] }
}
