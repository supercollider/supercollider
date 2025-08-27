// This first value caused a crash on some linux systems. 
[ 1810000, 3620000, 7240000].do { |n|
    var b = Pwhite().asStream;
    Array.fill(n, { b.next });
};
0.exit;
