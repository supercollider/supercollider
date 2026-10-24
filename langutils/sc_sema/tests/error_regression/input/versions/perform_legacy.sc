// lir
// Legacy code sends perform as written, as the old compiler did, 3.16 sends the performed selector, see
// messages/perform.sc.
PerformLegacy {
    postfix { ^1.perform(\foo, 2) }
    selectorOnly { ^1.perform(\foo) }
}
