SpecialVariables {
    var member;

    // expect: no diagnostics
    this_alone { ^this }
    // expect: no diagnostics
    this_receiver { ^this.foo }
    // expect: no diagnostics
    super_receiver { ^super.foo(1) }
    // expect: no diagnostics
    super_binary { ^super + 1 }
    // expect: no diagnostics
    super_setter { super.member = 1 }
    // expect: no diagnostics
    super_functional { ^foo(super, 1) }
    // expect: no diagnostics
    super_as_value { ^[super] }
    // expect: no diagnostics
    process { ^thisProcess }
    // expect: no diagnostics
    thread { ^thisThread }
    // expect: no diagnostics
    method { ^thisMethod }
    // expect: no diagnostics
    function_def { ^thisFunctionDef }
    // expect: no diagnostics
    function { ^{ thisFunction } }
    // expect: no diagnostics
    *class_this { ^this }
    // expect: no diagnostics
    in_generator { ^{: this, x <- [1] } }
    // expect: no diagnostics
    curried { ^this.foo(_) }
    // expect: no result
    not_constant { ^this }

    // expect-error: Special variable assigned
    // expect: poisoned
    assign_this { this = 1 }
    // expect-error: Special variable assigned
    assign_process { thisProcess = 2 }
    // expect-error: Special variable assigned
    pack_assign { #this, member = [1, 2] }
    // expect-error: Special variable declared
    declare_variable { var this = 1; ^this }
    // expect-error: Special variable declared
    declare_argument { |thisProcess| ^1 }
    // expect-error: Special variable declared
    declare_block_argument { ^{ |super| 1 } }
    // expect-error: Special variable declared
    declare_pack { var #a, thisThread = [1, 2]; ^a }
    // expect-error: Special variable declared
    declare_generator_element { ^{: 1, thisMethod <- [1] } }
    // expect-error: Special variable declared
    declare_generator_variable { ^{: 1, var thisFunction = 1 } }
}

SpecialMembers {
    var this;
    classvar thisFunctionDef = 1;
}
