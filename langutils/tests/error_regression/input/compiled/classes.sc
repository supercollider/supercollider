// compiled
// A class's layout: its superclass, how its elements are stored, and its instance variables, the inherited ones first,
// each with its initial value. Class variables and constants are the meta class's own, numbered from its first class
// variable, and its accessors are shortcut methods.
Shape {
    classvar <count = 0, registry;
    const sides = 0;
    var <>name = "shape", <origin;
}

Square : Shape {
    classvar <>unit = 1.0;
    const corners_count = 4;
    var <size = 2, corners;
    // expect: no diagnostics
    area { ^size * size }
}

Samples[float] : Shape {
    var rate = 44100;
}
