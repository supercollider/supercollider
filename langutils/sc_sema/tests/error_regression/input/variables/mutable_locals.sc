//# VERSION 3.16
MutableLocals {
    array { var a = [1, 2]; ^a }
    array_put { var a = [1, 2]; a[0] = 10; ^a }
    array_aliased { var a = [1, 2], b = a; b[0] = 10; ^a }
    array_argument { var a = [1, 2]; { |x| x[0] = 10 }.value(a); ^a }
    array_in_collection { var a = [1, 2]; ^[a] }
    event { var a = (b: 1); ^a }
    string { var a = "abc"; ^a }
    integer_receiver { var a = 1; ^a + 1 }
    float_receiver { var a = 1.5; ^a.floor }
    char_receiver { var a = $a; ^a.isAlpha }
    symbol_receiver { var a = \abc; ^a.asString }
    class { var a = Array; ^a.new }
    boolean { var a = true; ^a.not }
}
