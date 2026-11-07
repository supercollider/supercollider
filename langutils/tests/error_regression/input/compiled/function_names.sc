// compiled
// A method is named by its selector, a function by the variable or argument it initialises, the variable it is
// assigned to, or the setter it is a positional argument of, as the old compiler named them. Any other function has no
// name, including a setter's receiver, `{ 8 }.action_(1)`, which the old compiler named by mistake. Each nested function
// is in the function it is made in.
Names {
    var member;
    // expect: no diagnostics
    variables { var local = { 1 }, other; other = { 2 }; member = { 3 }; ~env = { 4 }; ^[local, other] }
    // expect: no diagnostics
    argument { |default = ({ 5 })| ^default }
    // expect: no diagnostics
    setters { this.action_({ 6 }); this.action = { 7 }; ^this }
    // expect: no diagnostics
    unnamed { this.action({ 9 }); this.action_(key: { 10 }); { 8 }.action_(1); ^[{ 11 }, 1 + { 12 }] }
    // expect: no diagnostics
    nested { var outer = { var inner = { |x| if (x) { ^1 }; ^2 }; inner }; ^outer }
    action { |f| ^f }
    action_ { |f| ^f }
}
