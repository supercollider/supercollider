//# VERSION 3.16
// A constant is equal to another by its bits, so `0.0` and `-0.0` stay apart, as does an array holding them.
SignedZero {
    // expect: -> [0.0, -0.0]
    both { var a = 0.0, b = -0.0; ^[a, b] }
    // expect: -> [-0.0, 0.0]
    reversed { var a = -0.0, b = 0.0; ^[a, b] }
    // expect: -> -0.0
    negative { ^-0.0 }
    // expect: -> [-0.0]
    negative_array { ^[-0.0] }
    // expect: -> [0.0]
    positive_array { ^[0.0] }
    // expect: -> DoubleArray[0.0, -0.0]
    typed_double { ^DoubleArray[0.0, -0.0] }
    // expect: -> FloatArray[-0.0]
    typed_float { ^FloatArray[-0.0] }
    // expect: -> FloatArray[0.0]
    typed_float_positive { ^FloatArray[0.0] }
}
