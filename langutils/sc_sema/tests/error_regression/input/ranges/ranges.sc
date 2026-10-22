Ranges {
    ascending { ^(1..5) }
    descending { ^(5..1) }
    step { ^(1, 3..9) }
    step_uneven { ^(1, 3..8) }
    from_zero { ^(..3) }
    single { ^(2..2) }
    step_away_from_end { ^(1, 2..0) }
    zero_step { ^(1, 1..5) }
    floats { ^(1.0..3) }
    not_constant { ^(1..2.foo) }
    too_large { ^(1..2000) }
    unbounded { ^(1..) }
    unbounded_step { ^(1, 3..) }
    in_array { ^[(1..3), 4] }
    receiver { ^(1..3).do { |x| x } }
}
