References {
    var member;

    // expect: no diagnostics
    // expect: no result
    literal { ^`1 }
    // expect-reads: a -> a = 1
    local { var a = 1; ^`a }
    // expect-reads: member -> member
    member { ^`member }
    nested { ^``1 }
    expression { ^`(1 + 2) }
    in_array { ^[`1, `2] }
    // expect: no diagnostics
    curried { ^`_ }
    // expect: no diagnostics
    curried_inside { ^`(_ + 1) }
    // expect-error: Variable not defined
    undefined { ^`undefined }
}
