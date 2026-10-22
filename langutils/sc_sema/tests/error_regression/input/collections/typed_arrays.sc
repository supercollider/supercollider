// Typed arrays store their elements unboxed. When all the elements are constant, `FloatArray[1, 2]` is a constant,
// with the elements converted as sclang's add does, and when add would fail it isn't folded.
DoubleArray {}
FloatArray {}
Int32Array {}
Int8Array {}
SymbolArray {}
// Overrides add, so it can't be folded.
Int16Array {
    add { ^nil }
}

TypedArrays {
    // expect: -> DoubleArray[1.0, 2.5]
    double_array { ^DoubleArray[1, 2.5] }
    // expect: -> FloatArray[1.0, 0.1]
    float_array { ^FloatArray[1, 0.1] }
    // expect: -> Int32Array[1, -2]
    int32_array { ^Int32Array[1, -2] }
    // expect: -> Int8Array[1, 127, -128, -56]
    int8_array_wraps { ^Int8Array[1, 127, 128, 200] }
    // Floats are truncated towards zero.
    // expect: -> Int32Array[1, -1]
    int_array_truncates_floats { ^Int32Array[1.9, -1.9] }
    // expect: -> SymbolArray[\a, 'b c']
    symbol_array { ^SymbolArray[\a, 'b c'] }
    // expect: -> FloatArray[]
    empty { ^FloatArray[] }
    // expect: -> Int32Array[3]
    folded_elements { var a = 1; ^Int32Array[a + 2] }

    // add would fail.
    // expect: no result
    // expect: no diagnostics
    symbol_in_float_array { ^FloatArray[\a] }
    // expect: no result
    integer_in_symbol_array { ^SymbolArray[1] }
    // expect: no result
    float_too_large { ^Int32Array[1e10] }
    // expect: no result
    not_constant { |a| ^FloatArray[a] }
    // expect: no result
    overridden_add { ^Int16Array[1] }

    // expect: no diagnostics
    non_local_return { |x| ^{ if (x) { ^Int8Array[1] }; SymbolArray[\a] } }
}
