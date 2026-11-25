f = {|a|a};
f = {|a, b|a + b};
f = {|...rest|rest};
f = {|a| -1}; // without the space, `|-` would be an operator
f = {| |1}; // `||` is an operator
f = {arg a; a};
x = a | b; // an operator, not a parameter list
