var f = { 42 };
var escaped;
var g = { escaped = f.inEnvir; nil };
var arrays = Array.new(64);

g.();
// At this point in time, g's frame might be considered for deletion by the interpreter.

// Here we store the frame of g but access it through escaped's caller. As stated in the class file, this is not allowed...
// However we know this SHOULD be safe because escaped should still have a reference to it as it hasn't been called!
~escaped_frame = escaped.pr__unsafe__getFrame__();
64.do {  arrays.add([1, 2, 3, 4, 5, 6]) };

// This should be the frame of g that was created when we called it.
~gs_frame = ~escaped_frame.pr__unsafe__instVarAt(1);

// Ensure that it is still a frame, and that it still has the definition of g stored in it.
if ( ~gs_frame.pr__unsafe__isKindOf(Frame) and: { ~gs_frame.pr__unsafe__instVarAt(0) === g.def} ) {
    0.exit
} {
    1.exit
};
