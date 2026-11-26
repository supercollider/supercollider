// Legacy matches the old compiler's prototype frame, a literal is the prototype, anything else is computed.
LegacyInitialValues {
    // A literal is the prototype, every call starts with it, so passing nil keeps nil.
    literal_defaults { arg a = 1, b = -2, c = "s", d = \x, e = #[1, \y], f = $c, g = nil, h = pi, i = 2pi; ^a }
    // Anything else is computed when the argument is nil, bracketed or not.
    computed_defaults { arg a = 1 + 1, b = (1), c(1), d = [1], e = (c: 3), f = { 1 }, g = Object; ^a }
    pipe_defaults { |a = 1, b(2), c = (3)| ^a }
    block_defaults { ^{ |a = 1, b = (1 + 1)| a } }
    // Variables are the same, without a value it is nil.
    variables { var a = 1, b = 1 + 1, c = (1), d = [1], e = #[1], f, g = "s"; ^a }
    nested_return { |x = 1, y = (2)| var z = 3; if (x) { ^y }; ^z }
}
